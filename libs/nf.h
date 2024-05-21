#ifndef NF_H_
#define NF_H_ 

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define NF_BACKPROP_TRADITIONAL

#ifndef NF_NN_ACT
#define NF_NN_ACT NF_ACT_TANH
#endif //NF_NN_ACT

#ifndef NF_MALLOC
#include <stdlib.h>
#define NF_MALLOC malloc
#endif // NF_MALLOC

#ifndef NF_ASSERT
#include <assert.h>
#define NF_ASSERT assert
#endif // NF_ASSERT

#ifndef ARRAY_LEN
#define ARRAY_LEN(xs) sizeof((xs))/sizeof((xs)[0])
#endif // ARRAY_LEN

float rand_float(void);

// ====================================================================
//                          Activation Functions
// ====================================================================
typedef enum {
    NF_ACT_SIG,
    NF_ACT_RELU,
    NF_ACT_LRELU,
    NF_ACT_TANH,
    NF_ACT_SIN,
} NF_Act;

// Activation name represented as a string
char *nf_activation_as_str(void);
// Normalization function name represented as a string
char *nf_normf_as_str(void);

float nf_sigmoidf(float x);
float nf_reluf(float x);
float nf_lreluf(float x);
float nf_tanhf(float x);
float nf_gelu(float x);

// ====================================================================
//          Arena Allocator ~ Region Based Memory Management
// ====================================================================
typedef struct {
    size_t capacity_;
    size_t size_;
    uintptr_t *words;
} Region;

// capacity is in bytes but it can allocate more just to keep words(data) alligned
Region region_alloc_alloc(size_t capacity);
void *region_alloc(Region *r, size_t size);
// TODO: reset -> release
#define region_reset(r) if (r) (r)->size_ = 0
#define region_occupied_bytes(r) (NF_ASSERT((r) != NULL) , (r)->size_*sizeof(*(r)->words))

// ====================================================================
//                 Declatrations For Matrix Operations
// ====================================================================
typedef struct {
    size_t rows;
    size_t cols;
    size_t stride;
    float  *es;
} NF_Mat;

// TODO: write docs
// TODO: write test
#define NF_MAT_AT(m, i, j) (m).es[(i)*(m).stride + (j)]
// Allocate memory for a matrix
// TODO: write docs
NF_Mat nf_mat_alloc(Region *r, size_t rows, size_t cols);
// TODO: write docs
// TODO: write test
void nf_mat_save(FILE *out, NF_Mat m);
// TODO: write docs
// TODO: write test
NF_Mat nf_mat_load(Region *r, FILE *in);
// TODO: write docs
void nf_mat_rand(NF_Mat m, float low, float high);
// TODO: write docs
void nf_mat_fill(NF_Mat m, float a);
// TODO: write docs
// TODO: write test
NF_Mat nf_mat_row(NF_Mat m, size_t row);
// TODO: write docs
void nf_mat_copy(NF_Mat dst, NF_Mat src);
// TODO: write docs
void nf_mat_dot(NF_Mat dst, NF_Mat a, NF_Mat b);
// TODO: write docs
void nf_mat_sum(NF_Mat dst, NF_Mat a);
// TODO: write docs
void nf_mat_shuffle_rows(NF_Mat m);
// TODO: write docs
// TODO: write test
void nf_mat_print(NF_Mat m, const char *name, size_t padding);
// TODO: write docs
// TODO: write test
#define NF_MAT_PRINT(m)  nf_mat_print((m), #m, 0)

// Handling the activation of the matrix
void nf_mat_act(NF_Mat m);

// ====================================================================
// Neural Network Normalization Function ~ NF_NN_NORMF
//     - this macro defines which function should be used for normalizing
//       the output of the neural model
//     - In most cases this function convert the output to a probability
//       destribution
//     - Normalization is applied on the output matrix if and only if the 
//       NF_NN_NORMF macro is defined
//     - The macro can be defined as one of the following:
//           - NF_NORMF_SOFTMAX
// ====================================================================
typedef enum {
    NF_NORMF_SOFTMAX,
    // TODO: NF_NORMF_ARGMAX
    // TODO: NF_NORMF_ARGMIN
} NF_Normf;

