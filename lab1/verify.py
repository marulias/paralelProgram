import numpy as np
import sys

def read_matrix(filename):
    try:
        with open(filename, 'r') as f:
            n = int(f.readline())
            M = []
            for _ in range(n):
                row = list(map(float, f.readline().split()))
                M.append(row)
            return np.array(M)
    except Exception as e:
        print(f"Ошибка при чтении {filename}: {e}")
        sys.exit(1)

def main():
    print("АВТОМАТИЧЕСКАЯ ВЕРИФИКАЦИЯ")
    
   
    A = read_matrix("matrix_A.txt")
    B = read_matrix("matrix_B.txt")
    C = read_matrix("result.txt")
    
    print(f"Размер матриц: {A.shape[0]}x{A.shape[1]}")
    
    
    expected = np.dot(A, B)
    
   
    diff = np.abs(C - expected)
    max_diff = np.max(diff)
    mean_diff = np.mean(diff)
    
    print(f"\nМаксимальная разница: {max_diff:.2e}")
    print(f"Средняя разница: {mean_diff:.2e}")
    
    
    if max_diff < 1e-10:
        print("\n ВЕРИФИКАЦИЯ ПРОЙДЕНА")
        print("  Результаты совпадают с эталоном (NumPy)")
        return 0
    else:
        print("\n ошибка: результаты не совпадают")
        print("\nПолученная матрица:")
        print(C)
        print("\nЭталонная матрица:")
        print(expected)
        return 1

if __name__ == "__main__":
    sys.exit(main())