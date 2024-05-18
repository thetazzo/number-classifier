#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    NF_Mat ti = nf_mat_alloc(NULL, 4, 2);
    NF_Mat to = nf_mat_alloc(NULL, 4, 1);

    NF_MAT_AT(ti, 0, 0) = 0;
    NF_MAT_AT(ti, 0, 1) = 0;
    NF_MAT_AT(ti, 1, 0) = 1;
    NF_MAT_AT(ti, 1, 1) = 0;
    NF_MAT_AT(ti, 2, 0) = 0;
    NF_MAT_AT(ti, 2, 1) = 1;
    NF_MAT_AT(ti, 3, 0) = 1;
    NF_MAT_AT(ti, 3, 1) = 1;

    NF_MAT_AT(to, 0, 0) = 0;
    NF_MAT_AT(to, 1, 0) = 1;
    NF_MAT_AT(to, 2, 0) = 1;
    NF_MAT_AT(to, 3, 0) = 0;

    NF_MAT_PRINT(ti);
    NF_MAT_PRINT(to);

    return 0;
}

