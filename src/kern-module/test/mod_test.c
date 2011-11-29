
int init_module(int a, int b) {
    return a + b;
}

void cleanup_module() {
    kprintf("executing cleanup_module ...");
}
