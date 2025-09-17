from setuptools import Extension, setup

module = Extension("mysymnmf", sources=["symnmfmodule.c", "symnmf.c"])
setup(
    name="mysymnmf",
    version="1.0",
    description="Python wrapper for symNMF extension",
    ext_modules=[module],
)
