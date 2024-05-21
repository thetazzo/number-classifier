#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    srand(69);

    NF_Mat m1 = nf_mat_alloc(NULL, 3, 2);
    NF_MAT_AT(m1, 0, 0) = 0;
    NF_MAT_AT(m1, 0, 1) = 1;
    NF_MAT_AT(m1, 1, 0) = 2;
    NF_MAT_AT(m1, 1, 1) = 3;
    NF_MAT_AT(m1, 2, 0) = 4;
    NF_MAT_AT(m1, 2, 1) = 5;

    NF_MAT_PRINT(m1);
    printf("--------------------------------------------------\n");
    nf_mat_shuffle_rows(m1);
    NF_MAT_PRINT(m1);

    return 0;
}