// ====================================================================
// Normalize the output column of the matrix using SOFTMAX function
//     - transforms the output of the matrix into a probability destribution
// ====================================================================
void nf_softmax(NF_Mat m);

// ====================================================================
//                     Neural Network Declarations 
// ====================================================================
typedef struct {
    size_t *arch;
    size_t arch_count; 
    NF_Mat *ws; // amount of total weights is arch_count-1 
    NF_Mat *bs; // amount of total biases is arch_count-1 
    
    // TODO: maybe remove this? It might be better to allocate the activations in a
    // temporary memory region during the forwarding
    NF_Mat *as; 
} NF_NN;

#define NF_NN_INPUT(nn) (NF_ASSERT((nn).arch_count > 0), (nn).as[0])
#define NF_NN_OUTPUT(nn) (NF_ASSERT((nn).arch_count > 0), (nn).as[(nn).arch_count-1])

NF_NN nf_nn_alloc(Region *r, size_t *arch, size_t arch_count);
void nf_nn_fill(NF_NN nn, float a);
void nf_nn_print(NF_NN nn, const char *name);
#define NF_NN_PRINT(nn) nf_nn_print((nn), #nn) 
void nf_nn_rand(NF_NN nn, float low, float high);
void nf_nn_forward(NF_NN nn);
float nf_nn_cost(NF_NN nn, NF_Mat ti, NF_Mat to);
NF_NN nf_nn_finite_diff(Region *r, NF_NN nn, NF_Mat ti, NF_Mat to, float eps);
NF_NN nf_nn_backprop(Region *r, NF_NN nn, NF_Mat ti, NF_Mat to);
void nf_nn_learn(NF_NN nn, NF_NN gn, float rate);

typedef struct {
    size_t begin;
    float cost;
    bool done;
} Batch;

void nf_batch_process(Region *r, Batch *b, size_t batch_size, NF_NN nn, NF_Mat td, float rate);

#ifdef NF_IMAGE_GENERATION
#include "stb_image.h"
#include "stb_image_write.h"

void nf_v_render_single_frame(NF_NN nn, float img_index);
int  nf_v_render_upscaled_screenshot(NF_NN nn, float img_index, const char *out_file_path);
int  nf_v_render_upscaled_video(NF_NN nn, float duration, const char *out_file_path);
#endif //NF_IMAGE_GENERATION

#endif // NF_H_

#ifdef NF_IMPLEMENTATION

//-------------------------------------
//  Utility Functions Implementations
//-------------------------------------
float rand_float(void)
{
    return (float)rand()/(float)RAND_MAX;
}

// ==========================================
//            Activation Functions
// ==========================================

char *nf_activation_as_str(void)
{
    switch (NF_NN_ACT) {
        case NF_ACT_SIG:
            return "SIGMOID";
        case NF_ACT_RELU: 
            return "RELU";
        case NF_ACT_LRELU:
            return "LEAKY RELU";
        case NF_ACT_TANH:
            return "TANH";
        case NF_ACT_SIN:
            return "SIN";
        default:
            NF_ASSERT(0 && "Unreachable");
    }
}

float nf_sigmoidf(float x)
{
    return 1.f / (1.f + expf(-x));
}

float nf_reluf(float x)
{
    if (x > 0) {
        return x;
    }
    return 0;
}

float nf_lreluf(float x)
{
    if (x < 0) {
        return 0.01*x;
    }
    return x;
}

float nf_tanhf(float x)
{
    float ex = expf(x);
    float enx = expf(-x);
    return (ex - enx) / (ex + enx);
}

float nf_gelu(float x)
{
    (void)x;
    NF_ASSERT(0 && "TODO: implement!");
}

