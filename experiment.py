import subprocess
import pandas as pd
import numpy as np
from sklearn.metrics import adjusted_rand_score, normalized_mutual_info_score
from sklearn.metrics import f1_score, fowlkes_mallows_score
from cdlib import evaluation, NodeClustering
from scipy.stats import entropy
import random
import os

# -----------------------------------------
# CONFIGURE PATHS
# -----------------------------------------
BASE = "/home/alek/CSE491-MTDS"
SYN = f"{BASE}/synGgen/syng"
MTDS = f"{BASE}/build/MTDS"

GRAPHS_DIR = f"{BASE}/TestGraphs/Graphs"
GT_DIR = f"{GRAPHS_DIR}/groundTruths"
SEED_DIR = f"{GRAPHS_DIR}/seeds"
PRED_DIR = f"{GRAPHS_DIR}/PredictedLabels"

# Ensure dirs exist
os.makedirs(GRAPHS_DIR, exist_ok=True)
os.makedirs(GT_DIR, exist_ok=True)
os.makedirs(SEED_DIR, exist_ok=True)
os.makedirs(PRED_DIR, exist_ok=True)

# -----------------------------------------
# METRICS FUNCTIONS
# -----------------------------------------

def purity_score(y_true, y_pred):
    labels_true = np.unique(y_true)
    labels_pred = np.unique(y_pred)
    matrix = np.zeros((len(labels_pred), len(labels_true)))

    for i, p in enumerate(labels_pred):
        for j, t in enumerate(labels_true):
            matrix[i, j] = np.sum((y_pred == p) & (y_true == t))

    return np.sum(np.max(matrix, axis=1)) / np.sum(matrix)


def cluster_entropy(labels):
    _, counts = np.unique(labels, return_counts=True)
    probs = counts / counts.sum()
    return entropy(probs, base=2)

def compute_gnmi(y_true, y_pred):
    # Convert labels → clusters
    clusters_true = {}
    clusters_pred = {}

    for i, c in enumerate(y_true):
        clusters_true.setdefault(c, []).append(i)

    for i, c in enumerate(y_pred):
        clusters_pred.setdefault(c, []).append(i)

    true_nc = NodeClustering(list(clusters_true.values()), graph=None, method_name="true")
    pred_nc = NodeClustering(list(clusters_pred.values()), graph=None, method_name="pred")

    score = evaluation.overlapping_normalized_mutual_information_LFK(pred_nc, true_nc).score
    return score

def fuzzy_ARI(U, V):
    n = U.shape[0]
    M = np.dot(U.T, U)
    N = np.dot(V.T, V)

    sum_comb_M = np.sum(M*(M-1)/2)
    sum_comb_N = np.sum(N*(N-1)/2)

    MN = np.dot(U.T, V)
    sum_comb_MN = np.sum(MN*(MN-1)/2)

    expected = (sum_comb_M * sum_comb_N) / (n*(n-1)/2)
    max_index = (sum_comb_M + sum_comb_N)/2

    return (sum_comb_MN - expected) / (max_index - expected)


def compute_fuzzy_ari(y_true, y_pred):
    # Convert hard labels → membership matrices
    classes_true = np.unique(y_true)
    classes_pred = np.unique(y_pred)

    onehot_true = np.zeros((len(y_true), len(classes_true)))
    onehot_pred = np.zeros((len(y_pred), len(classes_pred)))

    for i, c in enumerate(classes_true):
        onehot_true[y_true == c, i] = 1

    for i, c in enumerate(classes_pred):
        onehot_pred[y_pred == c, i] = 1

    return fuzzy_ARI(onehot_pred, onehot_true)

# --------------------------------------------------------------------------------------------------
# EXPERIMENT FUNCTIONS
# --------------------------------------------------------------------------------------------------

def full_grid(n_range, t_range, th_range, temp_range, alpha_range,
              norm_k=0.001, ktruss_range=(2,4)):
    experiments = []
    for n in n_range:
        for t in t_range:
            for th in th_range:
                for temp in temp_range:
                    for alpha in alpha_range:
                        for k in ktruss_range:
                            experiments.append({
                                "n": n,
                                "t": t,
                                "th": th,
                                "temp": temp,
                                "alpha": alpha,
                                "norm_k": norm_k,
                                "k_Truss": k
                            })
    return experiments


def random_tests(
        n_min, n_max,
        t_min, t_max,
        th_min, th_max,
        temp_min, temp_max,
        alpha_min, alpha_max,
        norm_k=0.001,
        k_min=2, k_max=4,
        count=100):

    experiments = []
    for _ in range(count):
        experiments.append({
            "n": random.randint(n_min, n_max),
            "t": random.randint(t_min, t_max),
            "th": round(random.uniform(th_min, th_max), 3),
            "temp": random.randint(temp_min, temp_max),
            "alpha": round(random.uniform(alpha_min, alpha_max), 3),
            "norm_k": norm_k,
            "k_Truss": random.randint(k_min, k_max)
        })
    return experiments

# --------------------------------------------------------------------------------------------------
# EXPERIMENT FUNCTIONS
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# EXPERIMENT CONFIGURATION
# --------------------------------------------------------------------------------------------------

# experiments = [
#     {"n": 30, "t": 2, "th": 0.8, "temp": 100, "alpha": 0.95, "norm_k" : 0.001, "k_Truss": 3},
#     {"n": 50, "t": 3, "th": 0.85, "temp": 200, "alpha": 0.90, "norm_k" : 0.001, "k_Truss": 3},
#     # ADD MORE EXPERIMENTS HERE
# ]

