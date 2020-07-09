#include <stdio.h>
int foo() {printf("Link Test OK\n");}

int foo2(int x, int y){printf("%d %d\n", x , y);}

int foo3(int x, int y, int z){printf("%d %d %d\n", x, y, z);}

int bar(int x, int y){
	return x*y;
}
