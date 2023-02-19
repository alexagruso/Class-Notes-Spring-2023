all: p2 p3 p4

p2: P2.c
	gcc -lm P2.c -o P2.out

p3: P3.c
	gcc -lm P3.c -o P3.out

p4: P4.c
	gcc -lm P4.c -o P4.out

sample:
	./P2.out >> P2.txt
	./P3.out >> P3.txt
	./P4.out >> P4.txt

clean:
	rm *.out
