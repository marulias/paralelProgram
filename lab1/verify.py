"""
Верификация результатов умножения матриц с помощью NumPy
и построение графиков зависимости времени и производительности
"""

import numpy as np
import os
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Для сохранения без GUI

# Данные из ваших экспериментов (из отчета)
sizes = [200, 400, 800, 1200, 1600, 2000]
times = [0.0940, 0.7386, 6.1112, 30.4954, 80.4029, 452.0348]
mflops = [170.20, 173.30, 167.56, 113.33, 101.89, 35.40]

# Конвертируем MFLOPS в GFLOPS для красоты
gflops = [m / 1000 for m in mflops]

print("=" * 60)
print("ВЕРИФИКАЦИЯ РЕЗУЛЬТАТОВ УМНОЖЕНИЯ МАТРИЦ")
print("=" * 60)

all_passed = True

# Проверяем только размер 200 (как в вашей программе)
test_size = 200
result_file = "result_200.txt"

if not os.path.exists(result_file):
    print(f"ОШИБКА: Файл {result_file} не найден!")
    print("Сначала запустите matrix_mult.exe для генерации результатов")
    all_passed = False
else:
    print(f"\nПроверка размера {test_size}×{test_size}...")
    
    try:
        # Читаем матрицы из файла result_200.txt
        with open(result_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Ищем матрицу A
        if "Матрица A:" in content:
            lines = content.split('\n')
            for i, line in enumerate(lines):
                if "Матрица A:" in line:
                    n = int(lines[i-2].split('x')[0]) if i-2 >= 0 else test_size
                    A = []
                    for j in range(i+1, i+1+n):
                        if j < len(lines) and lines[j].strip():
                            row = list(map(float, lines[j].split()))
                            A.append(row)
                    A = np.array(A)
                    break
        
        # Ищем матрицу B
        for i, line in enumerate(lines):
            if "Матрица B:" in line:
                B = []
                for j in range(i+1, i+1+n):
                    if j < len(lines) and lines[j].strip():
                        row = list(map(float, lines[j].split()))
                        B.append(row)
                B = np.array(B)
                break
        
        # Ищем матрицу C (результат)
        for i, line in enumerate(lines):
            if "Результат C:" in line:
                C = []
                for j in range(i+1, i+1+n):
                    if j < len(lines) and lines[j].strip():
                        row = list(map(float, lines[j].split()))
                        C.append(row)
                C = np.array(C)
                break
        
        # Вычисляем ожидаемый результат через NumPy
        expected = np.dot(A, B)
        
        # Сравниваем
        abs_diff = np.abs(C - expected)
        max_abs_diff = np.max(abs_diff)
        max_rel_diff = np.max(np.abs((C - expected) / (np.abs(expected) + 1e-12)))
        is_close = np.allclose(C, expected, rtol=1e-6, atol=1e-8)
        
        if is_close:
            print(f"   ✅ Верификация пройдена!")
            print(f"   Макс. абсолютная ошибка: {max_abs_diff:.2e}")
            print(f"   Макс. относительная ошибка: {max_rel_diff:.2e}")
        else:
            print(f"   ❌ Верификация НЕ пройдена!")
            print(f"   Макс. абсолютная ошибка: {max_abs_diff:.2e}")
            print(f"   Макс. относительная ошибка: {max_rel_diff:.2e}")
            all_passed = False
            
    except Exception as e:
        print(f"   ❌ Ошибка при проверке: {e}")
        all_passed = False

print("\n" + "=" * 60)
if all_passed:
    print("✅ ВСЕ ПРОВЕРКИ ПРОЙДЕНЫ УСПЕШНО!")
else:
    print("❌ ОБНАРУЖЕНЫ ОШИБКИ ПРИ ВЕРИФИКАЦИИ!")
print("=" * 60)

# Строим графики на основе ваших экспериментальных данных
print("\n" + "=" * 60)
print("ПОСТРОЕНИЕ ГРАФИКОВ")
print("=" * 60)

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# График 1: Время выполнения
ax1.plot(sizes, times, 'b-o', linewidth=2, markersize=8, label='Время выполнения')
ax1.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax1.set_ylabel('Время (секунды)', fontsize=12)
ax1.set_title('Зависимость времени выполнения от размера матрицы', fontsize=14)
ax1.grid(True, alpha=0.3)
ax1.legend()

# Добавляем подписи точек
for i, (x, y) in enumerate(zip(sizes, times)):
    ax1.annotate(f'{y:.3f}s', (x, y), textcoords="offset points", 
                 xytext=(0, 10), ha='center', fontsize=9)

# Теоретическая кривая O(n³)
n3_fit = [times[-1] * (s / sizes[-1])**3 for s in sizes]
ax1.plot(sizes, n3_fit, 'r--', linewidth=2, label='Теоретическая O(n³)')
ax1.legend()

# График 2: Производительность (в MFLOPS)
ax2.plot(sizes, mflops, 'g-s', linewidth=2, markersize=8, label='Производительность')
ax2.set_xlabel('Размер матрицы (n×n)', fontsize=12)
ax2.set_ylabel('Производительность (MFLOPS)', fontsize=12)
ax2.set_title('Зависимость производительности от размера матрицы', fontsize=14)
ax2.grid(True, alpha=0.3)
ax2.legend()

# Добавляем подписи точек
for i, (x, y) in enumerate(zip(sizes, mflops)):
    ax2.annotate(f'{y:.1f} MFLOPS', (x, y), textcoords="offset points", 
                 xytext=(0, 10), ha='center', fontsize=9)

plt.tight_layout()

# Сохраняем график
plt.savefig('performance_plot.png', dpi=150, bbox_inches='tight')
print("✅ График сохранён как 'performance_plot.png'")

try:
    plt.show()
except:
    print("   (График сохранён в файл, откройте его для просмотра)")

# Выводим таблицу результатов
print("\n" + "=" * 70)
print("ТАБЛИЦА РЕЗУЛЬТАТОВ ЭКСПЕРИМЕНТОВ")
print("=" * 70)
print(f"{'Размер матрицы':^15} | {'Время (сек)':^12} | {'Производительность (MFLOPS)':^25}")
print("-" * 70)
for size, time_sec, mflop in zip(sizes, times, mflops):
    print(f"{size:^15} | {time_sec:^12.6f} | {mflop:^25.2f}")
print("=" * 70)

# Сохраняем таблицу в файл
with open('results_table.txt', 'w', encoding='utf-8') as f:
    f.write("ТАБЛИЦА РЕЗУЛЬТАТОВ УМНОЖЕНИЯ КВАДРАТНЫХ МАТРИЦ\n")
    f.write("=" * 70 + "\n")
    f.write(f"{'Размер матрицы':<15} {'Время (сек)':<12} {'Производительность (MFLOPS)':<20}\n")
    f.write("-" * 70 + "\n")
    for size, time_sec, mflop in zip(sizes, times, mflops):
        f.write(f"{size:<15} {time_sec:<12.6f} {mflop:<20.2f}\n")
    f.write("=" * 70 + "\n")
    
    # Добавляем выводы
    f.write("\nВЫВОДЫ:\n")
    f.write("1. С увеличением размера матрицы время выполнения растет кубически O(n³)\n")
    f.write(f"2. При размере 200×200 время составило {times[0]:.4f} сек\n")
    f.write(f"3. При размере 2000×2000 время составило {times[-1]:.4f} сек\n")
    f.write(f"4. Производительность составляет от {mflops[0]:.2f} до {mflops[-1]:.2f} MFLOPS\n")
    f.write("5. Падение производительности на больших размерах связано с кэш-промахами\n")

print("\n✅ Таблица сохранена как 'results_table.txt'")
print("\n" + "=" * 60)
print("ВСЕ ОПЕРАЦИИ ЗАВЕРШЕНЫ")
print("=" * 60)

# Обновляем report.md
with open('report.md', 'r', encoding='utf-8') as f:
    report_content = f.read()

# Вставляем таблицу и ссылку на график
with open('report.md', 'w', encoding='utf-8') as f:
    # Находим место для вставки или просто перезаписываем нужную часть
    f.write(report_content)
    f.write("\n\n## Графики\n\n")
    f.write("![График производительности](performance_plot.png)\n\n")
    f.write("## Таблица результатов\n\n")
    f.write("| Размер | Время (сек) | MFLOPS |\n")
    f.write("|--------|-------------|--------|\n")
    for s, t, m in zip(sizes, times, mflops):
        f.write(f"| {s}×{s} | {t:.4f} | {m:.2f} |\n")