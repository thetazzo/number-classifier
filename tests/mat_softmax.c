#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    srand(25854);

    NF_Mat m1 = nf_mat_alloc(NULL, 1, 16);
    nf_mat_rand(m1, -1, 1);

    nf_mat_softmax(m1);

    NF_MAT_PRINT(m1);

    for (size_t i = 0; i < m1.rows; ++i) {
        for (size_t j = 0; j < m1.cols; ++j) {
            if (NF_MAT_AT(m1, i, j) > 1) {
                printf("NOT OK!\n");
                return 1;
            } else if (NF_MAT_AT(m1, i, j) < 0) {
                printf("NOT OK!\n");
                return 1;
            }
        }
    }
    printf("OK!\n");
    return 0;
}
