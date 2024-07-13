#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "project03.h"
#include "rv_emu.h"

extern bool g_verbose;

void copy_array(int *dst, int *src, int len) {
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

void print_array(char *prefix, int *arr, int len) {
    printf("%s", prefix);
    for (int i = 0; i < len; i++) {
        printf(" %d", arr[i]);
    }
    printf("\n");
}

void quadratic_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int x = atoi(argv[start_arg]);
    int a = atoi(argv[start_arg + 1]);
    int b = atoi(argv[start_arg + 2]);
    int c = atoi(argv[start_arg + 3]);

    r = quadratic_c(x, a, b, c);
    printf("C: %d\n", r);

    r = quadratic_s(x, a, b, c);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t *) quadratic_s, x, a, b, c);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

void midpoint_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int start = atoi(argv[start_arg]);
    int end = atoi(argv[start_arg + 1]);

    r = midpoint_c(start, end);
    printf("C: %d\n", r);

    r = midpoint_s(start, end);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t *) midpoint_s, start, end, 0, 0);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

void max3_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int a = atoi(argv[start_arg]);
    int b = atoi(argv[start_arg + 1]);
    int c = atoi(argv[start_arg + 2]);

    r = max3_c(a, b, c);
    printf("C: %d\n", r);

    r = max3_s(a, b, c);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t *) max3_s, a, b, c, 0);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

void to_upper_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    // Make a copy of the string since we modify it in place
    char tmp[MAX_ARRAY];
    char *str = argv[start_arg];

    memset(tmp, '\0', MAX_ARRAY);
    to_upper_c(str, tmp);
    printf("C: %s\n", tmp);

    memset(tmp, '\0', MAX_ARRAY);
    to_upper_s(str, tmp);
    printf("Asm: %s\n", tmp);

    memset(tmp, '\0', MAX_ARRAY);
    rv_init(rsp, (uint32_t*) to_upper_s, (uint64_t) str, (uint64_t) tmp, 0, 0);
    rv_emulate(rsp);
    printf("Emu: %s\n", tmp);
}

void get_bitseq_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int n = atoi(argv[start_arg]);
    int start = atoi(argv[start_arg + 1]);
    int end = atoi(argv[start_arg + 2]);

    r = get_bitseq_c(n, start, end);
    printf("C: %d\n", r);

    r = get_bitseq_s(n, start, end);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t *) get_bitseq_s, n, start, end, 0);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

void get_bitseq_signed_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int n = atoi(argv[start_arg]);
    int start = atoi(argv[start_arg + 1]);
    int end = atoi(argv[start_arg + 2]);

    r = get_bitseq_signed_c(n, start, end);
    printf("C: %d\n", r);

    r = get_bitseq_signed_s(n, start, end);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t *) get_bitseq_signed_s, n, start, end, 0);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

void fib_rec_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int n = atoi(argv[start_arg]);

    r = fib_rec_c(n);
    printf("C: %d\n", r);

    r = fib_rec_s(n);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t*) fib_rec_s, n, 0, 0, 0);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

void swap_test(int start_arg, int argc, char **argv, struct rv_state_st *rsp) {
    int tmp[MAX_ARRAY];
    int a[MAX_ARRAY] = {0,};
    int len = 0;
    int i, j;

    i = atoi(argv[start_arg]);
    start_arg += 1;
    j = atoi(argv[start_arg]);
    start_arg += 1;

    for (int i = 0; i < argc - start_arg; i++) {
        a[i] = atoi(argv[i + start_arg]);
        len++;
    }
    
    copy_array(tmp, a, len);
    swap_c(tmp, i, j);
    print_array("C:", tmp, len);

    copy_array(tmp, a, len);
    swap_c(tmp, i, j);
    print_array("Asm:", tmp, len);

    copy_array(tmp, a, len);
    rv_init(rsp, (uint32_t*) swap_s, (uint64_t) tmp, i, j, 0);
    rv_emulate(rsp);
    print_array("Emu:", tmp, len);
 
}

