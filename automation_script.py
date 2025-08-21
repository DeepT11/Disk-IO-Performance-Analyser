import subprocess
import csv
import re
import matplotlib.pyplot as plt

# Path to your compiled analyzer binary
ANALYZER_BINARY = "output/disk_io_analyzer"

# Regex to parse analyzer output lines
pattern = re.compile(r"\[(WRITE|READ )\] Block size: (\d+) KB, Time: ([\d.]+)s, Speed: ([\d.]+) MB/s")

results = []

print("Running Disk I/O Analyzer...\n")
process = subprocess.Popen([ANALYZER_BINARY], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

for line in process.stdout:
    line = line.strip()
    match = pattern.search(line)
    if match:
        op_type, block_kb, time_s, speed_mb = match.groups()
        results.append({
            "operation": op_type.strip(),
            "block_kb": int(block_kb),
            "time_s": float(time_s),
            "speed_mb": float(speed_mb)
        })
        print(line)

process.wait()

# Save results to CSV
csv_file = "io_benchmark_results.csv"
with open(csv_file, "w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=["operation", "block_kb", "time_s", "speed_mb"])
    writer.writeheader()
    writer.writerows(results)

print(f"\n Results saved to {csv_file}")

# Plot results
def plot_results():
    write_data = [r for r in results if r["operation"] == "WRITE"]
    read_data = [r for r in results if r["operation"] == "READ"]

    plt.figure(figsize=(8,5))
    plt.plot([r["block_kb"] for r in write_data], [r["speed_mb"] for r in write_data], marker='o', label="Write Speed")
    plt.plot([r["block_kb"] for r in read_data], [r["speed_mb"] for r in read_data], marker='s', label="Read Speed")

    plt.xscale("log", base=2)  # Block size grows exponentially
    plt.xlabel("Block Size (KB)")
    plt.ylabel("Throughput (MB/s)")
    plt.title("Disk I/O Performance")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig("io_benchmark_plot.png")
    plt.show()

plot_results()