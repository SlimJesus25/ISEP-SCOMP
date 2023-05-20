consult: consult.c
	gcc -Wall consult.c -o consult -lrt

consultAll: consultAll.c
	gcc -Wall consultAll.c -o consultAll -lrt

insert: insert.c
	gcc -Wall insert.c -o insert -lrt

ex8 : ex8.c
	gcc -Wall ex8.c -o ex8 -lrt

run: ex8 consult consultAll insert
	./ex8

clean:
	find . -type f -not -name "*.*" -not -name "Makefile" -not -name "makefile" -delete



