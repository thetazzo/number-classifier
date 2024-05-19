#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    NF_Mat m1 = nf_mat_alloc(NULL, 4, 2);
    NF_MAT_AT(m1, 0, 0) = 34;
    NF_MAT_AT(m1, 1, 1) = 20;
    NF_MAT_AT(m1, 2, 0) = 3;
    NF_MAT_AT(m1, 3, 1) = 2;

    NF_Mat m2 = nf_mat_alloc(NULL, 4, 2);
    NF_MAT_AT(m2, 0, 0) = 35;
    NF_MAT_AT(m2, 1, 1) = 400;
    NF_MAT_AT(m2, 2, 0) = 3;
    NF_MAT_AT(m2, 3, 1) = 40;

    nf_mat_sum(m1, m2);

    int valid = 0;

    NF_MAT_PRINT(m1);

    return valid;
}

