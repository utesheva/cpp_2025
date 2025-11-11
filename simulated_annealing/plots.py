import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def plot_heatmap():
    """Построение тепловой карты для последовательного алгоритма"""
    data = pd.read_csv('heatmap_data.csv')
    pivot_table = data.pivot(index='Jobs', columns='Processors', values='Time')
    
    plt.figure(figsize=(12, 8))
    sns.heatmap(pivot_table, annot=True, fmt='.1f', cmap='YlOrRd', cbar_kws={'label': 'Time (seconds)'})
    plt.title('Sequential Algorithm - Average Execution Time Heat Map')
    plt.xlabel('Number of Processors')
    plt.ylabel('Number of Jobs')
    plt.savefig('heat_map.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_parallel_scaling():
    """Построение графиков для параллельного алгоритма"""
    data = pd.read_csv('parallel_scaling.csv')
    
    # График времени выполнения
    plt.figure(figsize=(10, 6))
    plt.plot(data['Threads'], data['Time'], 'bo-', linewidth=2, markersize=8)
    plt.xlabel('Number of Threads (Nproc)')
    plt.ylabel('Average Execution Time (seconds)')
    plt.title('Parallel Algorithm - Execution Time vs Number of Threads')
    plt.grid(True, alpha=0.3)
    plt.savefig('parallel_time.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # График ускорения
    plt.figure(figsize=(10, 6))
    plt.plot(data['Threads'], data['Speedup'], 'go-', linewidth=2, markersize=8, label='Actual Speedup')
    plt.plot(data['Threads'], data['Threads'], 'r--', linewidth=1, label='Ideal Speedup')
    plt.xlabel('Number of Threads (Nproc)')
    plt.ylabel('Speedup')
    plt.title('Parallel Algorithm - Speedup vs Number of Threads')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('parallel_speedup.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # График эффективности
    plt.figure(figsize=(10, 6))
    plt.plot(data['Threads'], data['Efficiency'] * 100, 'mo-', linewidth=2, markersize=8)
    plt.xlabel('Number of Threads (Nproc)')
    plt.ylabel('Efficiency (%)')
    plt.title('Parallel Algorithm - Efficiency vs Number of Threads')
    plt.grid(True, alpha=0.3)
    plt.savefig('parallel_efficiency.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    plot_heatmap()
    plot_parallel_scaling()
