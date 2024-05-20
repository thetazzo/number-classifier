#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    NF_Mat m1 = nf_mat_alloc(NULL, 4, 4);
    nf_mat_fill(m1, 1);

    NF_Mat m2 = nf_mat_alloc(NULL, 4, 4);
    nf_mat_fill(m2, 1);

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            float mv1 = NF_MAT_AT(m1, i, j);
            float mv2 = NF_MAT_AT(m2, i, j);
            NF_MAT_AT(m1, i, j) = mv1 - mv2; 
        }
    }
    NF_MAT_PRINT(m1);
    return 0;
}

