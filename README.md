target.c是memleak.c和memleak_hook.h作为内存泄漏检测组件的演示案例，通过gcc memleak.c target.c -o target -g
可以检测target.c是否有内存泄漏
