#ifndef SYMNMF_H
#define SYMNMF_H

#define MAX_ITER 300
#define EPS 0.0001
#define BETA 0.5
#define DELTA 0.000000001

#include <stdio.h>

/*
 * ============================================================================
 * Struct definitions
 * ============================================================================
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
 * Command Line Execution Helper Function Prototypes
 * ============================================================================
 */

/**
 * @brief Reads points from a txt file.
 * @param file_name name of the file.
 * @return A 2D matrix of doubles
 **/
double **read_input(char *file_name);

/**
 * @brief Converts a linked list of vectors to a 2D matrix.
 * @param head_vec Pointer to the head of the vector linked list.
 * @param rows Number of rows (vectors) in the list.
 * @return A pointer to the allocated 2D matrix.
 */
double **vec_to_mat(vector *head_vec, int rows);

/**
 * @brief Frees memory allocated for vectors and cords during file read, closes file, and returns NULL.
 * @param head_vec Pointer to the head of the vector linked list.
 * @param head_cord Pointer to the head of the cord linked list.
 * @param fp Pointer to the open file (can be NULL if file read failed).
 * @return Always returns NULL for error handling.
 */
double **free_read(vector *head_vec, cord *head_cord, FILE *fp);

/**
 * @brief Parses a row from a file and builds linked lists of cords and vectors.
 * @param fp Pointer to the open file.
 * @param head_cord Pointer to the head of the cord linked list.
 * @param curr_cord Pointer to the current cord node.
 * @param curr_vec Pointer to the current vector node.
 * @param rows Pointer to the row count variable.
 * @return 0 on success, 1 on allocation error.
 */
int parse_row(FILE *fp, cord **head_cord, cord **curr_cord, vector **curr_vec, int *rows);

/**
 * @brief Executes the specified goal using the provided data points and prints the result.
 * @param goal The goal string ("sym", "ddg", or "norm").
 * @param data_points The input data points matrix.
 */
void run_goal(const char *goal, double **data_points);

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
 * @brief calculates squared Euclidean distance of two vectors.
 * @param vec1 The first vector.
 * @param vec2 The second vector.
 * @param dim The vectors size.
 * @return Euclidean distance.
 */
double euclidean_distance(double *vec1, double *vec2, int dim);

/**
 * @brief calculates squared Frobenius norm of a 2D matrix.
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
 * @param cols1 The first matrix number of columns.
 * @param cols2 The second matrix number of columns.
 * @returns a pointer to the caluclated matrix.
 */
double **matrix_multiply(double **matrix1, double **matrix2, int row1, int cols1, int cols2);

/**
 * @brief Calculates H * H^T (Matrix multiplication)
 * @param matrix The matrix to calculate.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * @returns pointer to a new H*H^T matrix.
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
double **matrix_subtract(double **matrix1, double **matrix2, int rows, int cols);

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
 * @returns a pointer to the matrix
 */
double **matrix_init(int rows, int cols);

/**
 * @brief changes the degree matrix D into D^(-0.5).
 * @param D pointer to matrix D.
 * @param n the size of the matrix.
 */
void inv_root(double **matrix, int n);

/**
 * @brief Frees the memory allocated for a cord structure.
 * @param cor The cord to free.
 */
void free_cords(cord *cor);

/**
 * @brief Frees the memory allocated for a vector structure.
 * @param vec The vec to free.
 */
void free_vectors(vector *vec);

#endif
