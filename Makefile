FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors -lm

symnmf: symnmf.c
	gcc -c symnmf.c $(FLAGS)
	
module: symnmfmodule.c setup.py
	python3 setup.py build_ext --inplace

clean:
	rm -f *.o
	rm -f *.so
	rm -rf build