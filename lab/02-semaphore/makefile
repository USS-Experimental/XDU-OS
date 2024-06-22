all:
	gcc a.c -o a.out -lpthread
	gcc b.c -o b.out -lpthread

testa:
	rm -rf file
	strace -f ./a.out file
	cat file

testb:
	rm -rf file
	./b.out file
	cat file

clean:
	rm -rf *.out file