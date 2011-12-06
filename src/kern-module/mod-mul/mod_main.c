
#include <types.h>
#include <mod.h>

int mul(int a, int b, int *c) {
    if (module_func_add == NULL) {
        return -1;
    }
    int i = 0;
    if (a > b) {
        int tmp = b;
        b = a;
        a = tmp;
    }
    *c = 0;
    for (; i < a; i++) {
        module_func_add(*c, b, c);
    }
    return 0;
}

void init_module() {
    register_mod_mul(mul);
}

void cleanup_module() {
    unregister_mod_mul();
}