// ==========================================
//            Normalization Functions
// ==========================================

char *nf_normf_as_str(void)
{
#ifdef NF_NN_NORMF
    switch (NF_NN_NORMF) {
        case NF_NORMF_SOFTMAX:
            return "SOFTMAX";
        default:
            NF_ASSERT(0 && "unreachable");
    }
#else
    return "NONE";
#endif
}

void nf_softmax(NF_Mat m)
{
    float exps = 0.f; // sum of the output column
    // Calculate the sum of the output column
    for (size_t i = 0; i < m.cols; ++i) {
        float yi = NF_MAT_AT(m, 0, i);
        exps += expf(yi);
    }

    // apply softmax
    for (size_t i = 0; i < m.cols; ++i) {
        float ex = expf(NF_MAT_AT(m, 0, i));
        NF_MAT_AT(m, 0, i) = ex / exps;
    }
}

/**
 * -------------------------------------
 *        Matrix Implementations 
 * -------------------------------------
 */

NF_Mat nf_mat_alloc(Region *r, size_t rows, size_t cols)
{
    NF_Mat m = {0}; 
    m.rows = rows;
    m.cols = cols;
    m.stride = cols;
    m.es = region_alloc(r, sizeof(*m.es) * rows * cols);
    NF_ASSERT(m.es != NULL);
    return m;
}

void nf_mat_save(FILE *out, NF_Mat m)
{
    const char *magic = "nn.h.mat";
    fwrite(magic, strlen(magic), 1, out);
    fwrite(&m.rows, sizeof(m.rows), 1, out);
    fwrite(&m.cols, sizeof(m.cols), 1, out);
    for (size_t i = 0; i < m.rows; ++i) {
        size_t n = fwrite(&NF_MAT_AT(m, i, 0), sizeof(*m.es), m.rows*m.cols, out);
        while (n < m.cols && !ferror(out)) {
            size_t k = fwrite(m.es + n, sizeof(*m.es), m.cols - n, out);
            n += k;
        }
    }
}

NF_Mat nf_mat_load(Region *r, FILE *in)
{
    uint64_t magic;
    fread(&magic, sizeof(magic), 1, in);
    NF_ASSERT(magic == 0x74616d2e682e6e6e);
    size_t rows, cols;
    fread(&rows, sizeof(rows), 1, in);
    fread(&cols, sizeof(cols), 1, in);
    NF_Mat m = nf_mat_alloc(r, rows, cols);
    
    size_t n = fread(m.es, sizeof(*m.es), rows*cols, in);
    while (n < rows*cols && !ferror(in)) {
        size_t k = fread(m.es, sizeof(*m.es) + n, rows*cols - n, in);
        n += k;
    }
    return m;
}

void nf_mat_rand(NF_Mat m, float low, float high)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            NF_MAT_AT(m, i, j) = rand_float()*(high - low) + low;
        }
    }
}

void nf_mat_fill(NF_Mat m, float a)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            NF_MAT_AT(m, i, j) = a;
        }
    }
}

NF_Mat nf_mat_row(NF_Mat m, size_t row)
{
    return (NF_Mat) {
        .rows = 1,
        .cols = m.cols,
        .stride = m.stride, 
        .es = &NF_MAT_AT(m, row, 0),
    };
}

void nf_mat_copy(NF_Mat dst, NF_Mat src)
{
    NF_ASSERT(dst.rows == src.rows);
    NF_ASSERT(dst.cols == src.cols);

    for (size_t i = 0; i < dst.rows; ++i) {
        for (size_t j = 0; j < dst.cols; ++j) {
            NF_MAT_AT(dst, i, j) = NF_MAT_AT(src, i, j);
        }
    }
}

