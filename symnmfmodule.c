#define PY_SSIZE_T_CLEAN
#include "symnmf.h"
#include <Python.h>

static PyMethodDef ModuleMethods[] = {
    {"FUNC",
     (PyCFunction)NULL,
     METH_VARARGS,
     PyDoc_STR("DESC")},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef mymodule = {
    PyModuleDef_HEAD_INIT,
    "mysymnmf",
    NULL,
    -1,
    ModuleMethods};

PyMODINIT_FUNC PyInit_mysymnmf(void) {
    PyObject *m = PyModule_Create(&mymodule);
    return !m ? NULL : m;
}