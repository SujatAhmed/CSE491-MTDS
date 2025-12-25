import subprocess
import pandas as pd
import numpy as np
from sklearn.metrics import adjusted_rand_score, normalized_mutual_info_score
from sklearn.metrics import f1_score, fowlkes_mallows_score
from cdlib import evaluation, NodeClustering
from scipy.stats import entropy
from collections import defaultdict
import random
import os
import re
import sys

# -----------------------------------------
# CONFIGURE PATHS
# -----------------------------------------
BASE = "/home/sujat/projects/cse491"
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

def compute_gnmi_from_labels(y_true, y_pred):
    true_clusters = defaultdict(list)
    pred_clusters = defaultdict(list)

    for i, c in enumerate(y_true):
        if c != -1:
            true_clusters[c].append(i)

    for i, c in enumerate(y_pred):
        if c != -1:
            pred_clusters[c].append(i)

    true_nc = NodeClustering(list(true_clusters.values()), None, "true")
    pred_nc = NodeClustering(list(pred_clusters.values()), None, "pred")

    return evaluation.overlapping_normalized_mutual_information_LFK(
        pred_nc, true_nc
    ).score

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
# LOAD
# --------------------------------------------------------------------------------------------------



def build_pred_labels_for_gt(gt_file, mtds_stdout):
    """
    Return predicted labels aligned with GT nodes in file.
    """
    # 1. Load GT node IDs
    gt_nodes = []
    with open(gt_file, 'r') as f:
        for line in f:
            if line.strip():
                parts = line.strip().split()
                gt_nodes.append(int(parts[0]))

    # 2. Parse MTDS output
    labels_dict = {}  # node_id -> cluster_id
    pattern = re.compile(r"\{([^}]*)\}")
    cluster_id = 1  # cluster IDs start at 1
    for match in pattern.finditer(mtds_stdout):
        nodes = match.group(1).split()
        for node in nodes:
            labels_dict[int(node)] = cluster_id
        cluster_id += 1

    # 3. Build aligned y_pred
    y_pred = [labels_dict.get(node, -1) for node in gt_nodes]

    return y_pred



def load_labels(file_path):
    labels_list = []
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()

            # Skip debug / empty lines
            if not line:
                continue
            if not line[0].isdigit() and not line[0] == '-':
                continue

            parts = line.split()
            try:
                labels_list.append([int(x) for x in parts])
            except ValueError:
                continue
    return labels_list


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



graph_groups = defaultdict(list)

for exp in experiments:
    graph_key = (
        exp["n"],
        exp["t"],
        exp["th"],
        exp["k_Truss"],
        exp["norm_k"]
    )
    graph_groups[graph_key].append(exp)


# --------------------------------------------------------------------------------------------------
# DATAFRAME INIT
# --------------------------------------------------------------------------------------------------

df_cols = [
    "Name", "Nodes", "TDS_Count", "Density", "Temperature", "Alpha", "norm_k", "k_Truss",
    "ARI", "NMI", "GNMI", "FuzzyARI", "Purity", "H_true", "H_pred", "F_measure", "FM",
    "Generated_Subgraphs", "Predicted_Subgraphs"
]

results = []


# ------------------------------------------------------------------------------------------------------------------------
# MAIN EXPERIMENT LOOP
# ------------------------------------------------------------------------------------------------------------------------

graph_id = 1

for (n, t, th, k_Truss, norm_k), mtds_exps in graph_groups.items():

    print(f"\n=== Generating Graph{graph_id} ===")

    # File naming (ONE graph)
    graph_file = f"Graph{graph_id}.txt"
    gt_file = f"Graph{graph_id}_ground_truth.txt"
    seed_file = f"Graph{graph_id}_seed.txt"

    graph_path = f"{GRAPHS_DIR}/{graph_file}"
    gt_path = f"{GT_DIR}/{gt_file}"
    seed_path = f"{SEED_DIR}/{seed_file}"

    # ----------------------------
    # RUN SYNG ONCE
    # ----------------------------
    syng_cmd = [
        SYN, graph_path, gt_path,
        seed_path, str(n), str(t), str(th),
        str(norm_k), str(k_Truss)
    ]

    syng_out = subprocess.check_output(syng_cmd, text=True)
    generated_subgraphs = syng_out.strip()

    # ----------------------------
    # RUN MTDS MULTIPLE TIMES
    # ----------------------------
    for exp in mtds_exps:

        temp = exp["temp"]
        alpha = exp["alpha"]

        print(f"Running MTDS: T={temp}, α={alpha}")

        pred_file = f"Graph{graph_id}_T{temp}_A{alpha}_predicted_labels.txt"
        pred_path = f"{PRED_DIR}/{pred_file}"

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
        y_true_list = load_labels(gt_path)

        y_true = [row[1] if row else -1 for row in y_true_list]
        y_pred = build_pred_labels_for_gt(gt_path, predicted_subgraphs)

        if len(y_true) != len(y_pred):
            raise RuntimeError(
                f"Label length mismatch: GT={len(y_true)} PRED={len(y_pred)}"
        )

        ari = adjusted_rand_score(y_true, y_pred)
        nmi = normalized_mutual_info_score(y_true, y_pred)
        gnmi = compute_gnmi_from_labels(y_true, y_pred)
        fuzzyari = compute_fuzzy_ari(y_true, y_pred)
        purity = purity_score(y_true, y_pred)
        H_true = cluster_entropy(y_true)
        H_pred = cluster_entropy(y_pred)
        f_measure = f1_score(
            np.array(y_true) != -1,
            np.array(y_pred) != -1
        )
        fm = fowlkes_mallows_score(y_true, y_pred)

        results.append([
            f"Graph{graph_id}", n, t, th, temp, alpha, norm_k, k_Truss,
            ari, nmi, gnmi, fuzzyari, purity, H_true, H_pred, f_measure, fm,
            generated_subgraphs, predicted_subgraphs
        ])

    graph_id += 1


# -----------------------------------------
# SAVE EXCEL
# -----------------------------------------

df = pd.DataFrame(results, columns=df_cols)
df = df.round(3)
output_excel = f"{BASE}/experiment_results.xlsx"
# df.to_excel(output_excel, index=False)

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
