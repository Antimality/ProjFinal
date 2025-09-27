#include "symnmf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int global_n;
int global_d;

/*
 * ============================================================================
 * Main function for command-line execution
 * ============================================================================
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        handle_error();
    }
    char *goal = argv[1];
    char *file_name = argv[2];
    double **data_points = read_input(file_name); // Read from file_name
    if (data_points == NULL) {
        handle_error();
    }
    if (strcmp(goal, "sym") == 0) {
        double **sim_matrix = calc_sym(data_points, global_n, global_d);
        print_matrix(sim_matrix, global_n, global_n);
        free_matrix(sim_matrix, global_n);
    } else if (strcmp(goal, "ddg") == 0) {
        double **sim_matrix = calc_sym(data_points, global_n, global_d);
        double **ddg_matrix = calc_ddg(sim_matrix, global_n);
        print_matrix(ddg_matrix, global_n, global_n);
        free_matrix(sim_matrix, global_n);
        free_matrix(ddg_matrix, global_n);
    } else if (strcmp(goal, "norm") == 0) {
        double **sim_matrix = calc_sym(data_points, global_n, global_d);
        double **ddg_matrix = calc_ddg(sim_matrix, global_n);
        double **norm_matrix = calc_norm(sim_matrix, ddg_matrix, global_n);
        print_matrix(norm_matrix, global_n, global_n);
        free_matrix(sim_matrix, global_n);
        free_matrix(ddg_matrix, global_n);
        free_matrix(norm_matrix, global_n);
    } else {
        handle_error();
    }
    free_matrix(data_points, global_n);
    return 0;
}

/*
 * ============================================================================
 * Core Algorithm Implementations
 * ============================================================================
 */

double **calc_sym(double **points, int n, int d) {
    double **matrix = matrix_init(n, n);
    if (matrix == NULL) {
        handle_error();
    }
    for (int i = 0; i < n; i++) {
        matrix[i][i] = 0.0;
        for (int j = i + 1; j < n; j++) {
            double dist = euclidean_distance(points[i], points[j], d);
            double sim = exp(-0.5 * dist);
            matrix[i][j] = sim;
            matrix[j][i] = sim;
        }
    }
    return matrix;
}

double **calc_ddg(double **similarity_matrix, int n) {
    double **matrix = matrix_init(n, n);
    if (matrix == NULL) {
        handle_error();
    }
    double sum;
    for (int i = 0; i < n; i++) {
        sum = 0.0;
        for (int j = 0; j < n; j++) {
            sum += similarity_matrix[i][j];
        }
        matrix[i][i] = sum;
    }
    return matrix;
}

double **calc_norm(double **similarity_matrix, double **ddg_matrix, int n) {
    inv_root(ddg_matrix, n);
    double **tmp = matrix_multiply(ddg_matrix, similarity_matrix, n, n, n, n);
    if (tmp == NULL) {
        handle_error();
    }
    double **result = matrix_multiply(tmp, ddg_matrix, n, n, n, n);
    free_matrix(tmp, n);
    if (result == NULL) {
        handle_error();
    }
    return result;
}

double **calc_symnmf(double **W, double **H, int n, int k) {
    double **new_H = NULL;
    double **H_copy = H;
    for (int i = 0; i < MAX_ITER; i++) {
        new_H = H_update(W, H_copy, n, n, n, k);
        if (new_H == NULL) {
            if (H_copy != H) {
                free_matrix(H_copy, n);
            }
            return NULL;
        }
        if (frobenius_norm(H_copy, new_H, n, k) < epsilon) {
            free_matrix(new_H, n);
            return H_copy;
        } else {
            free_matrix(H_copy, n);
            H_copy = new_H;
        }
    }
    return H_copy;
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

double euclidean_distance(double *vec1, double *vec2, int dim) {
    double sum = 0.0;
    double temp;
    for (int i = 0; i < dim; i++) {
        temp = vec1[i] - vec2[i];
        temp *= temp;
        sum += temp;
    }

    return sum;
}

double frobenius_norm(double **matrix1, double **matrix2, int rows, int cols) {
    double sum = 0.0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double diff = matrix1[i][j] - matrix2[i][j];
            sum += diff * diff;
        }
    }
    return sum;
}

