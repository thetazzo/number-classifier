#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    srand(69);

    NF_Mat m1 = nf_mat_alloc(NULL, 5, 5);
    nf_mat_rand(m1, -1, 1);
    NF_Mat m2 = nf_mat_alloc(NULL, 5, 5);

    nf_mat_copy(m2, m1);

    for (size_t i = 0; i < m1.rows; ++i) {
        for (size_t j = 0; j < m1.cols; ++j) {
            if (NF_MAT_AT(m1, i, j) != NF_MAT_AT(m2, i, j)) {
                printf("`m1` and `m2` are not equal\n");
                return 1;
            }
        }
    }
    printf("OK!\n");
    return 0;
}
