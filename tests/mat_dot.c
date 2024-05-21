#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    NF_Mat m1 = nf_mat_alloc(NULL, 1, 2);
    NF_MAT_AT(m1, 0, 0) = 33.5;
    NF_MAT_AT(m1, 0, 1) = 2;

    NF_Mat m2 = nf_mat_alloc(NULL, 2, 1);
    NF_MAT_AT(m2, 0, 0) = 2;
    NF_MAT_AT(m2, 1, 0) = 210;

    NF_Mat m3 = nf_mat_alloc(NULL, 1, 1);

    nf_mat_dot(m3, m1, m2);

    NF_MAT_PRINT(m1);
    NF_MAT_PRINT(m2);
    printf("--------------------------------------------------\n");
    NF_MAT_PRINT(m3);

    return 0;
}
