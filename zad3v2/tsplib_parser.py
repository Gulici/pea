import tsplib95
import json
import argparse


def load_tsplib_matrix(instance):
    n = instance.dimension
    matrix = [[0] * n for _ in range(n)]
    for i in range(1, n+1):
        for j in range(1, n+1):
            matrix[i-1][j-1] = instance.get_weight(i, j)
    return matrix


def load_optimal_tour(tour_file):
    with open(tour_file) as f:
        tour = []
        active = False
        for line in f:
            line = line.strip()
            if line == "TOUR_SECTION":
                active = True
                continue
            if not active:
                continue
            if line == "-1" or line == "EOF":
                break
            tour.append(int(line) - 1)
    return tour


# def compute_tour_cost(matrix, tour):
#     cost = 0
#     for i in range(len(tour) - 1):
#         cost += matrix[tour[i]-1][tour[i+1]-1]
#     cost += matrix[tour[-1]-1][tour[0]-1]
#     return cost

# def compute_tour_cost(matrix, tour):
#     """
#     Oblicza koszt cyklu 'tour' na macierzy 'matrix'.
#     - automatycznie wykrywa czy 'tour' jest 1-based i normalizuje na 0-based,
#     - waliduje długość i zakresy indeksów,
#     - sprawdza czy tour wygląda jak permutacja (opcjonalnie),
#     - jeśli napotka brakującą/nielegalną krawędź (np. None) -> rzuca ValueError.
#     """
#     n = len(matrix)
#     if n == 0:
#         return 0

#     # skopiuj, żeby nie modyfikować oryginału
#     t = list(tour)

#     # wykrycie 1-based: jeżeli max >= n, a max == n (lub <= n) -> załóż 1-based
#     m = max(t)
#     if m >= n:
#         # jeśli np. max == n -> najpewniej 1-based (wartość n -> index n-1)
#         # spróbuj zrzucić o 1 i walidować zakresy
#         t = [x - 1 for x in t]
#         if not all(0 <= x < n for x in t):
#             raise ValueError("Tour indices out of bounds even after converting 1->0 based")

#     # opcjonalna walidacja permutacji (możesz zakomentować jeśli nie chcesz)
#     if len(t) != n or sorted(t) != list(range(n)):
#         # może tour nie zawiera powrotu do startu (np. jest powtórzony pierwszy element) —
#         # dopuszczamy listę długości n bez powtórzenia; sprawdź dokładnie jeśli trzeba
#         # tutaj rzucimy błąd, żeby nie liczyć na niepełnej trasie
#         raise ValueError("Tour does not look like a permutation of 0..n-1")

#     # policz koszt
#     cost = 0
#     for i in range(n - 1):
#         w = matrix[t[i]][t[i+1]]
#         if w is None:
#             raise ValueError(f"No edge weight for {t[i]}->{t[i+1]}")
#         cost += w
#     w = matrix[t[-1]][t[0]]
#     if w is None:
#         raise ValueError(f"No edge weight for {t[-1]}->{t[0]}")
#     cost += w
#     return cost



def tsplib_to_json(tsp_file, opt, out_file):
    instance = tsplib95.load(tsp_file)
    matrix = load_tsplib_matrix(instance)
    optimum = opt

    data = {
        "size": instance.dimension,
        "optimum": optimum,
        "matrix": matrix
    }

    with open(out_file, "w") as f:
        json.dump(data, f, indent=2)


def main():
    parser = argparse.ArgumentParser(
        description="Convert TSPLIB TSP/ATSP instance + OPT tour to JSON containing size, optimum, matrix."
    )

    parser.add_argument("--tsp", required=True, help="Plik .tsp (instancja TSPLIB)")
    parser.add_argument("--opt", required=True, help="Wartość optymalnego rozwiązania")
    parser.add_argument("--out", required=True, help="Plik wynikowy .json")

    args = parser.parse_args()
    tsplib_to_json(args.tsp, args.opt, args.out)


if __name__ == "__main__":
    main()