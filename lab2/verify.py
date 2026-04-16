import numpy as np
import os
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')

sizes = [200, 400, 800, 1200, 1600, 2000]
threads = [1, 2, 4, 8]

times = {
    1: [0.0115, 0.1020, 0.7831, 5.9135, 14.9694, 48.3677],
    2: [0.0110, 0.0670, 0.6170, 2.6868, 6.8647, 27.2400],
    4: [0.0070, 0.0480, 0.3526, 1.5041, 4.1819, 19.4324],
    8: [0.0050, 0.0380, 0.4260, 1.9587, 5.8016, 22.8245]
}

mflops = {
    1: [1392.27, 1254.80, 1307.69, 584.42, 547.25, 330.80],
    2: [1454.81, 1910.33, 1659.53, 1286.31, 1193.35, 587.37],
    4: [2285.39, 2667.56, 2904.32, 2297.70, 1958.93, 823.37],
    8: [3203.20, 3367.98, 2403.58, 1764.45, 1412.02, 701.00]
}

print("=" * 70)
print("ПОСТРОЕНИЕ ГРАФИКОВ")
print("=" * 70)

fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))

colors = ['blue', 'green', 'orange', 'red']

for idx, t in enumerate(threads):
    ax1.plot(sizes, times[t], color=colors[idx], linewidth=2, label=f'{t} потоков')
ax1.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax1.set_ylabel('Время (секунды)', fontsize=12)
ax1.set_title('Зависимость времени выполнения от размера матрицы', fontsize=14)
ax1.grid(True, alpha=0.3)
ax1.legend()
ax1.set_xscale('log')
ax1.set_yscale('log')

for idx, t in enumerate(threads):
    ax2.plot(sizes, mflops[t], color=colors[idx], linewidth=2, label=f'{t} потоков')
ax2.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax2.set_ylabel('Производительность (MFLOPS)', fontsize=12)
ax2.set_title('Зависимость производительности от размера матрицы', fontsize=14)
ax2.grid(True, alpha=0.3)
ax2.legend()

serial_times = times[1]
for idx, t in enumerate(threads):
    if t != 1:
        speedup = [serial_times[i] / times[t][i] for i in range(len(sizes))]
        ax3.plot(sizes, speedup, color=colors[idx], linewidth=2, label=f'{t} потоков')
ax3.plot(sizes, [2] * len(sizes), 'k--', linewidth=2, label='Идеальное (2)')
ax3.plot(sizes, [4] * len(sizes), 'k--', linewidth=2, label='Идеальное (4)')
ax3.plot(sizes, [8] * len(sizes), 'k--', linewidth=2, label='Идеальное (8)')
ax3.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax3.set_ylabel('Ускорение', fontsize=12)
ax3.set_title('Ускорение при параллельном выполнении', fontsize=14)
ax3.grid(True, alpha=0.3)
ax3.legend()

for idx, t in enumerate(threads):
    if t != 1:
        speedup = [serial_times[i] / times[t][i] for i in range(len(sizes))]
        efficiency = [s / t for s in speedup]
        ax4.plot(sizes, efficiency, color=colors[idx], linewidth=2, label=f'{t} потоков')
ax4.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax4.set_ylabel('Эффективность', fontsize=12)
ax4.set_title('Эффективность параллельного выполнения', fontsize=14)
ax4.grid(True, alpha=0.3)
ax4.legend()
ax4.set_ylim(0, 1.1)

plt.tight_layout()
plt.savefig('parallel_performance_plot.png', dpi=150, bbox_inches='tight')
print(" График сохранён как 'parallel_performance_plot.png'")

fig2, ax = plt.subplots(figsize=(10, 6))

for idx, t in enumerate(threads):
    if t != 1:
        speedup = [times[1][i] / times[t][i] for i in range(len(sizes))]
        ax.plot(sizes, speedup, color=colors[idx], linewidth=2, label=f'{t} потоков')