void nf_mat_dot(NF_Mat dst, NF_Mat a, NF_Mat b)
{
    // validate rules of matrix multiplication
    NF_ASSERT(a.cols == b.rows);
    size_t n = a.cols;
    NF_ASSERT(dst.rows == a.rows);
    NF_ASSERT(dst.cols == b.cols);

    for (size_t i = 0; i < dst.rows; ++i) {
        for (size_t j = 0; j < dst.cols; ++j) {
            NF_MAT_AT(dst, i, j) = 0;
            for (size_t k = 0; k < n; ++k) {
                NF_MAT_AT(dst, i, j) += NF_MAT_AT(a, i, k) * NF_MAT_AT(b, k, j);
            }
        }
    }
}

void nf_mat_sum(NF_Mat dst, NF_Mat a)
{
    // validate that matrices are of the same order
    NF_ASSERT(dst.rows == a.rows);
    NF_ASSERT(dst.cols == a.cols);

    for (size_t i = 0; i < dst.rows; ++i) {
        for (size_t j = 0; j < dst.cols; ++j) {
            NF_MAT_AT(dst, i, j) += NF_MAT_AT(a, i, j);
        }
    }
}

void nf_mat_shuffle_rows(NF_Mat m)
{
    for (size_t i = 0; i < m.rows; ++i) {
        size_t j = i+rand()%(m.rows - i);
        if (i != j) {
            for (size_t k = 0; k < m.cols; ++k) {
                float t = NF_MAT_AT(m, i, k);
                NF_MAT_AT(m, i, k) = NF_MAT_AT(m, j, k);
                NF_MAT_AT(m, j, k) = t;
            }
        }
    }
}

void nf_mat_print(NF_Mat m, const char *name, size_t padding)
{
    printf("%*s%s = [\n", (int) padding, "", name);
    for (size_t i = 0; i < m.rows; ++i) {
        printf("%*s    ", (int) padding, "");
        for (size_t j = 0; j < m.cols; ++j) {
            printf("%f ",  NF_MAT_AT(m, i, j));
        }
        printf("\n");
    }
    printf("%*s]\n", (int) padding, "");
}

void nf_mat_act(NF_Mat m)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            switch (NF_NN_ACT) {
                case NF_ACT_SIG:
                    NF_MAT_AT(m, i, j) = nf_sigmoidf(NF_MAT_AT(m, i, j));
                    break;
                case NF_ACT_RELU: 
                    NF_MAT_AT(m, i, j) = nf_reluf(NF_MAT_AT(m, i, j));
                    break;
                case NF_ACT_LRELU:
                    NF_MAT_AT(m, i, j) = nf_lreluf(NF_MAT_AT(m, i, j));
                    break;
                case NF_ACT_TANH:
                    NF_MAT_AT(m, i, j) = nf_tanhf(NF_MAT_AT(m, i, j));
                    break;
                case NF_ACT_SIN:
                    NF_MAT_AT(m, i, j) = sinf(NF_MAT_AT(m, i, j));
                    break;
                default:
                    NF_ASSERT(0 && "Unreachable");
            }
        }
    }
}

// ------------------------------------------------
//              Neural Network Implementations 
// ------------------------------------------------

NF_NN nf_nn_alloc(Region *r, size_t *arch, size_t arch_count)
{
    NF_ASSERT(arch_count > 0);

    NF_NN nn;
    nn.arch = arch;
    nn.arch_count = arch_count;

    nn.ws = region_alloc(r, sizeof(*nn.ws)*nn.arch_count - 1);
    NF_ASSERT(nn.ws != NULL);
    nn.bs = region_alloc(r, sizeof(*nn.bs)*nn.arch_count - 1);
    NF_ASSERT(nn.bs != NULL);
    nn.as = region_alloc(r, sizeof(*nn.as)*(nn.arch_count));
    NF_ASSERT(nn.as != NULL);

    nn.as[0] = nf_mat_alloc(r, 1, arch[0]);
    for (size_t i = 1; i < arch_count;++i) {
        nn.ws[i - 1] = nf_mat_alloc(r, nn.as[i-1].cols, arch[i]);
        nn.bs[i - 1] = nf_mat_alloc(r, 1, arch[i]);
        nn.as[i]     = nf_mat_alloc(r, 1, arch[i]);
    }

    return nn;
}

