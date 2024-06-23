//gcc memleak.c target.c -o target -g

#include<stdio.h>
#include<stdlib.h>
#include"memleak_hook.h"

int main(){
    init_hook();

    //malloc函数会按内存池的方式分配0x20,0x30...防止内存碎片
	void *p1 = malloc(5);
	void *p2 = malloc(10);  
	void *p3 = malloc(35);

	free(p1);
	free(p2);

    return 0;
}

//addr2line -f -e ./target -a地址