#include <Python.h>

/*
 * ============================================================================
 * Matrix c/py Convertion Functions
 * ============================================================================
 */

/**
 * Converts a Python list of lists to a C double** matrix.
 * @param py_matrix Python object representing a list of lists.
 * @param n Number of rows.
 * @param m Number of columns.
 * @return Pointer to allocated C matrix, or NULL on error.
 */
double **matrix_py_to_c(PyObject *py_matrix, int n, int m);

/**
 * Converts a C double** matrix to a Python list of lists.
 * @param c_matrix Pointer to C matrix.
 * @param n Number of rows.
 * @param m Number of columns.
 * @return Python object representing a list of lists, or NULL on error.
 */
PyObject *matrix_c_to_py(double **c_matrix, int n, int m);

/*
 * ============================================================================
 * Wrapper Function Implementations
 * ============================================================================
 */

/**
 * Internal helper for sym: computes the similarity matrix from Python input.
 * @param points_py Python list of lists representing data points.
 * @param n Number of points.
 * @param d Dimension of each point.
 * @return Pointer to similarity matrix, or NULL on error.
 */
double **_sym_wrapper(PyObject *points_py, int n, int d);

/**
 * Python wrapper for similarity matrix calculation.
 * @param self Unused.
 * @param args Tuple: (points_py, n, d)
 * @return Similarity matrix as Python list of lists.
 */
static PyObject *sym_wrapper(PyObject *self, PyObject *args);

/**
 * Internal helper for ddg: computes the diagonal degree matrix from Python input.
 * @param points_py Python list of lists representing data points.
 * @param n Number of points.
 * @param d Dimension of each point.
 * @return Pointer to diagonal degree matrix, or NULL on error.
 */
double **_dgg_wrapper(PyObject *points_py, int n, int d);

/**
 * Python wrapper for diagonal degree matrix calculation.
 * @param self Unused.
 * @param args Tuple: (points_py, n, d)
 * @return Diagonal degree matrix as Python list of lists.
 */
static PyObject *ddg_wrapper(PyObject *self, PyObject *args);

/**
 * Internal helper for norm: computes the normalized similarity matrix from Python input.
 * @param points_py Python list of lists representing data points.
 * @param n Number of points.
 * @param d Dimension of each point.
 * @return Pointer to normalized similarity matrix, or NULL on error.
 */
double **_norm_wrapper(PyObject *points_py, int n, int d);

/**
 * Python wrapper for normalized similarity matrix calculation.
 * @param self Unused.
 * @param args Tuple: (points_py, n, d)
 * @return Normalized similarity matrix as Python list of lists.
 */
static PyObject *norm_wrapper(PyObject *self, PyObject *args);

/**
 * Python wrapper for symmetric NMF optimization.
 * @param self Unused.
 * @param args Tuple: (W_py, H_init_py, n, k)
 * @return Optimized H matrix as Python list of lists.
 */
static PyObject *symnmf_wrapper(PyObject *self, PyObject *args);