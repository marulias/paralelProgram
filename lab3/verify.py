import numpy as np
import os
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')

sizes = [200, 400, 800, 1200, 1600, 2000]
process_counts = [1, 2, 4, 8]

results = {}

for p in process_counts:
    filename = f"experiment_results_p{p}.txt"
    if os.path.exists(filename):
        times = []
        mflops = []
        with open(filename, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            for line in lines:
                parts = line.strip().split()
                if len(parts) >= 3 and parts[0].isdigit():
                    try:
                        times.append(float(parts[1]))
                        mflops.append(float(parts[2]))
                    except:
                        pass
        if len(times) == len(sizes):
            results[p] = {'times': times, 'mflops': mflops}

print("=" * 60)
print("ВЕРИФИКАЦИЯ РЕЗУЛЬТАТОВ")
print("=" * 60)

for p in process_counts:
    result_file = f"result_200_mpi_p{p}.txt"
    if os.path.exists(result_file):
        try:
            with open(result_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            lines = content.split('\n')
            n = 200
            
            A, B, C = None, None, None
            
            for i, line in enumerate(lines):
                if "Матрица A:" in line:
                    A = []
                    for j in range(i+1, i+1+n):
                        if j < len(lines) and lines[j].strip():
                            row = list(map(float, lines[j].split()))
                            A.append(row)
                    A = np.array(A)
            
            for i, line in enumerate(lines):
                if "Матрица B:" in line:
                    B = []
                    for j in range(i+1, i+1+n):
                        if j < len(lines) and lines[j].strip():
                            row = list(map(float, lines[j].split()))
                            B.append(row)
                    B = np.array(B)
            
            for i, line in enumerate(lines):
                if "Результат C:" in line:
                    C = []
                    for j in range(i+1, i+1+n):
                        if j < len(lines) and lines[j].strip():
                            row = list(map(float, lines[j].split()))
                            C.append(row)
                    C = np.array(C)
            
            if A is not None and B is not None and C is not None:
                expected = np.dot(A, B)
                is_close = np.allclose(C, expected, rtol=1e-6, atol=1e-8)
                if is_close:
                    print(f"p={p}: Верификация пройдена")
                else:
                    print(f"p={p}: Верификация НЕ пройдена")
            else:
                print(f"p={p}: Не удалось прочитать матрицы")
        except Exception as e:
            print(f"p={p}: Ошибка при проверке - {e}")
    else:
        print(f"p={p}: Файл {result_file} не найден")

print("\n" + "=" * 60)
print("ПОСТРОЕНИЕ ГРАФИКОВ")
print("=" * 60)

fig, axes = plt.subplots(2, 2, figsize=(14, 10))

colors = ['blue', 'green', 'red', 'orange']
markers = ['o', 's', '^', 'd']

# График 1: Время выполнения
ax1 = axes[0, 0]
for i, p in enumerate(process_counts):
    if p in results:
        ax1.plot(sizes, results[p]['times'], color=colors[i], marker=markers[i], 
                 linewidth=2, markersize=6, label=f'{p} процессов')
ax1.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax1.set_ylabel('Время (секунды)', fontsize=12)
ax1.set_title('Зависимость времени выполнения от размера матрицы', fontsize=14)
ax1.grid(True, alpha=0.3)
ax1.legend()
ax1.set_yscale('log')

# График 2: Производительность
ax2 = axes[0, 1]
for i, p in enumerate(process_counts):
    if p in results:
        ax2.plot(sizes, results[p]['mflops'], color=colors[i], marker=markers[i], 
                 linewidth=2, markersize=6, label=f'{p} процессов')
ax2.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax2.set_ylabel('Производительность (MFLOPS)', fontsize=12)
ax2.set_title('Зависимость производительности от размера матрицы', fontsize=14)
ax2.grid(True, alpha=0.3)
ax2.legend()

# График 3: Ускорение
ax3 = axes[1, 0]
if 1 in results:
    sequential_times = results[1]['times']
    for i, p in enumerate(process_counts):
        if p in results and p > 1:
            speedup = [sequential_times[j] / results[p]['times'][j] for j in range(len(sizes))]
            ax3.plot(sizes, speedup, color=colors[i], marker=markers[i], 
                     linewidth=2, markersize=6, label=f'{p} процессов')
ax3.plot(sizes, sizes, 'k--', linewidth=1, label='Линейное ускорение')
ax3.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax3.set_ylabel('Ускорение', fontsize=12)
ax3.set_title('Ускорение при параллельном вычислении', fontsize=14)
ax3.grid(True, alpha=0.3)
ax3.legend()

# График 4: Эффективность
ax4 = axes[1, 1]
if 1 in results:
    sequential_times = results[1]['times']
    for i, p in enumerate(process_counts):
        if p in results and p > 1:
            efficiency = [sequential_times[j] / (results[p]['times'][j] * p) for j in range(len(sizes))]
            ax4.plot(sizes, efficiency, color=colors[i], marker=markers[i], 
                     linewidth=2, markersize=6, label=f'{p} процессов')
ax4.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax4.set_ylabel('Эффективность', fontsize=12)
ax4.set_title('Эффективность параллельного вычисления', fontsize=14)
ax4.grid(True, alpha=0.3)
ax4.legend()
ax4.set_ylim(0, 1.8)

plt.tight_layout()
plt.savefig('performance_plot_mpi.png', dpi=150, bbox_inches='tight')
print("График сохранён как 'performance_plot_mpi.png'")

print("\n" + "=" * 70)
print("ТАБЛИЦА РЕЗУЛЬТАТОВ ЭКСПЕРИМЕНТОВ")
print("=" * 70)
print(f"{'Размер':^8} | ", end="")
for p in process_counts:
    print(f"{p} пр. время | {p} пр. MFLOPS | ", end="")
print()
print("-" * 70)

for i, size in enumerate(sizes):
    print(f"{size:^8} | ", end="")
    for p in process_counts:
        if p in results:
            print(f"{results[p]['times'][i]:^10.4f} | {results[p]['mflops'][i]:^12.2f} | ", end="")
        else:
            print(f"{'N/A':^10} | {'N/A':^12} | ", end="")
    print()

print("=" * 70)

with open('results_table_mpi.txt', 'w', encoding='utf-8') as f:
    f.write("ТАБЛИЦА РЕЗУЛЬТАТОВ ПАРАЛЛЕЛЬНОГО УМНОЖЕНИЯ МАТРИЦ (MPI)\n")
    f.write("=" * 80 + "\n")
    f.write(f"{'Размер':<10}")
    for p in process_counts:
        f.write(f"{p} процессов (сек)   {p} процессов (MFLOPS)   ")
    f.write("\n")
    f.write("-" * 80 + "\n")
    for i, size in enumerate(sizes):
        f.write(f"{size:<10}")
        for p in process_counts:
            if p in results:
                f.write(f"{results[p]['times'][i]:<18.4f}{results[p]['mflops'][i]:<20.2f}")
            else:
                f.write(f"{'N/A':<18}{'N/A':<20}")
        f.write("\n")
    f.write("=" * 80 + "\n")

print("\nТаблица сохранена как 'results_table_mpi.txt'")
print("\n" + "=" * 60)
print("ВСЕ ОПЕРАЦИИ ЗАВЕРШЕНЫ")
print("=" * 60)