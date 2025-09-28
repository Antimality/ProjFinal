import sys
import numpy as np
import mysymnmf as symnmf


def print_matrix(matrix: list[list[float]]):
    """
    Prints a matrix with 4 decimal places, comma-separated.
    Each row is printed on a new line.
    """
    for row in matrix:
        print(",".join(["%.4f" % val for val in row]))


def init_H(w_matrix: np.ndarray, k: int) -> np.ndarray:
    """
    Initializes the H matrix for SymNMF.
    Uses a fixed random seed for reproducibility.
    Each entry is drawn uniformly from [0, 2*sqrt(mean(W)/k)].
    
    Args:
        w_matrix (Numpy 2D array): The normalized similarity matrix W.
        k (int): Number of clusters/components.
    Returns:
        np.ndarray: Initialized H matrix of shape (n, k).
    """
    np.random.seed(1234)
    m = np.mean(w_matrix, dtype=np.float64)
    return np.random.uniform(0.0, 2.0 * np.sqrt(m / float(k)), size=(len(w_matrix), k)).astype(np.float64, copy=False)


def run_symnmf(k: int, goal: str, file_name: str) -> list[list[float]]:
    """
    Main execution function for SymNMF goals. Reads input data and executes the requested goal.
    
    Args:
        k (int): Number of clusters/components.
        goal (str): The goal to execute ("sym", "ddg", "norm", "symnmf").
        file_name (str): Path to the input data file.
    Returns:
        list: Resulting matrix as a list of lists.
    """
    try:
        data_points = np.loadtxt(file_name, delimiter=",")
        data_points_list = data_points.tolist()
        n, d = data_points.shape
        result_matrix = None

        if (k >= n): 
            handle_error()

        match goal:
            case "sym":
                result_matrix = symnmf.sym(data_points_list, n, d)
            case "ddg":
                result_matrix = symnmf.ddg(data_points_list, n, d)
            case "norm":
                result_matrix = symnmf.norm(data_points_list, n, d)
            case "symnmf":
                w_matrix = symnmf.norm(data_points_list, n, d)
                h_init = init_H(w_matrix, k)
                result_matrix = symnmf.symnmf(w_matrix, h_init.tolist(), n, k)

        if result_matrix is None:
            raise RuntimeError

        return result_matrix

    except Exception as _:
        handle_error()
        
def handle_error():
    """
    Prints a generic error message and exits the program.
    """
    print("An Error Has Occurred")
    sys.exit(1)

def parse_args(args: list[str]) -> tuple[int, str, str]:
    """
    Parses and validates command-line arguments.
    
    Args:
        args (list): List of command-line arguments.
    Returns:
        tuple: (k, goal, file_name)
    """
    # Check correct number of arguments
    if len(args) != 4:
        handle_error()
        
    # Check goal argument
    goal = args[2]
    if goal not in {"sym", "ddg", "norm", "symnmf"}:
        handle_error()
    
    # Check that k is a positive int
    try:
        k = int(args[1])
        if k <= 0 and goal == "symnmf":
            raise ValueError
    except Exception:
        handle_error()
    
    # Check that file exists
    file_name = args[3]
    try:
        with open(file_name, "r"):
            pass
    except Exception:
        handle_error()
        
    return k, goal, file_name

if __name__ == "__main__":
    k, goal, file_name = parse_args(sys.argv)
    print_matrix(run_symnmf(k, goal, file_name))
