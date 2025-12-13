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
BASE = "/home/sujat/projects/cse491/"
SYN = f"{BASE}/synGgen/syng"
MTDS = f"{BASE}/build/MTDS"

GRAPHS_DIR = f"{BASE}/TestGraphs/Graphs"
GT_DIR = f"{GRAPHS_DIR}/groundTruths"
SEED_DIR = f"{GRAPHS_DIR}/seeds"
PRED_DIR = f"{GRAPHS_DIR}/PredictedLabels"


EXPERIMENTS_FILE = os.path.join(os.path.dirname(__file__), "experiments.txt")

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



# --------------------------------------------------------------------------------------------------
# EXPERIMENT FUNCTIONS
# --------------------------------------------------------------------------------------------------



def load_experiments_from_file(path, norm_k=0.001):
    experiments = []
    with open(path, "r", encoding="utf-8") as f:
        for line_no, raw_line in enumerate(f, start=1):
            line = raw_line.strip()
            if not line:
                continue

            parts = [p.strip() for p in line.split(",")]
            if len(parts) != 6:
                raise ValueError(f"Line {line_no}: expected 6 values, got {len(parts)}")

            n, th, t, k_truss, temp, alpha = parts
            experiments.append({
                "n": int(n),
                "t": int(t),
                "th": float(th),
                "temp": int(temp),
                "alpha": float(alpha),
                "norm_k": norm_k,
                "k_Truss": int(k_truss)
            })
    return experiments


experiments = load_experiments_from_file(EXPERIMENTS_FILE)

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
    norm_k = 0.001
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

    def load_labels(path):
        with open(path, 'r') as f:
            # Read lines, split by space, convert to int, take all cols except the first (node ID)
            return [list(map(int, line.strip().split()))[1:] for line in f]

    # Inside the loop:
    y_true_list = load_labels(gt_path)  # Returns list of lists
    y_pred_list = load_labels(pred_path)

    # Note: Metrics like ARI/NMI in sklearn generally expect 1 label per item. 
    # For overlapping metrics (like your GNMI/FuzzyARI), you need to process these lists specifically.
    # For standard ARI/NMI, you might need to flatten or pick the primary label:
    y_true = [row[0] if row else -1 for row in y_true_list]
    y_pred = [row[0] if row else -1 for row in y_pred_list]

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
df = df.round(3)
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


# Check if the Excel file already exists
if os.path.exists(output_excel):
    print("\nExisting Excel found. Appending new results...")

    # 1. Read the existing DataFrame
    df_existing = pd.read_excel(output_excel)

    # 2. Concatenate the existing data with the new results
    df_combined = pd.concat([df_existing, df], ignore_index=True)

    # 3. Save the combined DataFrame back to the file (overwriting with combined data)
    df_combined.to_excel(output_excel, index=False)

    final_df = df_combined
else:
    print("No existing Excel found. Creating new file...")

    # If the file doesn't exist, just save the new results
    df.to_excel(output_excel, index=False)

    final_df = df

# -----------------------------------------
# CREATE BEST ROWS FOR EACH COMBINATION
# -----------------------------------------

# Define the grouping columns
grouping_cols = ["Nodes", "TDS_Count", "Density"]

# Function to get best rows for each metric
def get_best_rows_by_metric(df, metric_col):
    """Get the row with the best metric value for each unique combination of grouping columns."""
    best_rows = []
    
    # Group by the specified columns
    for group_key, group_df in df.groupby(grouping_cols):
        # Find the row with maximum metric value
        best_idx = group_df[metric_col].idxmax()
        best_row = group_df.loc[best_idx].copy()
        
        # Add a column to indicate what this row represents
        best_row["Selection_Criteria"] = f"Best_{metric_col}"
        
        best_rows.append(best_row)
    
    return pd.DataFrame(best_rows)

# Get best GNMI rows
best_gnmi_df = get_best_rows_by_metric(final_df, "GNMI")

# Get best FuzzyARI rows  
best_fuzzyari_df = get_best_rows_by_metric(final_df, "FuzzyARI")

# Sort both dataframes by Nodes, TDS_Count, Density for better readability
best_gnmi_df = best_gnmi_df.sort_values(by=grouping_cols).reset_index(drop=True)
best_fuzzyari_df = best_fuzzyari_df.sort_values(by=grouping_cols).reset_index(drop=True)

# -----------------------------------------
# SAVE TO EXCEL WITH MULTIPLE SHEETS
# -----------------------------------------

print("\nSaving results to Excel with multiple sheets...")

with pd.ExcelWriter(output_excel, engine='openpyxl') as writer:
    # Save all results to "All_Results" sheet
    final_df.to_excel(writer, sheet_name='All_Results', index=False)
    
    # Save best GNMI rows to "Best_GNMI" sheet
    best_gnmi_df.to_excel(writer, sheet_name='Best_GNMI', index=False)
    
    # Save best FuzzyARI rows to "Best_FuzzyARI" sheet
    best_fuzzyari_df.to_excel(writer, sheet_name='Best_FuzzyARI', index=False)
    
    # Optional: Add a summary sheet
    summary_data = {
        "Sheet": ["All_Results", "Best_GNMI", "Best_FuzzyARI"],
        "Rows": [len(final_df), len(best_gnmi_df), len(best_fuzzyari_df)],
        "Description": [
            "All experimental results",
            "Best row for each (Nodes, TDS_Count, Density) combination based on GNMI",
            "Best row for each (Nodes, TDS_Count, Density) combination based on FuzzyARI"
        ]
    }
    pd.DataFrame(summary_data).to_excel(writer, sheet_name='Summary', index=False)

print("\n=== ALL EXPERIMENTS COMPLETE ===")
print(f"Saved Excel at: {output_excel}")
print(f"\nSheet breakdown:")
print(f"1. All_Results: {len(final_df)} rows (all experiment data)")
print(f"2. Best_GNMI: {len(best_gnmi_df)} rows (best GNMI for each parameter combination)")
print(f"3. Best_FuzzyARI: {len(best_fuzzyari_df)} rows (best FuzzyARI for each parameter combination)")
print(f"4. Summary: Overview of all sheets")

# Optional: Display sample of best rows
print(f"\nSample of Best_GNMI rows (first 5):")
print(best_gnmi_df[["Name", "Nodes", "TDS_Count", "Density", "GNMI"]].head().to_string(index=False))

print(f"\nSample of Best_FuzzyARI rows (first 5):")
print(best_fuzzyari_df[["Name", "Nodes", "TDS_Count", "Density", "FuzzyARI"]].head().to_string(index=False))
