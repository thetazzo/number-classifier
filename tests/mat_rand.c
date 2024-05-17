#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main()
{
    NF_Mat prob1 = nf_mat_alloc(NULL, 4, 4);
    nf_mat_rand(prob1, -1 , 1);

    NF_Mat prob2 = nf_mat_alloc(NULL, 4, 4);
//    nf_mat_rand(prob2, -1 , 1);
    nf_mat_copy(prob2, prob1);

    size_t count = 0;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            float pv1 = NF_MAT_AT(prob1, i, j);
            float pv2 = NF_MAT_AT(prob2, i, j);
            if (pv1 == pv2) {
                count += 1;
            }
        }
    }
    if (count == 16) {
        return 1;
    }
    return 0;
}