experiments = random_tests(
    n_min=30, n_max=50,
    t_min=3, t_max=4,
    th_min=0.75, th_max=0.95,
    temp_min=90, temp_max=130,
    alpha_min=0.8, alpha_max=0.99,
    count=5
)

# experiments = full_grid(
#     n_range=range(30, 301, 10),
#     t_range=range(3, 7),
#     th_range=np.arange(0.3, 0.81, 0.05),
#     temp_range=range(90, 251, 20),
#     alpha_range=np.arange(0.8, 1.0, 0.05),
#     norm_k=0.001,
#     ktruss_range=range(2, 5)
# )

# --------------------------------------------------------------------------------------------------
# DATAFRAME INIT
# --------------------------------------------------------------------------------------------------

df_cols = [
    "Name", "Nodes", "TDS_Count", "Density", "Temperature", "Alpha", "norm_k", "k_Truss",
    "ARI", "NMI", "GNMI", "FuzzyARI", "Purity", "H_true", "H_pred", "F_measure", "FM",
    "Generated_Subgraphs", "Predicted_Subgraphs"
]

results = []


# -----------------------------------------
# MAIN EXPERIMENT LOOP
# -----------------------------------------

for exp_id, exp in enumerate(experiments, start=1):

    print(f"\n=== Running Experiment {exp_id} ===")

    n = exp["n"]
    t = exp["t"]
    th = exp["th"]
    temp = exp["temp"]
    alpha = exp["alpha"]
    norm_k = exp["norm_k"]
    k_Truss = exp["k_Truss"]


    # File naming
    graph_file = f"Graph{exp_id}.txt"
    gt_file = f"Graph{exp_id}_ground_truth.txt"
    seed_file = f"Graph{exp_id}_seed.txt"
    pred_file = f"Graph{exp_id}_predicted_labels.txt"

    graph_path = f"{GRAPHS_DIR}/{graph_file}"
    gt_path = f"{GT_DIR}/{gt_file}"
    seed_path = f"{SEED_DIR}/{seed_file}"
    pred_path = f"{PRED_DIR}/{pred_file}"

    # ----------------------------
    # RUN SYNG (Graph Generator)
    # ----------------------------
    print("Running syng...")

    syng_cmd = [
        SYN, graph_path, gt_path,
        seed_path, str(n), str(t), str(th), str(norm_k), str(k_Truss) 
    ]

    syng_out = subprocess.check_output(syng_cmd, text=True)

    # Capture generated subgraph prints
    generated_subgraphs = syng_out.strip()

    # ----------------------------
    # RUN MTDS (Simulated Annealing)
    # ----------------------------
    print("Running MTDS...")

    mtds_cmd = [
        MTDS,
        f"graph={graph_path}",
        f"seed={seed_path}",
        f"density={th}",
        f"temperature={temp}",
        f"alpha={alpha}",
        f"normalize_const_k={norm_k}"
    ]

    mtds_out = subprocess.check_output(mtds_cmd, text=True)

    predicted_subgraphs = mtds_out.strip()

    # ----------------------------
    # RUN VALIDATION METRICS
    # ----------------------------
    print("Computing metrics...")

    gt = np.loadtxt(gt_path, dtype=int)
    pred = np.loadtxt(pred_path, dtype=int)

    assert np.array_equal(gt[:, 0], pred[:, 0]), "Node ID mismatch"

    y_true = gt[:, 1]
    y_pred = pred[:, 1]

    ari = adjusted_rand_score(y_true, y_pred)
    nmi = normalized_mutual_info_score(y_true, y_pred)
    gnmi = compute_gnmi(y_true, y_pred)
    fuzzyari = compute_fuzzy_ari(y_true, y_pred)
    purity = purity_score(y_true, y_pred)
    H_true = cluster_entropy(y_true)
    H_pred = cluster_entropy(y_pred)
    f_measure = f1_score(y_true, y_pred, average='macro')
    fm = fowlkes_mallows_score(y_true, y_pred)

    # ----------------------------
    # SAVE ROW
    # ----------------------------
    results.append([
        f"Graph{exp_id}", n, t, th, temp, alpha, norm_k, k_Truss, 
        ari, nmi, gnmi, fuzzyari, purity, H_true, H_pred, f_measure, fm,
        generated_subgraphs, predicted_subgraphs
    ])


# -----------------------------------------
# SAVE EXCEL
# -----------------------------------------

df = pd.DataFrame(results, columns=df_cols)
output_excel = f"{BASE}/experiment_results.xlsx"
# df.to_excel(output_excel, index=False)

# Check if the Excel file already exists
if os.path.exists(output_excel):
    print("\nExisting Excel found. Appending new results...")
    
    # 1. Read the existing DataFrame
    df_existing = pd.read_excel(output_excel)
    
    # 2. Concatenate the existing data with the new results
    df_combined = pd.concat([df_existing, df], ignore_index=True)
    
    # 3. Save the combined DataFrame back to the file (overwriting with combined data)
    df_combined.to_excel(output_excel, index=False)
else:
    print("No existing Excel found. Creating new file...")
    
    # If the file doesn't exist, just save the new results
    df.to_excel(output_excel, index=False)

print("\n=== ALL EXPERIMENTS COMPLETE ===")
print("Saved Excel at:", output_excel)
