#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main()
{
    srand(69);

    NF_Mat prob1 = nf_mat_alloc(NULL, 4, 4);
    nf_mat_rand(prob1, -1 , 1);

    NF_Mat prob2 = nf_mat_alloc(NULL, 4, 4);
    nf_mat_rand(prob2, -1 , 1);

    NF_MAT_PRINT(prob1);
    printf("--------------------------------------------------\n");
    NF_MAT_PRINT(prob2);
    return 0;
}
