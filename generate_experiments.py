from itertools import product

# Sorted input lists (examples; replace with your actual lists)
n_list = [ 50, 100, 150, 200, 300 ]
theta_list = [ 0.8, 0.9 ]
TDS_list = [ 3, 4, 5 ]
k_truss_list = [ 3, 4 ]
T_list = [ 70, 100, 200 ]
alpha_list = [ 0.10, 0.50, 0.80, 0.90, 0.99 ]

# Generate all combinations
# Order matters: n (most significant) → alpha (least significant)
Experiments = [
    [n, theta, TDS, k_truss, T, alpha]
    for n, theta, TDS, k_truss, T, alpha
    in product(
        n_list,
        theta_list,
        TDS_list,
        k_truss_list,
        T_list,
        alpha_list
    )
]

output_path = "experiments.txt"

# Write all experiments to the output file
with open(output_path, "w", encoding="utf-8") as f:
    for exp in Experiments:
        f.write(", ".join(map(str, exp)) + "\n")

# Example: print first few experiments
for exp in Experiments[: len(Experiments)]:
    print(exp)

# Total number of combinations
print("Total experiments:", len(Experiments))
print(f"All experiments written to {output_path}")
