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
size_t arch[] = {28*28, 16, 16, 9, 9, 2}; // this specifies the architecture of the neural network
float rate = 0.5f; // neural network learning rate

typedef struct {
    int width;
    int height;
    int comp;
    uint8_t *data;
} RawImage;

// =======================================td.rows====================================
// Add image to the training matrix
//     - td    .... {NF_Mat} matrix containing trainig data
//     - rimg  .... {RawImage} image to be added
//     - index .... {size_t} the image integer representation 
// =============================================================================
void add_tia_training_data(NF_Mat td, RawImage rimg, size_t index)
{
    size_t ind = index;
    for (int y = 0; y < rimg.height; ++y) {
        for (int x = 0; x < rimg.width; ++x) {
            size_t i = y*rimg.width + x;
            NF_MAT_AT(td, ind, i) = rimg.data[i]/255.f;
            for (size_t k = 1; k <= 2; ++k) {
                if (k == (ind+1)) {
                    NF_MAT_AT(td, ind, i+k) = 1;
                } else {
                    NF_MAT_AT(td, ind, i+k) = 0;
                }
            }
        }
    }
}

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

    if (argc == 0) {
        printf("[ERROR]: path to testing image was not provided\n");
        printf("usage: %s <img1_path> <img2_path> <test_img_path>\n", program);
        return 1;
    }

    // neural network setup
    NF_NN nn = nf_nn_alloc(NULL, arch, ARRAY_LEN(arch));
    nf_nn_rand(nn, -1, 1);

    TIA training_imgs = {0};
    tia_fill_tia_from_dir(&training_imgs, training_dir_path);

    TIA testing_imgs = {0};
    tia_fill_tia_from_dir(&testing_imgs, testing_dir_path);

//    RawImage rimg3 = load_8bit_image_as_raw(test_img_file_path);

    NF_Mat td = nf_mat_alloc(NULL, 2, NF_NN_INPUT(nn).cols + NF_NN_OUTPUT(nn).cols);

//    add_image_training_data(td, rimg1, 0);
//    add_image_training_data(td, rimg2, 1);

    InitWindow(WIDTH, HEIGHT, "Number Classifier");
    Font font = LoadFont("./assets/fonts/iosevka-term-ss02-regular.ttf");

    size_t img_width = 28;

    // preapre image 3
//    Image img3 = raw_img_to_img(rimg3);
//    Texture2D img3_texture = LoadTextureFromImage(img3);

    size_t epoch = 0;

    Batch batch = {0};
    size_t batch_size = 25;

    float cost = 0.f;
    NUI_Plot cost_plot = {0};

    char ruf[2][256]; // result string buffer
    snprintf(ruf[0], sizeof(ruf[0]), "");
    snprintf(ruf[1], sizeof(ruf[1]), "");
    while (!WindowShouldClose()) {
        // neural network training starts here
        nf_batch_process(&temp, &batch, batch_size, nn, td, rate);
        if (batch.done) {
            epoch += 1;
            da_append(&cost_plot, batch.cost);
            cost = batch.cost;
            nf_mat_shuffle_rows(td);
        }
        // testing occurs when `T` is pressed
        if (IsKeyPressed(KEY_T)) {
            assert(0 && "testing not implemented");
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
        // draw test results
        Vector2 txt1_pos = CLITERAL(Vector2){isr.x + isr.w/3, isr.y+isr.h-20*scale};
        Vector2 txt2_pos = CLITERAL(Vector2){isr.x + isr.w/3, isr.y+isr.h-15*scale};
        DrawTextEx(font, ruf[0], txt1_pos, h*0.04f, 0.25f, WHITE); 
        DrawTextEx(font, ruf[1], txt2_pos, h*0.04f, 0.25f, WHITE); 
 //       DrawTextureEx(img3_texture, CLITERAL(Vector2){isr.x+isr.w/16+img_width*scale/2, isr.y+isr.h/4+img_width*scale}, 0, scale, WHITE);
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

