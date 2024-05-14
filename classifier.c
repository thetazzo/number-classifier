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
size_t arch[] = {2, 2, 1}; // this specifies the architecture of the neural network

Image load_8bit_image(char *img_file_path)
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

    Image img = GenImageColor(img_width, img_height, BLACK);
    for (int y = 0; y < img_height; ++y) {
        for (int x = 0; x < img_width; ++x) {
            uint8_t pixel = img_data[y*img_width + x]; 
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

int main(int argc, char **argv)
{
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

    InitWindow(WIDTH, HEIGHT, "Number Classifier");

    size_t img_width = 28;

    // preapre image 1
    Image img1 = load_8bit_image(img1_file_path);
    Texture2D img1_texture = LoadTextureFromImage(img1);

    // preapre image 2
    Image img2 = load_8bit_image(img2_file_path);
    Texture2D img2_texture = LoadTextureFromImage(img2);

    while (!WindowShouldClose()) {
        // Application rendering starts here
        size_t w = GetScreenWidth();
        size_t h = GetScreenHeight();
        float scale = h*0.009f;
        NUI_Rect root = {0, 0, w, h};
        BeginDrawing();
        ClearBackground(nui_background_color());
        nui_layout_begin(NLO_HORZ, root, 2, 0);
        NUI_Rect isr = nui_layout_slot();
        DrawTextureEx(img1_texture, CLITERAL(Vector2){isr.x+isr.w/16, isr.y+isr.h/3}, 0, scale, WHITE);
        DrawTextureEx(img2_texture, CLITERAL(Vector2){isr.x+isr.w/16+img_width*scale, isr.y+isr.h/3}, 0, scale, WHITE);
        nui_render_nn(nn, nui_layout_slot());
        nui_layout_end();
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}

