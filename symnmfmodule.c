#define PY_SSIZE_T_CLEAN
#include "symnmfmodule.h"
#include "symnmf.h"
#include <Python.h>

/*
 * ============================================================================
 * Python C API Method Definitions
 * ============================================================================
 */

/* Method definition object for this extension, mapping python method names to C functions */
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
    "mysymnmf",
    "Python C API for symmetric NMF algorithm.",
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
 * Matrix c/py Convertion Functions
 * ============================================================================
 */

double **matrix_py_to_c(PyObject *py_matrix, int n, int m) {
    Py_ssize_t i, j;
    PyObject *row, *item;
    double **c_matrix;

    c_matrix = (double **)malloc((n) * sizeof(double *));
    if (!c_matrix)
        return NULL;

    for (i = 0; i < n; i++) {
        /* Read rows */
        row = PyList_GetItem(py_matrix, i);
        if (!PyList_Check(row) || PyList_Size(row) != m) {
            free_matrix(c_matrix, i);
            return NULL;
        }

        c_matrix[i] = (double *)malloc((m) * sizeof(double));
        if (!c_matrix[i]) {
            free_matrix(c_matrix, i);
            return NULL;
        }

        for (j = 0; j < m; j++) {
            /* Read columns */
            item = PyList_GetItem(row, j);
            c_matrix[i][j] = PyFloat_AsDouble(item);
            if (PyErr_Occurred()) {
                free_matrix(c_matrix, i);
                return NULL;
            }
        }
    }

    return c_matrix;
}

PyObject *matrix_c_to_py(double **c_matrix, int n, int m) {
    PyObject *py_matrix, *row_list, *num;
    int i, j;

    if (!c_matrix)
        return NULL;

    py_matrix = PyList_New(n);
    if (!py_matrix)
        return NULL;

    for (i = 0; i < n; i++) {
        /* Read rows */
        row_list = PyList_New(m);
        if (!row_list) {
            Py_DECREF(py_matrix);
            free_matrix(c_matrix, n);
            return NULL;
        }

        for (j = 0; j < m; j++) {
            /* Read columns */
            num = PyFloat_FromDouble(c_matrix[i][j]);
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

/*
 * ============================================================================
 * Wrapper Function Implementations
 * ============================================================================
 */

double **_sym_wrapper(PyObject *points_py, int n, int d) {
    double **points_c, **sym_c;

    /* Translate point matrix to C */
    points_c = matrix_py_to_c(points_py, n, d);
    if (!points_c)
        return NULL;

    /* Calculate sym matrix */
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

    /* Calculate sym matrix*/
    sym_c = _sym_wrapper(points_py, n, d);
    if (!sym_c)
        return NULL;

    /* Translate matrix to Python*/
    sym_py = matrix_c_to_py(sym_c, n, n);
    free_matrix(sym_c, n);

    return sym_py;
}

double **_dgg_wrapper(PyObject *points_py, int n, int d) {
    double **sym_c, **dgg_c;

    /* Calculate sym matrix */
    sym_c = _sym_wrapper(points_py, n, d);
    if (!sym_c)
        return NULL;

    /* Calculate ddg matrix */
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

    /* Calculate dgg matrix */
    dgg_c = _dgg_wrapper(points_py, n, d);
    if (!dgg_c)
        return NULL;

    /* Translate matrix to Python */
    dgg_py = matrix_c_to_py(dgg_c, n, n);
    free_matrix(dgg_c, n);

    return dgg_py;
}

double **_norm_wrapper(PyObject *points_py, int n, int d) {
    double **sym_c, **dgg_c, **norm_c;

    /* Calculate sym matrix */
    sym_c = _sym_wrapper(points_py, n, d);
    if (!sym_c)
        return NULL;

    /* Calculate ddg matrix */
    dgg_c = calc_ddg(sym_c, n);
    if (!dgg_c) {
        free_matrix(sym_c, n);
        return NULL;
    }

    /* Calculate norm matrix */
    norm_c = calc_norm(sym_c, dgg_c, n);
    free_matrix(sym_c, n);
    free_matrix(dgg_c, n);

    return norm_c;
}

static PyObject *norm_wrapper(PyObject *self, PyObject *args) {
    PyObject *points_py, *norm_py;
    int n, d;
    double **norm_c;

    if (!PyArg_ParseTuple(args, "Oii", &points_py, &n, &d))
        return NULL;

    /* Calculate norm matrix */
    norm_c = _norm_wrapper(points_py, n, d);
    if (!norm_c)
        return NULL;

    /* Translate matrix to Python*/
    norm_py = matrix_c_to_py(norm_c, n, n);
    free_matrix(norm_c, n);

    return norm_py;
}

static PyObject *symnmf_wrapper(PyObject *self, PyObject *args) {
    PyObject *W_py, *H_init_py, *H_py;
    double **W_c, **H_init_c, **H_c;
    int n, k;

    if (!PyArg_ParseTuple(args, "OOii", &W_py, &H_init_py, &n, &k))
        return NULL;

    /* Translate W matrix to C */
    W_c = matrix_py_to_c(W_py, n, n);
    if (!W_c)
        return NULL;

    /* Translate initial H matrix to C */
    H_init_c = matrix_py_to_c(H_init_py, n, k);
    if (!H_init_c) {
        free_matrix(W_c, n);
        return NULL;
    }

    /* Calculate H matrix */
    H_c = calc_symnmf(W_c, H_init_c, n, k);
    free_matrix(W_c, n);

    /* Translate H matrix to Python*/
    H_py = matrix_c_to_py(H_c, n, k);
    free_matrix(H_c, n);

    return H_py;
}