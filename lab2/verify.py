import numpy as np
import sys
import os

def read_matrix_from_file(filename):
    try:
        with open(filename, 'r') as f:
            n = int(f.readline().strip())
            M = []
            for _ in range(n):
                row = list(map(float, f.readline().split()))
                M.append(row)
            return np.array(M)
    except Exception as e:
        print(f"Ошибка при чтении {filename}: {e}")
        sys.exit(1)

def read_matrix_from_result(filename):
    try:
        with open(filename, 'r') as f:
            lines = f.readlines()
            for i, line in enumerate(lines):
                if "Результат C:" in line:
                    for j, l in enumerate(lines):
                        if "Размер матрицы:" in l:
                            size = int(l.split()[2].split('x')[0])
                            break
                    M = []
                    for j in range(i+1, i+1+size):
                        if j < len(lines):
                            row = list(map(float, lines[j].split()))
                            M.append(row)
                    return np.array(M)
    except Exception as e:
        print(f"Ошибка при чтении: {e}")
        sys.exit(1)

def main():
    print("АВТОМАТИЧЕСКАЯ ВЕРИФИКАЦИЯ (Параллельная версия)")
    print("=" * 50)
    
    if os.path.exists("matrix_A.txt") and os.path.exists("matrix_B.txt"):
        A = read_matrix_from_file("matrix_A.txt")
        B = read_matrix_from_file("matrix_B.txt")
        print("Чтение из файлов matrix_A.txt и matrix_B.txt")
    else:
        A = read_matrix_from_result("result_parallel_200.txt")
        B = read_matrix_from_result("result_parallel_200.txt")
        print("Чтение из файла result_parallel_200.txt")
    
    C = read_matrix_from_result("result_parallel_200.txt")
    
    print(f"Размер матриц: {A.shape[0]}x{A.shape[1]}")
    
    expected = np.dot(A, B)
    
    if C.shape != expected.shape:
        print(f"\nОШИБКА: Размеры не совпадают!")
        print(f"Получено: {C.shape}")
        print(f"Ожидалось: {expected.shape}")
        return 1
    
    diff = np.abs(C - expected)
    max_diff = np.max(diff)
    mean_diff = np.mean(diff)
    
    print(f"\nМаксимальная разница: {max_diff:.2e}")
    print(f"Средняя разница: {mean_diff:.2e}")
    
    if max_diff < 1e-10:
        print("\nВЕРИФИКАЦИЯ ПРОЙДЕНА УСПЕШНО")
        print("Результаты совпадают с эталоном (NumPy)")
        return 0
    else:
        print("\nОШИБКА: результаты не совпадают")
        return 1

if __name__ == "__main__":
    sys.exit(main())