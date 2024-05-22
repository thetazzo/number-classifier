#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    srand(69);

    NF_Mat m1 = nf_mat_alloc(NULL, 4, 4);
    nf_mat_rand(m1, -3, 6);

    NF_Mat row3 = nf_mat_row(m1, 2);
    NF_MAT_PRINT(row3);

    return 0;
}