void nf_nn_fill(NF_NN nn, float a)
{
    for (size_t l = 0; l < nn.arch_count - 1; ++l) {
        nf_mat_fill(nn.ws[l], a);
        nf_mat_fill(nn.bs[l], a);
        nf_mat_fill(nn.as[l], a);
    }
    nf_mat_fill(nn.as[nn.arch_count - 1], a);
}

void nf_nn_print(NF_NN nn, const char *name)
{
    char buf[256];
    printf("%s = [\n", name);

    NF_Mat *ws = nn.ws;
    NF_Mat *bs = nn.bs;

    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        snprintf(buf, sizeof(buf), "ws%zu", i);
        nf_mat_print(ws[i], buf, 3);
        snprintf(buf, sizeof(buf), "bs%zu", i);
        nf_mat_print(bs[i], buf, 3);
    }
    
    printf("]\n");
}

void nf_nn_rand(NF_NN nn, float low, float high)
{
    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        nf_mat_rand(nn.ws[i], low, high);
        nf_mat_rand(nn.bs[i], low, high);
    }
}

void nf_nn_forward(NF_NN nn)
{
    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        nf_mat_dot(nn.as[i+1], nn.as[i], nn.ws[i]);
        nf_mat_sum(nn.as[i+1], nn.bs[i]);
        nf_mat_act(nn.as[i+1]);
    }

    // apply normalization functions only on the last layer (output) of the model
#ifdef NF_NN_NORMF
        switch (NF_NN_NORMF) {
            case NF_NORMF_SOFTMAX:
                nf_softmax(nn.as[nn.arch_count-1]);
                break;
            default:
                NF_ASSERT(0 && "unreachable");
        }
#endif
}

float nf_nn_cost(NF_NN nn, NF_Mat ti, NF_Mat to)
{
    NF_ASSERT(ti.rows == to.rows);
    NF_ASSERT(to.cols == NF_NN_OUTPUT(nn).cols);
    size_t n = ti.rows;

    float c = 0;
    for (size_t i = 0; i < n; ++i) {
        NF_Mat x = nf_mat_row(ti, i);
        NF_Mat y = nf_mat_row(to, i);

        nf_mat_copy(NF_NN_INPUT(nn), x);
        nf_nn_forward(nn);
        size_t q = to.cols;
        for (size_t j = 0; j < q; ++j) {
            float d = NF_MAT_AT(NF_NN_OUTPUT(nn), 0, j) - NF_MAT_AT(y, 0, j);
            c += d*d;
        }
    }

    return c/n;
}

NF_NN nf_nn_finite_diff(Region *r, NF_NN nn, NF_Mat ti, NF_Mat to, float eps)
{
    float saved;
    float c = nf_nn_cost(nn, ti, to);
    NF_NN gn = nf_nn_alloc(r, nn.arch, nn.arch_count);

    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        for (size_t j = 0; j < nn.ws[i].rows; ++j) {
            for (size_t k = 0; k < nn.ws[i].cols; ++k) {
                saved = NF_MAT_AT(nn.ws[i], j, k);
                NF_MAT_AT(nn.ws[i], j, k) += eps;
                NF_MAT_AT(gn.ws[i], j, k) = (nf_nn_cost(nn, ti, to) - c)/eps;
                NF_MAT_AT(nn.ws[i], j, k) = saved;
            }
        }

        for (size_t j = 0; j < nn.bs[i].rows; ++j) {
            for (size_t k = 0; k < nn.bs[i].cols; ++k) {
                saved = NF_MAT_AT(nn.bs[i], j, k);
                NF_MAT_AT(nn.bs[i], j, k) += eps;
                NF_MAT_AT(gn.bs[i], j, k) = (nf_nn_cost(nn, ti, to) - c)/eps;
                NF_MAT_AT(nn.bs[i], j, k) = saved;
            }
        }
    }
    return gn;
}

