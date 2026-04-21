import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

csv_file = 'benchmark_results.csv'

if not os.path.exists(csv_file):
    print(f"Error: {csv_file} not found. Cannot generate plot.")
    exit(1)

# Read the CSV file
df = pd.read_csv(csv_file)
df = df.sort_values(by=['filter_name', 'threads'])

# Setup seaborn style
sns.set_theme(style="whitegrid")
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
fig.suptitle('HPC OpenMP Performance Analysis', fontsize=18, fontweight='bold', y=1.05)

# Subplot 1: Speedup vs Threads
for filter_name in df['filter_name'].unique():
    subset = df[df['filter_name'] == filter_name]
    ax1.plot(subset['threads'], subset['speedup'], marker='o', linewidth=2, label=filter_name)

# Ideal Speedup reference line
max_threads = df['threads'].max()
ax1.plot([1, max_threads], [1, max_threads], 'k--', alpha=0.5, label='Ideal Linear Speedup')

ax1.set_xlabel('Number of Threads', fontsize=12)
ax1.set_ylabel('Speedup Factor', fontsize=12)
ax1.set_title("Amdahl's Law Actual Speedup", fontsize=14)
ax1.set_xticks(df['threads'].unique())
ax1.legend(loc='upper left')

# Subplot 2: Execution Time Comparison
for filter_name in df['filter_name'].unique():
    subset = df[df['filter_name'] == filter_name]
    ax2.plot(subset['threads'], subset['parallel_time'], marker='s', linewidth=2, label=filter_name)

ax2.set_xlabel('Number of Threads', fontsize=12)
ax2.set_ylabel('Execution Time (seconds)', fontsize=12)
ax2.set_title('Strong Scaling: Time vs Threads', fontsize=14)
ax2.set_xticks(df['threads'].unique())
ax2.legend(loc='upper right')
ax2.set_yscale('log') # Log scale is often better for execution time
plt.tight_layout()

# Save
output_file = 'speedup_graph.png'
plt.savefig(output_file, dpi=300, bbox_inches='tight')
print(f"Graph successfully saved as {output_file}")
