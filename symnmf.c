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
    char *goal, *file_name;
    double **data_points;

    /* Check correct number of arguments */
    if (argc != 3)
        handle_error();

    /* Read arguments */
    goal = argv[1];
    file_name = argv[2];

    /* Read file */
    data_points = read_input(file_name);
    if (data_points == NULL)
        handle_error();

    run_goal(goal, data_points);

    return 0;
}

double **read_input(char *file_name) {
    vector *head_vec, *curr_vec;
    cord *head_cord, *curr_cord;
    int rows, err;
    FILE *fp;

    head_vec = (vector *)malloc(sizeof(vector));
    curr_vec = head_vec;
    head_cord = (cord *)malloc(sizeof(cord));
    curr_cord = head_cord;
    rows = 0;
    err = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL || head_cord == NULL || head_vec == NULL)
        return free_read(curr_vec, curr_cord, fp);

    err = parse_row(fp, &head_cord, &curr_cord, &curr_vec, &rows);
    free_cords(head_cord);
    fclose(fp);

    if (err)
        return free_read(head_vec, head_cord, fp);

    return vec_to_mat(head_vec, rows);
}

double **vec_to_mat(vector *head_vec, int rows) {
    vector *curr_vec;
    cord *curr_cord;
    double **matrix;
    int d, i, j;

    curr_vec = head_vec;
    curr_cord = head_vec->cords;

    /* Read dimention */
    d = 0;
    while (curr_cord != NULL) {
        d++;
        curr_cord = curr_cord->next;
    }

    matrix = matrix_init(rows, d);
    if (matrix == NULL) {
        free_vectors(head_vec);
        return NULL;
    }

    /* Copy data to matrix */
    for (i = 0; i < rows; i++) {
        curr_cord = curr_vec->cords;
        for (j = 0; j < d; j++) {
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
    /* Free used memory */
    free_vectors(head_vec);
    free_cords(head_cord);

    /* Close file access */
    if (fp != NULL)
        fclose(fp);

    return NULL;
}

void run_goal(const char *goal, double **data_points) {
    double **sym_matrix, **ddg_matrix, **result_matrix;

    if (strcmp(goal, "sym") == 0) {
        /* Sym */
        result_matrix = calc_sym(data_points, global_n, global_d);

    } else if (strcmp(goal, "ddg") == 0) {
        /* ddg */
        sym_matrix = calc_sym(data_points, global_n, global_d);
        result_matrix = calc_ddg(sym_matrix, global_n);
        free_matrix(sym_matrix, global_n);

    } else if (strcmp(goal, "norm") == 0) {
        /* norm */
        sym_matrix = calc_sym(data_points, global_n, global_d);
        ddg_matrix = calc_ddg(sym_matrix, global_n);
        result_matrix = calc_norm(sym_matrix, ddg_matrix, global_n);
        free_matrix(sym_matrix, global_n);
        free_matrix(ddg_matrix, global_n);

    } else {
        /* Invalid goal */
        handle_error();
    }

    free_matrix(data_points, global_n);
    print_matrix(result_matrix, global_n, global_n);
    free_matrix(result_matrix, global_n);
}

int parse_row(FILE *fp, cord **head_cord, cord **curr_cord, vector **curr_vec, int *rows) {
    double n;
    char c;

    while (fscanf(fp, "%lf%c", &n, &c) == 2) {
        /* Read value */
        (*curr_cord)->value = n;
        if (c == '\n') {
            /* Line end */
            (*rows)++;
            (*curr_vec)->cords = *head_cord;

            /* Allocate next vec */
            (*curr_vec)->next = (vector *)malloc(sizeof(vector));
            *curr_vec = (*curr_vec)->next;
            (*curr_vec)->cords = NULL;
            (*curr_vec)->next = NULL;
            *head_cord = (cord *)malloc(sizeof(cord));
            *curr_cord = *head_cord;
            (*curr_cord)->next = NULL;
            if (*curr_cord == NULL || *curr_vec == NULL)
                return 1;
        } else {
            /* Allocate next cord */
            (*curr_cord)->next = (cord *)malloc(sizeof(cord));
            *curr_cord = (*curr_cord)->next;
            if (*curr_cord == NULL)
                return 1;
            (*curr_cord)->next = NULL;
        }
    }

    return 0;
}

/*
 * ============================================================================
 * Core Algorithm Implementations
 * ============================================================================
 */

double **calc_sym(double **points, int n, int d) {
    double dist, sym, **matrix;
    int i, j;

    matrix = matrix_init(n, n);
    if (matrix == NULL) {
        handle_error();
    }

    for (i = 0; i < n; i++) {
        matrix[i][i] = 0.0;
        for (j = i + 1; j < n; j++) {
            dist = euclidean_distance(points[i], points[j], d);
            sym = exp(-0.5 * dist);
            matrix[i][j] = sym;
            matrix[j][i] = sym;
        }
    }

    return matrix;
}

double **calc_ddg(double **similarity_matrix, int n) {
    double sum, **matrix;
    int i, j;

    matrix = matrix_init(n, n);
    if (matrix == NULL) {
        handle_error();
    }

    for (i = 0; i < n; i++) {
        sum = 0.0;
        for (j = 0; j < n; j++) {
            sum += similarity_matrix[i][j];
        }
        matrix[i][i] = sum;
    }

    return matrix;
}

double **calc_norm(double **similarity_matrix, double **ddg_matrix, int n) {
    double **tmp, **result;

    inv_root(ddg_matrix, n); /* D <- D^-0.5 */

    tmp = matrix_multiply(ddg_matrix, similarity_matrix, n, n, n); /* D^-0.5*A */
    if (tmp == NULL) {
        handle_error();
    }

    result = matrix_multiply(tmp, ddg_matrix, n, n, n); /* D^-0.5*A*D^-0.5 */

    free_matrix(tmp, n);
    if (result == NULL) {
        handle_error();
    }

    return result;
}

double **calc_symnmf(double **W, double **H, int n, int k) {
    double **H_new, **H_prev;
    int i;

    H_new = NULL;
    H_prev = H;
    for (i = 0; i < MAX_ITER; i++) {
        H_new = H_update(W, H_prev, n, n, n, k);
        if (H_new == NULL) {
            if (H_prev != H) {
                free_matrix(H_prev, n);
            }
            return NULL;
        }

        /* Check convergence */
        if (frobenius_norm(H_prev, H_new, n, k) < EPS) {
            break;
        } else {
            free_matrix(H_prev, n);
            H_prev = H_new;
        }
    }

    free_matrix(H_prev, n);
    return H_new;
}

/*
 * ============================================================================
 * Helper Function Implementations
 * ============================================================================
 */

void print_matrix(double **matrix, int rows, int cols) {
    int i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%.4f", matrix[i][j]);
            if (j < cols - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}

void free_matrix(double **matrix, int rows) {
    int i;

    if (matrix != NULL) {
        /* Free matrix rows */
        for (i = 0; i < rows; i++) {
            free(matrix[i]);
        }
        /* Free matrix pointer */
        free(matrix);
    }
}

void handle_error() {
    printf("An Error Has Occurred\n");
    exit(1);
}

double euclidean_distance(double *vec1, double *vec2, int dim) {
    double sum, temp;
    int i;

    sum = 0.0;
    for (i = 0; i < dim; i++) {
        temp = vec1[i] - vec2[i];
        temp *= temp;
        sum += temp;
    }

    return sum;
}

double frobenius_norm(double **matrix1, double **matrix2, int rows, int cols) {
    double sum, diff;
    int i, j;

    sum = 0.0;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            diff = matrix1[i][j] - matrix2[i][j];
            sum += diff * diff;
        }
    }

    return sum;
}

double **matrix_multiply(double **matrix1, double **matrix2, int row1, int cols1, int cols2) {
    double **result;
    int i, j, k;

    result = matrix_init(row1, cols2);
    if (result == NULL) {
        handle_error();
    }

    for (i = 0; i < row1; i++) {
        for (j = 0; j < cols2; j++) {
            for (k = 0; k < cols1; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

double **H_multiply(double **matrix, int rows, int cols) {
    double **HHt, sum;
    int i, j, m;

    HHt = matrix_init(rows, rows);
    if (HHt == NULL) {
        handle_error();
    }

    for (i = 0; i < rows; i++) {
        for (j = i; j < rows; j++) {
            sum = 0.0;
            for (m = 0; m < cols; m++) {
                sum += matrix[i][m] * matrix[j][m];
            }
            HHt[i][j] = sum;
            HHt[j][i] = sum;
        }
    }

    return HHt;
}

double **H_update(double **matrix_W, double **matrix_H, int row_W, int row_H, int cols_W, int cols_H) {
    double **HHt, **WH, **HHtH, **H_new;
    int i, j;

    HHt = H_multiply(matrix_H, row_H, cols_H); /* H * H^T */
    if (HHt == NULL)
        handle_error();

    WH = matrix_multiply(matrix_W, matrix_H, row_W, cols_W, cols_H); /* W * H */
    if (WH == NULL) {
        free_matrix(HHt, row_H);
        handle_error();
    }

    HHtH = matrix_multiply(HHt, matrix_H, row_H, row_H, cols_H); /* H * H^T * H */
    if (HHtH == NULL) {
        free_matrix(HHt, row_H);
        free_matrix(WH, row_W);
        handle_error();
    }

    H_new = matrix_init(row_H, cols_H);
    if (H_new == NULL) {
        free_matrix(HHt, row_H);
        free_matrix(WH, row_W);
        free_matrix(HHtH, row_W);
        handle_error();
    }
    for (i = 0; i < row_H; i++) {
        for (j = 0; j < cols_H; j++) {
            H_new[i][j] = matrix_H[i][j] * (1 - BETA + BETA * (WH[i][j] / (HHtH[i][j] + DELTA)));
        }
    }

    free_matrix(HHt, row_H);
    free_matrix(WH, row_W);
    free_matrix(HHtH, row_W);

    return H_new;
}

double **matrix_init(int rows, int cols) {
    double **matrix;
    int i, r;

    matrix = malloc(rows * sizeof(double *));
    if (!matrix)
        return NULL;

    for (i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(double));
        if (!matrix[i]) {
            /* free previously allocated rows and the array */
            for (r = 0; r < i; r++) {
                free(matrix[r]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

void inv_root(double **matrix, int n) {
    int i;

    for (i = 0; i < n; i++) {
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
