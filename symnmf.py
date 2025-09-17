import sys
import numpy as np
import mysymnmf as symnmf


def print_matrix(matrix):
    """Prints a matrix with 4 decimal places, comma-separated."""
    for row in matrix:
        print(",".join(["%.4f" % val for val in row]))


def _symnmf(data_points_list, n, k):
    # 1. Calculate W (normalized similarity matrix)
    w_matrix = symnmf.norm(data_points_list)

    # 2. Initialize H in Python as required [cite: 68-70]
    np.random.seed(1234)
    m = np.mean(w_matrix)
    h_init = np.random.uniform(0, 2 * np.sqrt(m / k), size=(n, k))

    # 3. Call the C extension with initial H and W
    return symnmf.symnmf(w_matrix, h_init.tolist())


def main():
    """Main execution function."""
    try:
        # Validate and parse command-line arguments [cite: 60-67]
        if len(sys.argv) != 4:
            raise ValueError("Invalid number of arguments")

        k = int(sys.argv[1])
        goal = sys.argv[2]
        file_name = sys.argv[3]

        # Read data from file
        data_points = np.loadtxt(file_name, delimiter=",")
        data_points_list = data_points.tolist()
        n, d = data_points.shape

        result_matrix = None

        # Execute the requested goal
        match goal:
            case "sym":
                result_matrix = symnmf.sym(data_points_list)
            case "ddg":
                result_matrix = symnmf.ddg(data_points_list)
            case "norm":
                result_matrix = symnmf.norm(data_points_list)
            case "symnmf":
                result_matrix = _symnmf(data_points_list, n, k)

        # Print the final matrix
        if result_matrix:
            print_matrix(result_matrix)

    except Exception as _:
        print("An Error Has Occurred")
        sys.exit(1)


if __name__ == "__main__":
    main()
