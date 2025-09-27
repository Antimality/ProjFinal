#ifndef SYMNMF_H
#define SYMNMF_H
#define MAX_ITER 300
#define epsilon 0.0001
#define beta 0.5
#define delta 0.000001

/*
struct definitions
*/

struct cord {
    double value;
    struct cord *next;
};

struct vector {
    struct cord *cords;
    struct vector *next;
};

typedef struct cord cord;
typedef struct vector vector;
/*
 * ============================================================================
 * Function Prototypes
 * ============================================================================
 */

/**
 * @brief Calculates the similarity matrix A from a set of data points X.
 * @param points An array of data points.
 * @param n Number of data points.
 * @param d Dimension of each data point.
 * @return A pointer to the allocated similarity matrix.
 */
double **calc_sym(double **points, int n, int d);

/**
 * @brief Calculates the Diagonal Degree Matrix D from the similarity matrix A.
 * @param similarity_matrix The similarity matrix A.
 * @param n The number of data points.
 * @return A pointer to the allocated diagonal degree matrix.
 */
double **calc_ddg(double **similarity_matrix, int n);

/**
 * @brief Calculates the normalized similarity matrix W.
 * @param similarity_matrix The similarity matrix A.
 * @param ddg_matrix The diagonal degree matrix D.
 * @param n The number of data points.
 * @return A pointer to the allocated normalized similarity matrix.
 */
double **calc_norm(double **similarity_matrix, double **ddg_matrix, int n);

/**
 * @brief Performs the symmetric Non-negative Matrix Factorization optimization.
 * @param W The normalized similarity matrix.
 * @param H The initial H matrix.
 * @param n The number of data points.
 * @param k The number of clusters.
 * @return A pointer to the final optimized H matrix.
 */
double **calc_symnmf(double **W, double **H, int n, int k);

/*
 * ============================================================================
 * Helper Function Prototypes
 * ============================================================================
 */

/**
 * @brief Prints a matrix to standard output in the required format.
 * @param matrix The matrix to print.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 */
void print_matrix(double **matrix, int rows, int cols);

/**
 * @brief Frees the memory allocated for a 2D matrix.
 * @param matrix The matrix to free.
 * @param rows The number of rows in the matrix.
 */
void free_matrix(double **matrix, int rows);

/**
 * @brief Handles errors by printing a standard message and exiting.
 */
void handle_error();

/**
 * @brief calculates Euclidean distance of two vectors.
 * @param vec1 The first vector.
 * @param vec2 The second vector.
 * @param dim The vectors size.
 * @return Euclidean distance.
 */
double euclidean_distance(double *vec1, double *vec2, int dim);

/**
 * @brief calculates Frobenius norm of a 2D matrix.
 * @param matrix1 The matrix to calculate.
 * @param matrix2 The matrix to calculate.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * @param dim the vector dimansion.
 * @return Frobenius norm.
 */
double frobenius_norm(double **matrix1, double **matrix2, int rows, int cols);

/**
 * @brief Returns the multiplication of two matrixes.
 * @param matrix1 The  first matrix.
 * @param matrix2 The  second matrix.
 * @param row1 The first matrix number of rows.
 * @param row2 The second matrix number of columns.
 * @param cols1 The first matrix number of columns.
 * @param cols2 The second matrix number of columns.
 * @returns a pointer to the caluclated matrix.
 */
double **matrix_multiply(double **matrix1, double **matrix2, int row1, int row2, int cols1, int cols2 );

/**
 * @brief Returns the special multiplication of the H matrix.
 * @param matrix The matrix to calculate.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * @returns pointer to a new H(H^T) matrix.
 */
double **H_multiply(double **matrix, int rows, int cols);

/**
 * @brief Returns the subtraction of two matrixes.
 * @param matrix1 The first matrix.
 * @param matrix2 The second matrix.
 * @param rows The matrix number of rows.
 * @param cols The first  number of columns.
 * @returns a pointer to the caluclated matrix.
 */
double **matrix_subtract(double **matrix1, double **matrix2, int rows, int cols );

/**
 * @brief Returns the next iteration of H
 * @param matrix_W The W matrix.
 * @param matrix_H The H matrix.
 * @param row_W The W matrix number of rows.
 * @param row_H The H matrix number of columns.
 * @param cols_W The W matrix number of columns.
 * @param cols_H The H matrix number of columns.
 * @returns a pointer to the caluclated matrix.
 */
double **H_update(double **matrix_W, double **matrix_H, int row_W, int row_H, int cols_W, int cols_H);

/**
 * @brief initilize a matrix full of 0.0
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * @returns a pointer to the matrix/
 */
double **matrix_init(int rows, int cols);

/**
 * @brief changes the degree matrix D into D^(-0.5).
 * @param D pointer to matrix D.
 * @param n the size of the matrix.
 */
void inv_root(double **matrix, int n);

void free_cords(cord *head);

void free_vectors(vector *vec);

double **read_input(char *file_name);

double **vec_to_mat(vector *head_vec,int rows);

double **free_read(vector *head_vec, cord *head_cord, FILE *fp)
#endif
