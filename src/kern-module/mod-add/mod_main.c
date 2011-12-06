
int add(int a, int b, int *c) {
    *c = a + b;
    return 0;
}

void init_module() {
    register_mod_add(add);
}

void cleanup_module() {
    unregister_mod_add();
}
