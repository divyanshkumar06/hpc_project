import csv
import json
import os

csv_file = 'benchmark_results.csv'
js_file = 'dashboard/data.js'

def generate_data():
    if not os.path.exists(csv_file):
        print(f"Error: {csv_file} not found. Cannot generate dashboard data.")
        return False
        
    # Ensure dashboard dir exists
    os.makedirs('dashboard', exist_ok=True)

    metrics = []
    
    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            metrics.append({
                'filter': row['filter_name'],
                'threads': int(row['threads']),
                'serialTime': float(row['serial_time']),
                'parallelTime': float(row['parallel_time']),
                'speedup': float(row['speedup']),
                'efficiency': float(row['efficiency'])
            })
            
    # Structure for the JS dashboard
    dashboard_data = {
        'cpuCores': 8, # Assumed based on the AMD Ryzen 7 config
        'metrics': metrics
    }
    
    # Write to a .js file so it can be loaded locally without a server (avoiding CORS issues)
    with open(js_file, 'w') as f:
        f.write(f"const dashboardData = {json.dumps(dashboard_data, indent=4)};\n")
        
    print(f"Successfully generated {js_file}")
    return True

if __name__ == "__main__":
    generate_data()
