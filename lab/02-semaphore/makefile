all:
	gcc a.c -o a.out
	gcc b.c -o b.out

testa:
	rm -rf file
	./a.out file
	cat file

testb:
	rm -rf file
	./b.out file
	cat file

clean:
	rm -rf *.out file