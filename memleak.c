//gcc memleak.c -o memleak -g

#include"memleak_hook.h"
// kvstore -c kv.conf

//hook
typedef void*(*malloc_t)(size_t size);
malloc_t malloc_f=NULL;
typedef void (*free_t)(void *ptr);
free_t free_f=NULL;

//printf内部使用了malloc，防止递归调用定义下面两个变量
int enable_malloc=1;
int enable_free=1;

//将一个给定的地址转换为相对于 ELF 可执行文件基址的偏移量
void *ConvertToELF(void *addr){
    Dl_info info;   //存储 `dladdr1` 函数返回的动态链接信息
    struct link_map *link; //用于存储动态链接的映射信息
    dladdr1(addr,&info,(void**)&link,RTLD_DL_LINKMAP);
    return (void*)((size_t)addr-link->l_addr);
}

void *malloc(size_t size){
    void *p=NULL;
    if(enable_malloc){
        enable_malloc=0;
        p=malloc_f(size);
        //使用了gcc内置函数，获取是哪一个函数调用当前函数的地址，0是上一层
        void *caller=__builtin_return_address(0);

        char buff[128]={0};
        snprintf(buff,128,"./mem/%p.mem",p);

        FILE *fp=fopen(buff,"w");
        if(!fp){
            free(p);
            return NULL;
        }
        fprintf(fp, "[+]%p, addr: %p, size: %ld\n", ConvertToELF(caller), p, size);
		fflush(fp);

        enable_malloc=1;
    }else{
        p=malloc_f(size);
    }
    return p;
}

void free(void *ptr){
    if(enable_free){
        enable_free=0;
        
        char buff[128]={0};
        snprintf(buff,128,"./mem/%p.mem",ptr);

        //这段代码的作用是在尝试删除文件时检查是否发生了错误（返回值小于 0），如果发生错误，则打印一条消息并返回
        if (unlink(buff) < 0) {
			printf("double free: %p\n", ptr);
			return;
		}
		free_f(ptr);
		enable_free = 1;
    }else{
        free_f(ptr);
    }
    return;
}

void init_hook(void) {

	if (!malloc_f) {
		malloc_f = (malloc_t)dlsym(RTLD_NEXT, "malloc");
	}
	if (!free_f) {
		free_f = (free_t)dlsym(RTLD_NEXT, "free");
	}
}

#if 0
int main() {

	init_hook();

    //malloc函数会按内存池的方式分配0x20,0x30...防止内存碎片
	void *p1 = malloc(5);
	void *p2 = malloc(10);  
	void *p3 = malloc(35);
	void *p4 = malloc(10);

	free(p1);
	free(p3);
	free(p4);

    return 0;
} 
//addr2line -f -e ./memleak -a 地址
#endif