NF_NN nf_nn_backprop(Region *r, NF_NN nn, NF_Mat ti, NF_Mat to)
{
    NF_ASSERT(ti.rows == to.rows);
    size_t n = ti.rows;
    NF_ASSERT(NF_NN_OUTPUT(nn).cols == to.cols);

    NF_NN gn = nf_nn_alloc(r, nn.arch, nn.arch_count);
    nf_nn_fill(gn, 0); // clear the gradient network
    
    // Feed-Forward With Back-Propagation
    // sample - i
    for (size_t i = 0; i < n; ++i) {
        // ================================================================================================
        //  Feed-Forward
        //  forward the current sample(i-th row of ti) into the neual network
        // ================================================================================================
        nf_mat_copy(NF_NN_INPUT(nn), nf_mat_row(ti, i));
        // TODO: maybe the normalization should not be applied at the end of nf_nn_forward but somewhere else?
        nf_nn_forward(nn);

        // clean up activations of the gradient network
        for (size_t l = 0; l < nn.arch_count; ++l) {
            nf_mat_fill(gn.as[l], 0);
        }
        
        // Compute the differances of the next layer and store it as the output activaion
        // (last layer activation) of the gradient neural network
        for (size_t j = 0; j < to.cols; ++j) {
        #ifdef NF_BACKPROP_TRADITIONAL
            NF_MAT_AT(NF_NN_OUTPUT(gn), 0, j) = 2.f/n*(NF_MAT_AT(NF_NN_OUTPUT(nn), 0, j) - NF_MAT_AT(to, i, j)); 
        #else 
            NF_MAT_AT(NF_NN_OUTPUT(gn), 0, j) = 1.f/n*NF_MAT_AT(NF_NN_OUTPUT(nn), 0, j) - NF_MAT_AT(to, i, j); 
        #endif //NF_BACKPROP_TRADITIONAL
        }

        #ifdef NF_BACKPROP_TRADITIONAL
                float s = 1.f;
        #else
                float s = 2.f;
        #endif //NF_BACKPROP_TRADITIONAL
        
        // ------------------------------------------------------------------------------------------------
        //  Back-Propagation
        //  layer - l
        //  Note: 
        //   - in fact we have count-1 layers bacuase the 0th layer is the input layer 
        //      -> the reason why I compute ws, bs, as of the (l-1) layer 
        //   - a0 wb0 a1 wb1 a2 wb2 ... a(n-1) wb(n-1) an 
        //   - l points to the layer after the current one
        //   - the last layer is the output layer 
        // ------------------------------------------------------------------------------------------------
        for (size_t l = nn.arch_count-1; l > 0; --l) {
            // current activation - j
            for (size_t j = 0; j < nn.as[l].cols; ++j) {
                // j-th activation of the l-th layer
                float a  = NF_MAT_AT(nn.as[l], 0, j);
                // j-th derivitive of the l-th activation                        
                float da = NF_MAT_AT(gn.as[l], 0, j);                           
                // comopute the derivitive of the activation function
                float act_deriv;
                switch (NF_NN_ACT) {
                    case NF_ACT_SIG:
                        act_deriv = a*(1-a);
                        break;
                    case NF_ACT_RELU:
                        act_deriv = a >= 0 ? 1 : 0;
                        break;
                    case NF_ACT_LRELU:
                        act_deriv = a >= 0 ? 1 : 0.01f;
                        break;
                    case NF_ACT_TANH:
                        act_deriv = 1 - a*a;
                        break;
                    case NF_ACT_SIN:
                        act_deriv = cosf(asinf(a));
                        break;
                    default:
                        NF_ASSERT(0 && "Unreachable");
                }
                // compute the partial derivitive for the j-th bias of the previoius(l-1) layer
                NF_MAT_AT(gn.bs[l-1], 0, j) += s*da*act_deriv;
                // previoius activation - k
                for (size_t k = 0; k < nn.as[l-1].cols; ++k) {
                    // k-th activation of the previoius layer (l-1)
                    float pa = NF_MAT_AT(nn.as[l-1], 0, k);                      
                    // k-th,j-th weight of the previoius layer (l-1)
                    float pw = NF_MAT_AT(nn.ws[l-1], k, j);                     
                    // compute the partial derivitive for the k,j-th weight of the previoius layer (l-1)
                    NF_MAT_AT(gn.ws[l-1], k, j) += s*da*act_deriv*pa;           
                    // compute the partial derivitive for the 
                    NF_MAT_AT(gn.as[l-1], 0, k) += s*da*act_deriv*pw;           
                }
            }
        }
    }
    
    return gn;
}