void sort_test(int start_arg, int argc, char **argv, struct rv_state_st *rsp) {
    int tmp[MAX_ARRAY];
    int a[MAX_ARRAY] = {0,};
    int len = 0;

    for (int i = 0; i < argc - start_arg; i++) {
        a[i] = atoi(argv[i + start_arg]);
        len++;
    }
    
    copy_array(tmp, a, len);
    sort_c(tmp, len);
    print_array("C:", tmp, len);

    copy_array(tmp, a, len);
    sort_s(tmp, len);
    print_array("Asm:", tmp, len);

    copy_array(tmp, a, len);
    rv_init(rsp, (uint32_t*) sort_s, (uint64_t) tmp, len, 0, 0);
    rv_emulate(rsp);
    print_array("Emu:", tmp, len);
 
}

void add2_test(int start_arg, char **argv, struct rv_state_st *rsp) {
    int r;
    int a = atoi(argv[start_arg]);
    int b = atoi(argv[start_arg + 1]);

    r = add2_c(a, b);
    printf("C: %d\n", r);

    r = add2_s(a, b);
    printf("Asm: %d\n", r);

    rv_init(rsp, (uint32_t *) add2_s, a, b, 0, 0);
    r = rv_emulate(rsp);
    printf("Emu: %d\n", r);
}

int parse_params(int argc, char **argv, struct rv_state_st *rsp) {
    // Parse optional command line parameters which must precede
    // the name of the emulated program and its parameters
    rsp->analyze = false;
    rsp->cache_sim = false;
    rsp->i_cache.type = CACHE_NONE;
    
    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            // Return the index of the first non-optional parameter,
            // which is the name of the emulated program
            break;
        } else if (!strcmp(argv[i], "-a")) {
            rsp->analyze = true;
        } else if (!strcmp(argv[i], "-dm")) {
            rsp->cache_sim = true;
            rsp->i_cache.type = CACHE_DM;
            rsp->i_cache.size = atoi(argv[i + 1]);
            rsp->i_cache.block_size = atoi(argv[i + 2]);
            rsp->i_cache.ways = 1;
            i += 2;
        } else if (!strcmp(argv[i], "-sa")) {
            rsp->cache_sim = true;
            rsp->i_cache.type = CACHE_SA;
            rsp->i_cache.size = atoi(argv[i + 1]);
            rsp->i_cache.block_size = atoi(argv[i + 2]);
            rsp->i_cache.ways = atoi(argv[i + 3]);
            i += 3;
        } else if (!strcmp(argv[i], "-v")) {
            rsp->verbose = true;
            g_verbose = true;
        }
    }
    return i;
}

void usage() {
    printf("usage: project04 <prog> [<arg1> ...]\n");
    exit(-1);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        usage();
    }
    
    struct rv_state_st state;
    int i = parse_params(argc, argv, &state);

    if (!strcmp(argv[i], "quadratic")) {
        quadratic_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "midpoint")) {
        midpoint_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "max3")) {
        max3_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "to_upper")) {
        to_upper_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "get_bitseq")) {
        get_bitseq_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "get_bitseq_signed")) {
        get_bitseq_signed_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "fib_rec")) {
        fib_rec_test(i + 1, argv, &state);
    } else if (!strcmp(argv[i], "swap")) {
        swap_test(i + 1, argc, argv, &state);
    } else if (!strcmp(argv[i], "sort")) {
        sort_test(i + 1, argc, argv, &state);
    } else if (!strcmp(argv[1], "add2")) {
        add2_test(i + 1, argv, &state);
    } else {
        usage();
    }

    if (state.analyze) {
        rv_print(&state.analysis);
    }

    if (state.cache_sim) {
        cache_print(&state.i_cache, "(I)");
    }

    return 0;
}
