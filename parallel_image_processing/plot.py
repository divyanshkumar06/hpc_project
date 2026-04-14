import pandas as pd
import matplotlib.pyplot as plt
import os

csv_file = 'benchmark_results.csv'

if not os.path.exists(csv_file):
    print(f"Error: {csv_file} not found. Cannot generate plot.")
    exit(1)

# Read the CSV file
df = pd.read_csv(csv_file, names=['Threads', 'SerialTime', 'ParallelTime', 'Speedup', 'Efficiency'])

# Sort by threads just in case
df = df.sort_values(by='Threads')

# Create a figure with two subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
fig.suptitle('OpenMP Parallelization Performance', fontsize=16)

# Subplot 1: Ideal vs Actual Speedup
ax1.plot(df['Threads'], df['Threads'], 'k--', label='Ideal Speedup')
ax1.plot(df['Threads'], df['Speedup'], 'b-o', label='Actual Speedup', linewidth=2)
ax1.set_xlabel('Number of Threads', fontsize=12)
ax1.set_ylabel('Speedup', fontsize=12)
ax1.set_title('Speedup Analysis', fontsize=14)
ax1.set_xticks(df['Threads'])
ax1.grid(True, linestyle='--', alpha=0.7)
ax1.legend()

# Subplot 2: Execution Time Comparison
ax2.plot(df['Threads'], df['SerialTime'], 'r--', label='Serial Time (Base)')
ax2.plot(df['Threads'], df['ParallelTime'], 'g-o', label='Parallel Time', linewidth=2)
ax2.set_xlabel('Number of Threads', fontsize=12)
ax2.set_ylabel('Execution Time (seconds)', fontsize=12)
ax2.set_title('Execution Time vs Threads', fontsize=14)
ax2.set_xticks(df['Threads'])
ax2.grid(True, linestyle='--', alpha=0.7)
ax2.legend()

plt.tight_layout()
plt.subplots_adjust(top=0.9)

output_file = 'speedup_graph.png'
plt.savefig(output_file, dpi=300)
print(f"Graph successfully saved as {output_file}")
