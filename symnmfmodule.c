#define PY_SSIZE_T_CLEAN
#include "symnmf.h"
#include <Python.h>

// Forward declarations for the wrapper functions
static PyObject *sym_wrapper(PyObject *self, PyObject *args);
static PyObject *ddg_wrapper(PyObject *self, PyObject *args);
static PyObject *norm_wrapper(PyObject *self, PyObject *args);
static PyObject *symnmf_wrapper(PyObject *self, PyObject *args);

/*
 * ============================================================================
 * Python C API Method Definitions
 * ============================================================================
 */

// Method definition object for this extension, mapping python method names to C functions
static PyMethodDef symnmf_methods[] = {
    {
        "sym",
        (PyCFunction)sym_wrapper,
        METH_VARARGS,
        PyDoc_STR("Calculate the similarity matrix."),
    },
    {
        "ddg",
        (PyCFunction)ddg_wrapper,
        METH_VARARGS,
        PyDoc_STR("Calculate the diagonal degree matrix."),
    },
    {
        "norm",
        (PyCFunction)norm_wrapper,
        METH_VARARGS,
        PyDoc_STR("Calculate the normalized similarity matrix."),
    },
    {
        "symnmf",
        (PyCFunction)symnmf_wrapper,
        METH_VARARGS,
        PyDoc_STR("Perform the symNMF optimization."),
    },
    {NULL, NULL, 0, NULL},
};

/*
 * ============================================================================
 * Module Definition
 * ============================================================================
 */
static struct PyModuleDef symnmf_module = {
    PyModuleDef_HEAD_INIT,
    "mysymnmf",                                  // Module name
    "Python C API for symmetric NMF algorithm.", // Module documentation
    -1,
    symnmf_methods};

/*
 * ============================================================================
 * Module Initialization
 * ============================================================================
 */
PyMODINIT_FUNC PyInit_mysymnmf(void) {
    return PyModule_Create(&symnmf_module);
}

/*
 * ============================================================================
 * Wrapper Function Implementations
 * ============================================================================
 */

// Helper function to convert a Python list of lists to a C double**
double **matrix_py_to_c(PyObject *py_matrix, int n, int d) {
    // 1. Get dimensions n (rows) and d (cols).
    // 2. Allocate memory for a C double** matrix.
    // 3. Iterate through the Python list, converting each element to double and populating the C matrix.
    // 4. Return the C matrix. Return NULL on failure.
    Py_ssize_t i, j;
    double **c_matrix = (double **)malloc((n) * sizeof(double *));
    if (!c_matrix)
        return NULL;
    for (i = 0; i < n; i++) {
        PyObject *row = PyList_GetItem(py_matrix, i);
        if (!PyList_Check(row) || PyList_Size(row) != d) {
            free_matrix(c_matrix, i);
            return NULL;
        }
        c_matrix[i] = (double *)malloc((d) * sizeof(double));
        if (!c_matrix[i]) {
            free_matrix(c_matrix, i);
            return NULL;
        }
        for (j = 0; j < d; j++) {
            PyObject *item = PyList_GetItem(row, j);
            c_matrix[i][j] = PyFloat_AsDouble(item);
            if (PyErr_Occurred()) {
                free_matrix(c_matrix, i);
                return NULL;
            }
        }
    }
    return c_matrix;
}

// Helper function to convert a C double** to a Python list of lists
PyObject *matrix_c_to_py(double **c_matrix, int n, int d) {
    // 1. Create a new Python list (PyList_New).
    // 2. Iterate through the C matrix. For each row, create a new Python list.
    // 3. For each element in the row, create a Python float (PyFloat_FromDouble) and add it to the row list.
    // 4. Add the row list to the main list.
    // 5. Return the main Python list.
    if (!c_matrix)
        return NULL;

    PyObject *py_matrix = PyList_New(n);
    if (!py_matrix)
        return NULL;
    for (int i = 0; i < n; i++) {
        PyObject *row_list = PyList_New(d);
        if (!row_list) {
            Py_DECREF(py_matrix);
            free_matrix(c_matrix, n);
            return NULL;
        }
        for (int j = 0; j < d; j++) {
            PyObject *num = PyFloat_FromDouble(c_matrix[i][j]);
            if (!num) {
                Py_DECREF(row_list);
                Py_DECREF(py_matrix);
                free_matrix(c_matrix, n);
                return NULL;
            }
            PyList_SET_ITEM(row_list, j, num);
        }
        PyList_SET_ITEM(py_matrix, i, row_list);
    }
    return py_matrix;
}

double **_sym_wrapper(PyObject *points_py, int n, int d) {
    double **points_c, **sym_c;
    points_c = matrix_py_to_c(points_py, n, d);
    Py_DECREF(points_py);
    if (!points_c)
        return NULL;
    sym_c = calc_sym(points_c, n, d);
    free_matrix(points_c, n);
    return sym_c;
}

static PyObject *sym_wrapper(PyObject *self, PyObject *args) {
    PyObject *points_py, *sym_py;
    int n, d;
    double **sym_c;
    if (!PyArg_ParseTuple(args, "Oii", &points_py, &n, &d))
        return NULL;
    sym_c = _sym_wrapper(points_py, n, d);
    if (!sym_c)
        return NULL;
    sym_py = matrix_c_to_py(sym_c, n, d);
    free_matrix(sym_c, n);
    return sym_py;
}

double **_dgg_wrapper(PyObject *points_py, int n, int d) {
    double **sym_c, **dgg_c;
    sym_c = _sym_wrapper(points_py, n, d);
    if (!sym_c)
        return NULL;
    dgg_c = calc_ddg(sym_c, n);
    free_matrix(sym_c, n);
    return dgg_c;
}

static PyObject *ddg_wrapper(PyObject *self, PyObject *args) {
    PyObject *points_py, *dgg_py;
    int n, d;
    double **dgg_c;
    if (!PyArg_ParseTuple(args, "Oii", &points_py, &n, &d))
        return NULL;
    dgg_c = _dgg_wrapper(points_py, n, d);
    if (!dgg_c)
        return NULL;
    dgg_py = matrix_c_to_py(dgg_c, n, d);
    free_matrix(dgg_c, n);
    return dgg_py;
}

double **_norm_wrapper(PyObject *points_py, int n, int d) {
    double **sym_c, **dgg_c, **norm_c;
    sym_c = _sym_wrapper(points_py, n, d);
    if (!sym_c)
        return NULL;
    dgg_c = calc_ddg(sym_c, n);
    if (!dgg_c) {
        free_matrix(sym_c, n);
        return NULL;
    }
    norm_c = calc_norm(sym_c, dgg_c, n);
    free_matrix(sym_c, n);
    free(dgg_c, n);
    return norm_c;
}

static PyObject *norm_wrapper(PyObject *self, PyObject *args) {
    PyObject *points_py, *norm_py;
    int n, d;
    double **norm_c;
    if (!PyArg_ParseTuple(args, "Oii", &points_py, &n, &d))
        return NULL;
    norm_c = _norm_wrapper(points_py, n, d);
    if (!norm_c)
        return NULL;
    norm_py = matrix_c_to_py(norm_c, n, d);
    free_matrix(norm_c, n);
    return norm_py;
}

static PyObject *symnmf_wrapper(PyObject *self, PyObject *args) {
    PyObject *W_py, *H_init_py;
    if (!PyArg_ParseTuple(args, "OO", &W_py, &H_init_py))
        return NULL;
    // TODO: Convert W_py and H_init_py to C, call calc_symnmf, convert result, free memory, and return.
    return Py_BuildValue(""); // Placeholder
}