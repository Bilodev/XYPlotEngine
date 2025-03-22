link: Engine.o main.o utils.o
	gcc Engine.o main.o utils.o -o main.exe -lraylib -lm -lpthread -ldl -lrt  

utils.o:
	gcc -c ./Engine/utils.c -lm

Engine.o:
	gcc -c ./Engine/Engine.c -lm

main.o:
	gcc -c main.c -lm

clean:
	rm -f *.o *.exe