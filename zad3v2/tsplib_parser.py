import tsplib95
import json
import argparse


# def load_tsplib_matrix(instance):
#     n = instance.dimension
#     matrix = [[0] * n for _ in range(n)]
#     for i in range(1, n+1):
#         for j in range(1, n+1):
#             matrix[i-1][j-1] = instance.get_weight(i, j)
#     return matrix

# def load_tsplib_matrix(instance):
#     n = instance.dimension
#     matrix = [[0] * n for _ in range(n)]

#     INF_THRESHOLD = 10**7  # typowe dla TSPLIB

#     for i in range(1, n+1):
#         for j in range(1, n+1):
#             w = instance.get_weight(i, j)
#             if w is None:
#                 matrix[i-1][j-1] = None
#             else:
#                 matrix[i-1][j-1] = None if w >= INF_THRESHOLD else w
#     return matrix

def load_tsplib_matrix(instance):
    n = instance.dimension

    # wynik: macierz 0-based, w której 0 oznacza brak krawędzi
    matrix = [[0] * n for _ in range(n)]

    # TSPLIB używa absurdalnie dużych wag jako "zakaz przejścia"
    INF_THRESHOLD = 10**7

    for i in range(1, n + 1):
        for j in range(1, n + 1):

            w = instance.get_weight(i, j)

            # ---- Walidacja wag ----
            if w is None:
                # brak danych → traktuj jak brak krawędzi
                matrix[i - 1][j - 1] = 0
                continue

            if w < 0:
                raise ValueError(f"Ujemna waga w TSPLIB: edge {i}->{j} = {w}")

            if w == 0:
                # 0 w TSPLIB jako legalna waga NIE powinno wystąpić
                # (niektóre przykłady mają tylko 0 na diagonalach)
                if i != j:
                    raise ValueError(
                        f"TSPLIB zawiera 0 jako wagę nie-diagonalną: edge {i}->{j}. "
                        "W Twoim solverze 0 = brak krawędzi, więc to niedozwolone."
                    )
                # jeśli diagonalne 0 (legalne) → i tak daj 0 jako brak krawędzi
                matrix[i - 1][j - 1] = 0
                continue

            if w >= INF_THRESHOLD:
                # duża liczba = "brak krawędzi"
                matrix[i - 1][j - 1] = 0
                continue

            # ścieżka normalna
            matrix[i - 1][j - 1] = w

    return matrix

# def load_optimal_tour(tour_file):
#     with open(tour_file) as f:
#         tour = []
#         active = False
#         for line in f:
#             line = line.strip()
#             if line == "TOUR_SECTION":
#                 active = True
#                 continue
#             if not active:
#                 continue
#             if line == "-1" or line == "EOF":
#                 break
#             tour.append(int(line) - 1)
#     return tour


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