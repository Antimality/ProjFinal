FLAGS = -Wall -Wextra -pedantic-errors -lm

symnmf: symnmf.c symnmf.h
	gcc symnmf.c -o symnmf $(FLAGS)
	
module: symnmfmodule.c setup.py symnmf.c symnmf.h
	rm -f *.so
	rm -rf build
	python3 setup.py build_ext --inplace

clean:
	rm -f *.o
	rm -f *.so
	rm -rf build
	rm -f symnmf
	rm -rf __pycache__

// TODO: Add back ANSI and Werror