ax.plot(sizes, [2] * len(sizes), 'k--', linewidth=2, label='Идеальное (2)')
ax.plot(sizes, [4] * len(sizes), 'k--', linewidth=2, label='Идеальное (4)')
ax.plot(sizes, [8] * len(sizes), 'k--', linewidth=2, label='Идеальное (8)')
ax.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax.set_ylabel('Ускорение', fontsize=12)
ax.set_title('Ускорение при параллельном умножении матриц (OpenMP)', fontsize=14)
ax.grid(True, alpha=0.3)
ax.legend()

plt.tight_layout()
plt.savefig('speedup_plot.png', dpi=150, bbox_inches='tight')
print(" График ускорения сохранён как 'speedup_plot.png'")

print("\n" + "=" * 90)
print("ТАБЛИЦА РЕЗУЛЬТАТОВ")
print("=" * 90)

print(f"\n{'Размер':^8} |", end="")
for t in threads:
    print(f"{'Время ('+str(t)+'п)':^12} |", end="")
print()

print("-" * (8 + 15 * len(threads)))

for idx, s in enumerate(sizes):
    print(f"{s:^8} |", end="")
    for t in threads:
        print(f"{times[t][idx]:^12.4f} |", end="")
    print()

print("\n" + "-" * 90)

print(f"\n{'Размер':^8} |", end="")
for t in threads:
    if t != 1:
        print(f"{'Ускорение ('+str(t)+'п)':^15} |", end="")
print()

print("-" * (8 + 17 * (len(threads)-1)))

for idx, s in enumerate(sizes):
    print(f"{s:^8} |", end="")
    for t in threads:
        if t != 1:
            speedup = times[1][idx] / times[t][idx]
            print(f"{speedup:^15.2f} |", end="")
    print()

print("\n" + "=" * 90)

with open('parallel_results_table.txt', 'w', encoding='utf-8') as f:
    f.write("=" * 90 + "\n")
    f.write("РЕЗУЛЬТАТЫ ПАРАЛЛЕЛЬНОГО УМНОЖЕНИЯ МАТРИЦ (OpenMP)\n")
    f.write("=" * 90 + "\n\n")
    
    f.write("ВРЕМЯ ВЫПОЛНЕНИЯ (секунды)\n")
    f.write("-" * 90 + "\n")
    f.write(f"{'Размер':<10}")
    for t in threads:
        f.write(f"{f'Потоков {t}':<15}")
    f.write("\n")
    f.write("-" * 90 + "\n")
    
    for idx, s in enumerate(sizes):
        f.write(f"{s:<10}")
        for t in threads:
            f.write(f"{times[t][idx]:<15.4f}")
        f.write("\n")
    
    f.write("\n" + "-" * 90 + "\n")
    f.write("ПРОИЗВОДИТЕЛЬНОСТЬ (MFLOPS)\n")
    f.write("-" * 90 + "\n")
    f.write(f"{'Размер':<10}")
    for t in threads:
        f.write(f"{f'Потоков {t}':<15}")
    f.write("\n")
    f.write("-" * 90 + "\n")
    
    for idx, s in enumerate(sizes):
        f.write(f"{s:<10}")
        for t in threads:
            f.write(f"{mflops[t][idx]:<15.2f}")
        f.write("\n")
    
    f.write("\n" + "-" * 90 + "\n")
    f.write("УСКОРЕНИЕ\n")
    f.write("-" * 90 + "\n")
    f.write(f"{'Размер':<10}")
    for t in threads:
        if t != 1:
            f.write(f"{f'Потоков {t}':<15}")
    f.write("\n")
    f.write("-" * 90 + "\n")
    
    for idx, s in enumerate(sizes):
        f.write(f"{s:<10}")
        for t in threads:
            if t != 1:
                speedup = times[1][idx] / times[t][idx]
                f.write(f"{speedup:<15.2f}")
        f.write("\n")
    
    f.write("\n" + "=" * 90 + "\n")

print("\n Таблица сохранена как 'parallel_results_table.txt'")
print("\n" + "=" * 70)
print("ВСЕ ОПЕРАЦИИ ЗАВЕРШЕНЫ")
print("=" * 70)