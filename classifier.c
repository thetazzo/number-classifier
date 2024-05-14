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

// =============================================================================
// Commands to be executed on each update call
//  - An update is executed just before a render call (on_app_render)
//  - An application update is some logic/arithmetic that gets executed
//  - In our case here is where the neural network training happens
// =============================================================================
void on_app_update()
{
    assert(0 && "Not Implemented");
}

// =============================================================================
// Commands to be executed on each render call
//  - A render call is executed after an update call (on_app_update)
//  - An application render is something that produces some visual update(draw)
//  - In our case here is where the neural network is visualized
// =============================================================================
void on_app_render()
{
    assert(0 && "Not Implemented");
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Number Classifier");

    while (!WindowShouldClose()) {
        on_app_update();

        // Application rendering starts here
        BeginDrawing();
            ClearBackground(background_color());
            on_app_render();
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}

