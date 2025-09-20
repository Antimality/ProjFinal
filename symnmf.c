#include "symnmf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Make sure all functions are less than 40 lines

/*
 * ============================================================================
 * Main function for command-line execution
 * ============================================================================
 */
int main(int argc, char *argv[]) {
    // TODO: Implement main function logic as per section 2.2 [cite: 86]
    // 1. Validate argc is 3. If not, handle_error().
    // 2. Read 'goal' and 'file_name' from argv.
    // 3. Read data points from 'file_name'.
    // 4. Based on 'goal', call the appropriate function (calc_sym, calc_ddg, calc_norm).
    // 5. Print the resulting matrix using print_matrix().
    // 6. Free all allocated memory.

    // Example structure:
    if (argc != 3) {
        handle_error();
    }

    char *goal = argv[1];
    char *file_name = argv[2];

    // Placeholder for data reading
    int n = 0;                   // Number of data points
    int d = 0;                   // Dimension of data points
    double **data_points = NULL; // Read from file_name

    if (strcmp(goal, "sym") == 0) {
        // TODO: Call calc_sym and print result
    } else if (strcmp(goal, "ddg") == 0) {
        // TODO: Call calc_sym, then calc_ddg, and print result
    } else if (strcmp(goal, "norm") == 0) {
        // TODO: Call calc_sym, calc_ddg, calc_norm, and print result
    } else {
        handle_error();
    }

    // TODO: Free memory
    return 0;
}

/*
 * ============================================================================
 * Core Algorithm Implementations
 * ============================================================================
 */

double **calc_sym(double **points, int n, int d) {
    // TODO: Implement similarity matrix calculation as per section 1.1 [cite: 11-15]
    double **matrix = matrix_init(n,n);
    for (int i = 0; i < n; i++) {
        matrix[i][i] = 0.0; 
        for (int j = i + 1; j < n; j++) {
            double dist = euclidean_distance(points[i], points[j], d);
            double sim  = exp(-0.5 * dist);
            matrix[i][j] = sim;
            matrix[j][i] = sim;
        }
    }
    return NULL; // Placeholder
}

double **calc_ddg(double **similarity_matrix, int n) {
    // TODO: Implement diagonal degree matrix calculation as per section 1.2 [cite: 16-19]
    double **matrix = matrix_init(n,n);
    double sum;
    for (int i = 0; i < n; i++){
        sum = 0.0;
        for (int j = 0; j < n; j++){
            sum += similarity_matrix[i][j];
        }
        matrix[i][i] = sum;
    }
    return NULL; // Placeholder
}

double **calc_norm(double **similarity_matrix, double **ddg_matrix, int n) {
    // TODO: Implement normalized similarity matrix calculation as per section 1.3 [cite: 22-25]
    inv_root(ddg_matrix, n);
    similarity_matrix = matrix_multiply (similarity_matrix, ddg_matrix, n, n, n, n);
    similarity_matrix = matrix_multiply (similarity_matrix, ddg_matrix, n, n, n, n);
    return NULL; // Placeholder
}

double **calc_symnmf(double **W, double **H, int n, int k) {
    // TODO: Implement the H optimization algorithm as per section 1.4 [cite: 26-36]
    // Use max_iter=300 and epsilon=1e-4 [cite: 154]
    double **new_H = matrix_init(n,k);
    for (int i = 0; i < MAX_ITER; i++){
        new_H = H_update(W, H, n, n, n, k);
        if(frobenius_norm(matrix_subtract(H, new_H, n, k), n, k) < epsilon){
            //return new_H and free what should be freed
        }
        else{
            H = new_H;
        }
    }
    return NULL; // Placeholder
}

/*
 * ============================================================================
 * Helper Function Implementations
 * ============================================================================
 */

void print_matrix(double **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.4f", matrix[i][j]);
            if (j < cols - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}

void free_matrix(double **matrix, int rows) {
    if (matrix != NULL) {
        for (int i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
}

void handle_error() {
    printf("An Error Has Occurred\n");
    exit(1);
}

double euclidean_distance(double *vec1, double *vec2, int dim){
    double sum = 0.0;
    double temp;
    for (int i = 0; i < dim; i++)
    {
        temp = vec1[i] - vec2[i];
        temp *= temp;
        sum += temp;
    }
    
    return sum;
}

double frobenius_norm(double **matrix, int rows, int cols){
        double sum = 0.0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sum += matrix[i][j] * matrix[i][j];
        }
    }
    return sum;
}

double **matrix_multiply(double **matrix1, double **matrix2, int row1, int row2, int cols1, int cols2 ){
    double **result = matrix_init(row1, cols2);

    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < cols2; j++) {
            for (int k = 0; k < cols1; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

double **H_multiply(double **matrix, int rows, int cols){
    double **HHt = matrix_init(rows, rows);

    for (int i = 0; i < rows; i++) {
        for (int j = i; j < rows; j++) {  
            double sum = 0.0;
            for (int m = 0; m < cols; m++) {
                sum += matrix[i][m] * matrix[j][m];
            }
            HHt[i][j] = sum;
            HHt[j][i] = sum; 
        }
    }
    return HHt;
}

double **matrix_subtract(double **matrix1, double **matrix2, int rows ,int cols ){
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            matrix2[i][j] = matrix2[i][j] - matrix1[i][j];
        }
    }
    return matrix2;
}

double **H_update(double **matrix_W, double **matrix_H, int row_W, int row_H, int cols_W, int cols_H){
    double **HHt = H_multiply(matrix_H, row_H, cols_H);
    double **WH = matrix_multiply (matrix_W, matrix_H, row_W, row_H, cols_W, cols_H);
    double **HHtH = matrix_multiply (HHt, matrix_H, row_W, row_H, cols_W, cols_H);
    for (int i = 0; i < row_H; i++){
        for (int j = 0; j < cols_H; j++){
            matrix_H[i][j] *=(1 - beta + beta*(WH[i][j]/(HHtH[i][j] + delta)));
        }
    }
    free_matrix(HHt, row_H);
    free_matrix(WH, row_W);
    free_matrix(HHtH, row_W);
    return matrix_H;
}

double **matrix_init(int rows, int cols){
    double **matrix = malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(double));
    }
    return matrix;
}

void inv_root(double **matrix, int n){
    for (int i = 0; i < n; i++){
        matrix[i][i] = 1/sqrt(matrix[i][i]);
    }
}
