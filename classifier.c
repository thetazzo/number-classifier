#include <assert.h>

#define NF_IMPLEMENTATION
#include "./libs/nf.h"

#define NUI_BACKGROUND {0x12, 0x12, 0x12, 0xFF}
#define NUI_IMPLEMENTATION
#include "./libs/nui.h"

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
size_t arch[] = {28*28, 16, 16, 10}; // this specifies the architecture of the neural network

typedef struct {
    int width;
    int height;
    int comp;
    uint8_t *data;
} RawImage;

// =============================================================================
// Reads the image file at provided path and turns it into a RawImage structure
// =============================================================================
RawImage load_8bit_raw_image(char *img_file_path)
{
    int img_width, img_height, img_comp;
    uint8_t *img_data = (uint8_t *)stbi_load(img_file_path, &img_width, &img_height, &img_comp, 0);
    if (img_data == NULL) {
        fprintf(stderr, "ERROR: could not load image %s\n", img_file_path);
        exit(1);
    }
    if (img_comp != 1) {
        fprintf(stderr, "ERROR:  image %s is %d bits image, Only 8 bit grayscale images are supported", img_file_path, img_comp*8);
        exit(1);
    }
    RawImage rimg = {
        .width = img_width,
        .height = img_height,
        .comp = img_comp,
        .data = img_data,
    };
    return rimg;
}

// =============================================================================
// Converts the RawImage structure into Raylib Image structure
// =============================================================================
Image raw_img_to_img(RawImage rimg) 
{
    Image img = GenImageColor(rimg.width, rimg.height, BLACK);
    for (int y = 0; y < rimg.height; ++y) {
        for (int x = 0; x < rimg.width; ++x) {
            uint8_t pixel = rimg.data[y*rimg.width + x]; 
            ImageDrawPixel(
                &img,
                x,
                y,
                CLITERAL(Color){pixel, pixel, pixel, 255}
            );
        }
    }
    return img;
}

// =============================================================================
// Add image to the training matrix
//     - td    .... {NF_Mat} matrix containing trainig data
//     - rimg  .... {RawImage} image to be added
//     - index .... {size_t} the image integer representation 
// =============================================================================
void add_image_training_data(NF_Mat td, RawImage rimg, size_t index)
{
    size_t ind = index - 1;
    for (int y = 0; y < rimg.height; ++y) {
        for (int x = 0; x < rimg.width; ++x) {
            size_t i = y*rimg.width + x;
            NF_MAT_AT(td, ind, i) = rimg.data[i]/255.f;
            if (ind == 0) {
                NF_MAT_AT(td, ind, i+1)  = 1; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 1) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 1; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 2) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 1; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 3) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 1; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 4) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 1; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 5) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 1; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 6) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 1; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 7) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 1; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 8) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 1; // 8
                NF_MAT_AT(td, ind, i+10) = 0; // 9
            } else if (ind == 9) {
                NF_MAT_AT(td, ind, i+1)  = 0; // 0
                NF_MAT_AT(td, ind, i+2)  = 0; // 1
                NF_MAT_AT(td, ind, i+3)  = 0; // 2
                NF_MAT_AT(td, ind, i+4)  = 0; // 3
                NF_MAT_AT(td, ind, i+5)  = 0; // 4
                NF_MAT_AT(td, ind, i+6)  = 0; // 5
                NF_MAT_AT(td, ind, i+7)  = 0; // 6
                NF_MAT_AT(td, ind, i+8)  = 0; // 7
                NF_MAT_AT(td, ind, i+9)  = 0; // 8
                NF_MAT_AT(td, ind, i+10) = 1; // 9
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
        printf("usage: %s <img1_path> <img2_path>\n", program);
        return 1;
    }

    // Read image 1 path path
    char *img1_file_path = pop_argv(&argc, &argv);

    if (argc == 0) {
        printf("[ERROR]: path to image 2 was not provided\n");
        printf("usage: %s <img1_path> <img2_path>\n", program);
        return 1;
    }

    // Read image 2 path
    char *img2_file_path = pop_argv(&argc, &argv);

    // neural network setup
    NF_NN nn = nf_nn_alloc(NULL, arch, ARRAY_LEN(arch));
    nf_nn_rand(nn, -1, 1);

    RawImage rimg1 = load_8bit_raw_image(img1_file_path);
    RawImage rimg2 = load_8bit_raw_image(img2_file_path);

    NF_Mat td = nf_mat_alloc(NULL, 9, NF_NN_INPUT(nn).cols + NF_NN_OUTPUT(nn).cols);

    add_image_training_data(td, rimg1, 9);
    add_image_training_data(td, rimg2, 2);

    InitWindow(WIDTH, HEIGHT, "Number Classifier");

    size_t img_width = 28;

    // preapre image 1
    Image img1 = raw_img_to_img(rimg1);
    Texture2D img1_texture = LoadTextureFromImage(img1);

    // preapre image 2
    Image img2 = raw_img_to_img(rimg2);
    Texture2D img2_texture = LoadTextureFromImage(img2);

    float rate = 0.05f;

    size_t epoch = 0;

    Batch batch = {0};
    size_t batch_size = 25;

    float cost = 0.f;
    NUI_Plot cost_plot = {0};

    while (!WindowShouldClose()) {
        // neural network training starts here
        nf_batch_process(&temp, &batch, batch_size, nn, td, rate);
        if (batch.done) {
            epoch += 1;
            da_append(&cost_plot, batch.cost);
            cost = batch.cost;
            nf_mat_shuffle_rows(td);
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
        DrawTextureEx(img1_texture, CLITERAL(Vector2){isr.x+isr.w/16, isr.y+isr.h/3}, 0, scale, WHITE);
        DrawTextureEx(img2_texture, CLITERAL(Vector2){isr.x+isr.w/16+img_width*scale, isr.y+isr.h/3}, 0, scale, WHITE);
        nui_plot(cost_plot, nui_layout_slot());
        nui_render_nn(nn, nui_layout_slot());
        nui_layout_end();
        EndDrawing();
        region_reset(&temp);
    }

    CloseWindow();

    return 0;
}

