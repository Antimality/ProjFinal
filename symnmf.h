#ifndef SYMNMF_H
#define SYMNMF_H

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

#endif /* SYMNMF_H */