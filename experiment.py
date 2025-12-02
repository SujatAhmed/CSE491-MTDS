import subprocess
import pandas as pd
import numpy as np
from sklearn.metrics import adjusted_rand_score, normalized_mutual_info_score
from sklearn.metrics import f1_score, fowlkes_mallows_score
from scipy.stats import entropy
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


# -----------------------------------------
# EXPERIMENT CONFIGURATION
# -----------------------------------------

experiments = [
    {"n": 30, "t": 4, "th": 0.6, "temp": 100, "alpha": 0.95},
    {"n": 50, "t": 5, "th": 0.7, "temp": 200, "alpha": 0.90},
    # ADD MORE EXPERIMENTS HERE
]

# -----------------------------------------
# DATAFRAME INIT
# -----------------------------------------

df_cols = [
    "Name", "Nodes", "TDS_Count", "Density", "Temperature", "Alpha",
    "ARI", "NMI", "Purity", "H_true", "H_pred", "F_measure", "FM",
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
        seed_path, str(n), str(t), str(th)
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
        f"alpha={alpha}"
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
    purity = purity_score(y_true, y_pred)
    H_true = cluster_entropy(y_true)
    H_pred = cluster_entropy(y_pred)
    f_measure = f1_score(y_true, y_pred, average='macro')
    fm = fowlkes_mallows_score(y_true, y_pred)

    # ----------------------------
    # SAVE ROW
    # ----------------------------
    results.append([
        f"Graph{exp_id}", n, t, th, temp, alpha,
        ari, nmi, purity, H_true, H_pred, f_measure, fm,
        generated_subgraphs, predicted_subgraphs
    ])


# -----------------------------------------
# SAVE EXCEL
# -----------------------------------------

df = pd.DataFrame(results, columns=df_cols)
output_excel = f"{BASE}/experiment_results.xlsx"
df.to_excel(output_excel, index=False)

print("\n=== ALL EXPERIMENTS COMPLETE ===")
print("Saved Excel at:", output_excel)
