#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    srand(69);

    NF_Mat m1 = nf_mat_alloc(NULL, 3, 8);
    nf_mat_rand(m1, 4, 6);

    nf_mat_print(m1, "m1: ", 0);
    printf("---------------------------------------------------------------------------\n");
    NF_MAT_PRINT(m1);

    return 0;
}