double **matrix_multiply(double **matrix1, double **matrix2, int row1, int row2, int cols1, int cols2) {
    double **result = matrix_init(row1, cols2);
    if (result == NULL) {
        handle_error();
    }
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < cols2; j++) {
            for (int k = 0; k < cols1; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

double **H_multiply(double **matrix, int rows, int cols) {
    double **HHt = matrix_init(rows, rows);
    if (HHt == NULL) {
        handle_error();
    }
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

double **H_update(double **matrix_W, double **matrix_H, int row_W, int row_H, int cols_W, int cols_H) {
    double **HHt = H_multiply(matrix_H, row_H, cols_H);
    if (HHt == NULL) {
        handle_error();
    }
    double **WH = matrix_multiply(matrix_W, matrix_H, row_W, cols_W, row_H, cols_H);
    if (WH == NULL) {
        free_matrix(HHt, row_H);
        handle_error();
    }
    double **HHtH = matrix_multiply(HHt, matrix_H, row_H, row_H, row_H, cols_H);
    if (HHtH == NULL) {
        free_matrix(HHt, row_H);
        free_matrix(WH, row_W);
        handle_error();
    }
    double **new_H = matrix_init(row_H, cols_H);
    if (new_H == NULL) {
        free_matrix(HHt, row_H);
        free_matrix(WH, row_W);
        free_matrix(HHtH, row_W);
        handle_error();
    }
    for (int i = 0; i < row_H; i++) {
        for (int j = 0; j < cols_H; j++) {
            new_H[i][j] = matrix_H[i][j] * (1 - beta + beta * (WH[i][j] / (HHtH[i][j] + delta)));
        }
    }
    free_matrix(HHt, row_H);
    free_matrix(WH, row_W);
    free_matrix(HHtH, row_W);
    return new_H;
}

double **matrix_init(int rows, int cols) {
    double **matrix = malloc(rows * sizeof(double *));
    if (!matrix)
        return NULL;
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(double));
        if (!matrix[i]) {
            /* free previously allocated rows and the array */
            for (int r = 0; r < i; r++) {
                free(matrix[r]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

void inv_root(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        if (matrix[i][i] != 0) {
            matrix[i][i] = 1 / sqrt(matrix[i][i]);
        }
    }
}

void free_cords(cord *head) {
    cord *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void free_vectors(vector *vec) {
    vector *temp;
    while (vec != NULL) {
        temp = vec;
        free_cords(vec->cords);
        vec = vec->next;
        free(temp);
    }
}

double **read_input(char *file_name) {
    vector *head_vec, *curr_vec;
    cord *head_cord, *curr_cord;
    int rows;
    double n;
    char c;
    FILE *fp;
    if ((fp = fopen(file_name, "r")) == NULL)
        return NULL;
    rows = 0;
    head_cord = (cord *)malloc(sizeof(cord));
    curr_cord = head_cord;
    head_vec = (vector *)malloc(sizeof(vector));
    curr_vec = head_vec;
    if (head_cord == NULL || head_vec == NULL)
        return free_read(curr_vec, curr_cord, fp);
    while (fscanf(fp, "%lf%c", &n, &c) == 2) {
        if (c == '\n') {
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = (vector *)malloc(sizeof(vector));
            curr_vec = curr_vec->next;
            head_cord = (cord *)malloc(sizeof(cord));
            curr_cord = head_cord;
            rows++;
            if (curr_cord == NULL || curr_vec == NULL)
                return free_read(head_vec, head_cord, fp);
            continue;
        }
        curr_cord->value = n;
        curr_cord->next = (cord *)malloc(sizeof(cord));
        curr_cord = curr_cord->next;
        if (curr_cord == NULL)
            return free_read(head_vec, curr_cord, fp);
        curr_cord->next = NULL;
    }
    free_cords(head_cord);
    fclose(fp);
    return vec_to_mat(head_vec, rows);
}

double **vec_to_mat(vector *head_vec, int rows) {
    vector *curr_vec = head_vec;
    cord *curr_cord = head_vec->cords;
    int d = 0;
    while (curr_cord != NULL) {
        d++;
        curr_cord = curr_cord->next;
    }
    double **matrix = matrix_init(rows, d);
    if (matrix == NULL) {
        free_vectors(head_vec);
        return NULL;
    }
    for (int i = 0; i < rows; i++) {
        curr_cord = curr_vec->cords;
        for (int j = 0; j < d; j++) {
            matrix[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
        curr_vec = curr_vec->next;
    }
    free_vectors(head_vec);
    global_n = rows;
    global_d = d;
    return matrix;
}

double **free_read(vector *head_vec, cord *head_cord, FILE *fp) {
    free_vectors(head_vec);
    free_cords(head_cord);
    if (fp != NULL)
        fclose(fp);
    return NULL;
}