void nf_nn_learn(NF_NN nn, NF_NN gn, float rate)
{
    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        for (size_t j = 0; j < nn.ws[i].rows; ++j) {
            for (size_t k = 0; k < nn.ws[i].cols; ++k) {
                NF_MAT_AT(nn.ws[i], j, k) -= rate*NF_MAT_AT(gn.ws[i], j, k);
            }
        }

        for (size_t j = 0; j < nn.bs[i].rows; ++j) {
            for (size_t k = 0; k < nn.bs[i].cols; ++k) {
                NF_MAT_AT(nn.bs[i], j, k) -= rate*NF_MAT_AT(gn.bs[i], j, k);
            }
        }
    }
}

void nf_batch_process(Region *r, Batch *b, size_t batch_size, NF_NN nn, NF_Mat td, float rate)
{
    if (b->done) {
        b->done = false;
        b->begin = 0;
        b->cost = 0;
    }

    size_t size = batch_size;
    if (b->begin + batch_size >= td.rows) {
        size = td.rows - b->begin;
    }

    NF_Mat batch_ti = {
        .rows = size,
        .cols = NF_NN_INPUT(nn).cols,
        .stride = td.stride,
        .es = &NF_MAT_AT(td, b->begin, 0),
    };
    NF_Mat batch_to = {
        .rows = size,
        .cols = NF_NN_OUTPUT(nn).cols,
        .stride = td.stride,
        .es = &NF_MAT_AT(td, b->begin, batch_ti.cols),
    };

    NF_NN gn = nf_nn_backprop(r, nn, batch_ti, batch_to);
    nf_nn_learn(nn, gn, rate);
    b->cost += nf_nn_cost(nn, batch_ti, batch_to);
    b->begin += batch_size;

    if (b->begin >= td.rows) {
        size_t batch_count = (td.rows + batch_size - 1) / batch_size;
        b->cost /= batch_count;
        b->done = true;
    }
}

#ifdef NF_IMAGE_GENERATION
#define STR(x) #x
 
#define out_width 256
#define out_height 256
#define FPS 30
uint32_t out_pixles[out_width*out_height];

#define READ_END 0
#define WRITE_END 1

void nf_v_render_single_frame(NF_NN nn, float img_index)
{
    for (int y = 0; y < out_height; ++y) {
        for (int x = 0; x < out_width; ++x) {
            NF_MAT_AT(NF_NN_INPUT(nn), 0, 0) = (float)x/(out_width - 1);;
            NF_MAT_AT(NF_NN_INPUT(nn), 0, 1) = (float)y/(out_height - 1);
            NF_MAT_AT(NF_NN_INPUT(nn), 0, 2) = img_index;
            nf_nn_forward(nn);
            float activation = NF_MAT_AT(NF_NN_OUTPUT(nn), 0, 0);
            if (activation < 0) activation = 0;
            if (activation > 1) activation = 1;
            uint32_t bright = activation*255.f;
            uint32_t pixel = 0xFF000000|bright|(bright<<8)|(bright<<16);
            out_pixles[y*out_width + x] = pixel;
        }
    }
}

