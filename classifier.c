#include <assert.h>

#define NF_IMPLEMENTATION
#include "./libs/nf.h"

#define NUI_BACKGROUND {0x12, 0x12, 0x12, 0xFF}
#define NUI_IMPLEMENTATION
#include "./libs/nui.h"

// =============================================================================
// Window sizing definition
//    - Assuming a 16:9 aspect ratio
//    - FACT is a number for sizing the aspect ratio window (bigger = bigger)
// =============================================================================
#define FACT  120
#define WIDTH  16*FACT
#define HEIGHT  9*FACT

size_t arch[] = {2, 2, 1};

int main(void)
{
    // neural network setup
    NF_NN nn = nf_nn_alloc(NULL, arch, ARRAY_LEN(arch));
    nf_nn_rand(nn, -1, 1);

    InitWindow(WIDTH, HEIGHT, "Number Classifier");

    while (!WindowShouldClose()) {
        // Application rendering starts here
        BeginDrawing();
            ClearBackground(nui_background_color());
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}

