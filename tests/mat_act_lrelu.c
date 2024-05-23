#define NF_NN_ACT NF_ACT_LRELU
#define NF_IMPLEMENTATION
#include "../libs/nf.h"

int main(void)
{
    srand(69);

    NF_Mat m1 = nf_mat_alloc(NULL, 4, 4);
    NF_MAT_AT(m1, 0, 0) = -28.9238;
    NF_MAT_AT(m1, 0, 1) =  53.4480;
    NF_MAT_AT(m1, 0, 2) = -71.0551;
    NF_MAT_AT(m1, 0, 3) =  16.6250;
    NF_MAT_AT(m1, 1, 0) =  68.0991;
    NF_MAT_AT(m1, 1, 1) = -71.7835;
    NF_MAT_AT(m1, 1, 2) =  96.7467;
    NF_MAT_AT(m1, 1, 3) = -42.1679;
    NF_MAT_AT(m1, 2, 0) =  76.8027;
    NF_MAT_AT(m1, 2, 1) = -79.0028;
    NF_MAT_AT(m1, 2, 2) =  23.8032;
    NF_MAT_AT(m1, 2, 3) = -38.2792;
    NF_MAT_AT(m1, 3, 0) =  60.1184;
    NF_MAT_AT(m1, 3, 1) = -15.8995;
    NF_MAT_AT(m1, 3, 2) =  97.5887;
    NF_MAT_AT(m1, 3, 3) =  81.3468;

    nf_mat_act(m1);

    NF_MAT_PRINT(m1);

    NF_Mat expected = nf_mat_alloc(NULL, 4, 4);
    NF_MAT_AT(expected, 0, 0) = -0.289238;
    NF_MAT_AT(expected, 0, 1) = 53.4480;
    NF_MAT_AT(expected, 0, 2) = -0.710551;
    NF_MAT_AT(expected, 0, 3) = 16.6250;
    NF_MAT_AT(expected, 1, 0) = 68.0991;
    NF_MAT_AT(expected, 1, 1) = -0.717835;
    NF_MAT_AT(expected, 1, 2) = 96.7468;
    NF_MAT_AT(expected, 1, 3) = -0.421679; 
    NF_MAT_AT(expected, 2, 0) = 76.8027;
    NF_MAT_AT(expected, 2, 1) = -0.790028;
    NF_MAT_AT(expected, 2, 2) = 23.8032;
    NF_MAT_AT(expected, 2, 3) = -0.382792;
    NF_MAT_AT(expected, 3, 0) = 60.1184;
    NF_MAT_AT(expected, 3, 1) = -0.158995;
    NF_MAT_AT(expected, 3, 2) = 97.5887;
    NF_MAT_AT(expected, 3, 3) = 81.3468;

    for (size_t i = 0; i < m1.rows; ++i) {
        for (size_t j = 0; j < m1.cols; ++j) {
            float diff = NF_MAT_AT(expected, i, j) - NF_MAT_AT(m1, i, j);
            if (diff > 0.002) {
                printf("NOT OK!\n");
                return 1;
            }
        }
    }
    printf("OK!\n");
    printf("Computation is whitin %s error\n", "0.2%");
    return 0;
}