int nf_v_render_upscaled_screenshot(NF_NN nn, float img_index, const char *out_file_path)
{
    assert(out_pixles != NULL);
    nf_v_render_single_frame(nn, img_index);
    if (!stbi_write_png(out_file_path, out_width, out_height, 4, out_pixles, out_width*sizeof(*out_pixles))) {
        fprintf(stderr, "ERROR: could not write image %s\n", out_file_path);
        return 1;
    }
    printf("Generated %s\n", out_file_path);
    return 0;
}

int nf_v_render_upscaled_video(NF_NN nn, float duration, const char *out_file_path)
{
    // connecting two processes with a pipe ~ unidirectional pipe
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        fprintf(stderr, "ERROR: could not create a pipe: %s\n", strerror(errno));
        return 1;
    }
    // Fork the current process
    pid_t child = fork();
    // if child pid is negative it means that the child process was not created
    if (child < 0) {
        fprintf(stderr, "ERROR: could not fork a child: %s\n", strerror(errno));
        return 1;
    }
    // if you are the child process pid is equal to 0
    if (child == 0) {
        // replace the stdinput with the read end of the pipe
        if (dup2(pipefd[READ_END], STDIN_FILENO) < 0) {
            fprintf(stderr, "ERROR: could not reopen read end of the pipe as stdin: %s\n", strerror(errno));
            return 1;
        }
        close(pipefd[WRITE_END]);

        int ret = execlp("ffmpeg",
                         "ffmpeg",
                         "-loglevel", "verbose",
                         "-y",
                         "-f", "rawvideo",
                         "-pix_fmt", "rgb32",
                         "-s", STR(out_width) "x" STR(out_height),
                         "-r", STR(FPS),
                         "-an",
                         "-i", "-", 
                         "-c:v", "libx264",
                         out_file_path,
                         NULL
                         );
        if (ret < 0) {
            fprintf(stderr, "ERROR: could not run ffmpeg as a child process: %s\n", strerror(errno));
            return 1;
        }
        assert(0 && "unreachable");
    }

    close(pipefd[READ_END]);

    typedef struct {
        float start;
        float end;
    } Segment;
    
    Segment segments[] = {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 0},
    };

    size_t segments_count = ARRAY_LEN(segments);
    float segment_length = 1.f/segments_count;

    // render video
    size_t frame_count = FPS*duration;
    for (size_t i = 0; i < frame_count; ++i) {
        float img_index = (float)i/frame_count;
        
        // easing animation
        size_t segment_index = floorf(img_index/segment_length);
        float segment_porgress = img_index/segment_length - segment_index;
        if (segment_index > segments_count) segment_index = segments_count - 1;
        Segment segment = segments[segment_index];
        float a = segment.start + (segment.end - segment.start)*sqrtf(segment_porgress);
        nf_v_render_single_frame(nn, a);
        write(pipefd[WRITE_END], out_pixles, sizeof(*out_pixles)*out_width*out_height); 
    }

    close(pipefd[WRITE_END]);

    // wait for the child to finish executing
    wait(NULL);

    printf("Generated %s\n", out_file_path);
    return 0;
}
#endif // NF_IMAGE_GENERATION

Region region_alloc_alloc(size_t capacity)
{
    Region r = {0};
    size_t word_size = sizeof(*r.words);
    size_t words_capacity = (capacity + word_size - 1)/word_size;
    void *words = NF_MALLOC(words_capacity*word_size);
    NF_ASSERT(words != NULL);
    r.capacity_ = capacity,
    r.words = words;
    return r;
}

void *region_alloc(Region *r, size_t size)
{
    if (r == NULL) {
        return NF_MALLOC(size);
    }

    size_t word_size = sizeof(*r->words);
    size_t words_size = (size + word_size - 1)/word_size;
    NF_ASSERT(r->size_ + words_size <= r->capacity_);
    if (r->size_ + words_size > r->capacity_) {
        return NULL;
    }
    void *result = &r->words[r->size_];
    r->size_ += words_size;
    return result;
}

#endif // NF_IMPLEMENTATION
