#include <assert.h>

#define NF_IMPLEMENTATION
#include "./libs/nf.h"

#define NUI_BACKGROUND {0x12, 0x12, 0x12, 0xFF}
#define NUI_IMPLEMENTATION
#include "./libs/nui.h"

#define TIA_IMPLEMENTATION
#include "./libs/tia.h"

// =============================================================================
// Pop and return the top argv
// =============================================================================
char *pop_argv(int *argc, char ***argv)
{
    char *result = **argv;
    (*argc) -= 1;
    (*argv) += 1;
    return result;
}

// =============================================================================
// Window sizing definition
//    - Assuming a 16:9 aspect ratio
//    - FACT is a number for sizing the aspect ratio window (bigger = bigger)
// =============================================================================
#define FACT  120
#define WIDTH  16*FACT
#define HEIGHT  9*FACT

// =============================================================================
// Neural network constants
// =============================================================================
size_t arch[] = {28*28, 16, 16, 9, 9, 10}; // this specifies the architecture of the neural network
float rate = 0.02f; // neural network learning rate

int main(int argc, char **argv)
{
    Region temp = region_alloc_alloc(1024*1014*256);
    char *program = pop_argv(&argc, &argv);

    if (argc == 0) {
        printf("[ERROR]: path to image 1 was not provided\n");
        printf("usage: %s <img1_path> <img2_path> <test_img_path>\n", program);
        return 1;
    }

    // Read trining dir path
    char *training_dir_path = pop_argv(&argc, &argv);

    if (argc == 0) {
        printf("[ERROR]: path to image 2 was not provided\n");
        printf("usage: %s <img1_path> <img2_path> <test_img_path>\n", program);
        return 1;
    }

    // Read testing dir path
    char *testing_dir_path = pop_argv(&argc, &argv);

    // neural network setup
    NF_NN nn = nf_nn_alloc(NULL, arch, ARRAY_LEN(arch));
    nf_nn_rand(nn, -1, 1);

    TIA training_imgs = {0};
    tia_fill_tia_from_dir(&training_imgs, training_dir_path);

    TIA testing_imgs = {0};
    tia_fill_tia_from_dir(&testing_imgs, testing_dir_path);

    NF_Mat td = nf_mat_alloc(
        NULL,
        training_imgs.count,
        NF_NN_INPUT(nn).cols + NF_NN_OUTPUT(nn).cols
    );

    // Add training images to training data matrix
    for (size_t z = 0; z < training_imgs.count; ++z) {
        TrainImage ti = training_imgs.items[z];
        for (int y = 0; y < ti.height; y++) {
            for (int x = 0; x < ti.width; x++) {
                int i = y*ti.width + x;
                NF_MAT_AT(td, z, i) = ti.data[i]/255.f;
                for (int k = 1; k <= 10; ++k) {
                    if ((k-1) == ti.type) {
                        NF_MAT_AT(td, z, i+k) = 1;
                    } else {
                        NF_MAT_AT(td, z, i+k) = 0;
                    }
                }
            }
        }
    }

    // raylib window setup
    InitWindow(WIDTH, HEIGHT, "Number Classifier");
    Font font = LoadFont("./assets/fonts/iosevka-term-ss02-regular.ttf");

    size_t img_width = 28;

    size_t epoch = 0;
    size_t max_epoch = 200;
    Batch batch = {0};
    size_t batch_size = 25;
    size_t batches_per_frame = 20;

    NUI_Plot cost_plot = {0};

    bool isRunning = false;

    char info_sb[256]; // display epoch, activation, rate, cost, etc.
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            isRunning = !isRunning;
        }
        // neural network training starts here
        for (size_t k = 0; k < batches_per_frame && epoch < max_epoch && isRunning; ++k) {
            nf_batch_process(&temp, &batch, batch_size, nn, td, rate);
            if (batch.done) {
                epoch += 1;
                da_append(&cost_plot, batch.cost);
                nf_mat_shuffle_rows(td);
            }
        }
        // testing occurs when `T` is pressed
        if (IsKeyPressed(KEY_T)) {
            size_t vc = 0;
            size_t nvc = 0;
            for (size_t z = 0; z < testing_imgs.count; ++z) {
                TrainImage ti = testing_imgs.items[z];
                for (int y = 0; y < ti.height; y++) {
                    for (int x = 0; x < ti.width; x++) {
                        int i = y*ti.width + x;
                        NF_MAT_AT(NF_NN_INPUT(nn), 0, i) = ti.data[i]/255.f;
                    }
                }
                nf_nn_forward(nn);
                float expected_ = NF_MAT_AT(NF_NN_OUTPUT(nn), 0, ti.type);
                bool valid = true;
                for (int q = 0; q < 10; ++q) {
                    float o = NF_MAT_AT(NF_NN_OUTPUT(nn), 0, q);
                    if (q != ti.type && o >= expected_) {
                        valid = false;
                    }
                }
                if (valid) {
                    vc += 1;
                } else {
                    nvc += 1;
                }
            }
            printf("No. valid is %zu\n", vc);
            printf("No. invalid is %zu\n", nvc);
        }
        // Application rendering starts here
        size_t w = GetScreenWidth();
        size_t h = GetScreenHeight();
        float scale = h*0.009f;
        NUI_Rect root = {0, 0, w, h};
        BeginDrawing();
        ClearBackground(nui_background_color());
        nui_layout_begin(NLO_HORZ, root, 3, 0);
        NUI_Rect isr = nui_layout_slot();
        // draw info about the neural network
        snprintf(
            info_sb,
            sizeof(info_sb),
            "%s",
            isRunning ? "running" : "paused",
            activation_as_str(),
            rate,
            cost_plot.count > 0 ? cost_plot.items[cost_plot.count-1] : 0,
            training_imgs.count
        );
        DrawTextEx(font, info_sb, CLITERAL(Vector2){100, 50 + 0*h*0.04f}, h*0.04f, 0.25f, WHITE); 
        snprintf(
            info_sb,
            sizeof(info_sb),
            "activation: %s",
            activation_as_str()
        );
        DrawTextEx(font, info_sb, CLITERAL(Vector2){100, 50 + 1*h*0.04f}, h*0.04f, 0.25f, WHITE); 
        snprintf(
            info_sb,
            sizeof(info_sb),
            "rate: %f",
            rate
        );
        DrawTextEx(font, info_sb, CLITERAL(Vector2){100, 50 + 2*h*0.04f}, h*0.04f, 0.25f, WHITE); 
        snprintf(
            info_sb,
            sizeof(info_sb),
            "cost: %f",
            cost_plot.count > 0 ? cost_plot.items[cost_plot.count-1] : 0
        );
        DrawTextEx(font, info_sb, CLITERAL(Vector2){100, 50 + 3*h*0.04f}, h*0.04f, 0.25f, WHITE); 
        snprintf(
            info_sb,
            sizeof(info_sb),
            "training on %zu images",
            training_imgs.count
        );
        DrawTextEx(font, info_sb, CLITERAL(Vector2){100, 50 + 4*h*0.04f}, h*0.04f, 0.25f, WHITE); 
        // draw cost plot
        nui_plot(cost_plot, nui_layout_slot());
        // draw neural network
        nui_render_nn(nn, nui_layout_slot());
        nui_layout_end();
        EndDrawing();
        region_reset(&temp);
    }

    CloseWindow();

    return 0;
}

