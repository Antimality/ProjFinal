import sys
import numpy as np
import mysymnmf as symnmf


def print_matrix(matrix):
    """Prints a matrix with 4 decimal places, comma-separated."""
    for row in matrix:
        print(",".join(["%.4f" % val for val in row]))


def init_H(w_matrix, k):
    np.random.seed(1234)
    m = np.mean(w_matrix, dtype=np.float64)
    return np.random.uniform(0.0, 2 * np.sqrt(m / float(k)), size=(len(w_matrix), k)).astype(np.float64, copy=False)


def run_symnmf(k, goal, file_name):
    """Main execution function."""
    try:
        # Read data from file
        data_points = np.loadtxt(file_name, delimiter=",")
        data_points_list = data_points.tolist()
        n, d = data_points.shape

        result_matrix = None

        # Execute the requested goal
        match goal:
            case "sym":
                result_matrix = symnmf.sym(data_points_list, n, d)
            case "ddg":
                result_matrix = symnmf.ddg(data_points_list, n, d)
            case "norm":
                result_matrix = symnmf.norm(data_points_list, n, d)
            case "symnmf":
                # Calculate W (normalized similarity matrix)
                w_matrix = symnmf.norm(data_points_list, n, d)

                # Initialize H in Python as required
                h_init = init_H(w_matrix, k)

                # Call the C extension with initial H and W
                result_matrix = symnmf.symnmf(w_matrix, h_init.tolist(), n, k)

        if result_matrix is None:
            raise RuntimeError

        return result_matrix

    except Exception as _:
        print("An Error Has Occurred")
        sys.exit(1)


if __name__ == "__main__":
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]
    print_matrix(run_symnmf(k, goal, file_name))
