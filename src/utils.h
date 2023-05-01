#ifndef BENCHMARKS_UTILS_H
#define BENCHMARKS_UTILS_H

#undef ensure
#define ensure(x)                                                                           \
    do {                                                                                    \
        if (!(x)) {                                                                         \
            fprintf(stderr, "@@@ [%s:%d] Assert failed: '%s' @@@", __FILE__, __LINE__, #x); \
            exit(1);                                                                        \
        }                                                                                   \
    } while (0)

// Sample struct use for benchmarks
typedef struct test_t {
    int a;
    int b;
    const char* str;
    uint64_t t;
} test_t;

#endif // BENCHMARKS_UTILS_H
