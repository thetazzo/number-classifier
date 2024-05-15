#ifndef NF_H_
#error "Neural Framework (NF) library is missing" 
#endif

#ifndef NUI_H_
#define NUI_H_

#include <math.h>
#include <errno.h>
// posix specific headers :: allows forking childs in linux
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <float.h>

#include "raylib.h"
#include "raymath.h"

// ================================================================= 
// Macro specifying the inital dynamic array capacity
// ================================================================= 
#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif // DA_INIT_CAP

// ================================================================= 
// Macro used for appending items(data) into a dynamic array
// ================================================================= 
#ifndef da_append
#define da_append(da, item)                                                                 \
    do {                                                                                    \
        if ((da)->count >= (da)->capacity) {                                                \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;          \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));        \
            assert((da)->items != NULL && "Buy more RAM");                                  \
        }                                                                                   \
        (da)->items[(da)->count++] = (item);                                                \
    } while (0)                                                                           
#endif // da_append

// ================================================================= 
// Macro defining the background color that used in the application
//     - { R, G, B, A} 
//     - hexadecimal values of each color component
//     - the last component (A) is the brightness of the color
// ================================================================= 
#ifndef NUI_BACKGROUND
#define NUI_BACKGROUND {0x00, 0xAA, 0x55, 0xFF}
#endif

// ================================================================= 
// Raylib Color interpreation of the NUI_BACKGROUND 
// ================================================================= 
Color nui_background_color();

typedef struct {
    float x; // position on the x axis
    float y; // position on the y axis
    float w; // width
    float h; // height
} NUI_Rect;

typedef enum {
    NLO_HORZ, // horizontal orientation
    NLO_VERT, // vertical orientation
} NUI_Layout_Orient;

typedef struct {
    NUI_Rect rect;
    NUI_Layout_Orient orient;
    size_t count;
    size_t i;
    float gap;
} NUI_Layout;

NUI_Rect nui_layout_slot_loc(NUI_Layout *l, const char *file_path, int line);

typedef struct {
    NUI_Layout *items;
    size_t count;
    size_t capacity;
} NUI_Layout_Stack;

void nui_layout_stack_push(NUI_Layout_Stack *ls, NUI_Layout_Orient orient, NUI_Rect rect, size_t count, float gap);
#define nui_layout_stack_slot(ls) (assert((ls)->count > 0), nui_layout_slot_loc(&(ls)->items[(ls)->count - 1], __FILE__, __LINE__))
#define nui_layout_stack_pop(ls) do { assert((ls)->count > 0); (ls)->count -= 1; } while (0)

static NUI_Layout_Stack default_nui_layout_stack = {0};

// ================================================================= 
// Macro used for marking the start of a new render layout
//     - orient .... {NUI_Layout_Orient} orientation of the layout 
//     - rect   .... {NUI_RECT} the rectangle on which the layout is applied 
//     - count  .... {size_t} amount of slots inside the layout 
//     - gap    .... {float} amount of space between layout slots
// ================================================================= 
#define nui_layout_begin(orient, rect, count, gap) nui_layout_stack_push(&default_nui_layout_stack, (orient), (rect), (count), (gap))
// ================================================================= 
// Macro used for marking the end of a render layout
// ================================================================= 
#define nui_layout_end() nui_layout_stack_pop(&default_nui_layout_stack)
// ================================================================= 
// Macro used for retrieving the next available slot whitin the current layout 
// ================================================================= 
#define nui_layout_slot() nui_layout_stack_slot(&default_nui_layout_stack)

typedef struct {
    float *items;
    size_t count;
    size_t capacity;
} NUI_Plot;

// ================================================================= 
// Creates a visualisation of the provided neural network
//     - nn .... {NF_NN} the neural network to be visualized
//     - r  .... {NUI_Rect} the rectangle where the visualisation is rendered
// ================================================================= 
void nui_render_nn(NF_NN nn, NUI_Rect r);
void nui_plot(NUI_Plot plot, NUI_Rect);
void nui_slider(float *value, bool *is_dragging, float rx, float ry, float rw, float rh);

void nui_render_mat_as_heatmap(NF_Mat m, NUI_Rect r,  size_t max_width);
void nui_render_nn_weights_heatmap(NF_NN nn, NUI_Rect r);
void nui_render_nn_activations_heatmap(NF_NN nn, NUI_Rect r);

#ifdef NUI_IMPLEMENTATION

Color nui_background_color()
{
    return CLITERAL(Color)NUI_BACKGROUND;
}

NUI_Rect nui_layout_slot_loc(NUI_Layout *l, const char *file_path, int line)
{
    if (l->i >= l->count) {
        fprintf(stderr, "%s:%d: ERROR: Layout overflow\n", file_path, line);
        exit(1);
    }

    NUI_Rect r = {0};

    switch (l->orient) {
    case NLO_HORZ:
        r.w = l->rect.w/l->count;
        r.h = l->rect.h;
        r.x = l->rect.x + l->i*r.w;
        r.y = l->rect.y;

        if (l->i == 0) { // First
            r.w -= l->gap/2;
        } else if (l->i >= l->count - 1) { // Last
            r.x += l->gap/2;
            r.w -= l->gap/2;
        } else { // Middle
            r.x += l->gap/2;
            r.w -= l->gap;
        }

        break;

    case NLO_VERT:
        r.w = l->rect.w;
        r.h = l->rect.h/l->count;
        r.x = l->rect.x;
        r.y = l->rect.y + l->i*r.h;

        if (l->i == 0) { // First
            r.h -= l->gap/2;
        } else if (l->i >= l->count - 1) { // Last
            r.y += l->gap/2;
            r.h -= l->gap/2;
        } else { // Middle
            r.y += l->gap/2;
            r.h -= l->gap;
        }

        break;

    default:
        assert(0 && "Unreachable");
    }

    l->i += 1;

    return r;
}

void nui_layout_stack_push(NUI_Layout_Stack *ls, NUI_Layout_Orient orient, NUI_Rect rect, size_t count, float gap)
{
    NUI_Layout l = {0};
    l.orient = orient;
    l.rect = rect;
    l.count = count;
    l.gap = gap;
    da_append(ls, l);
}

void nui_render_nn(NF_NN nn, NUI_Rect r) {
    Color low_color  = { 0x00, 0x00, 0xFF, 0xFF };
    Color high_color = { 0xFF, 0x00, 0x00, 0xFF };

    float neuron_rad = r.h*0.02;
    float layer_border_hpad = 50;
    float layer_border_vpad = 50;

    float nn_width   = r.w - 2*layer_border_hpad;
    float nn_height  = r.h - 2*layer_border_vpad;
    float nn_x       = r.x + r.w/2 - nn_width/2;
    float nn_y       = r.y + r.h/2 - nn_height/2;

    int layer_hpad = nn_width / nn.arch_count;
    for (size_t l = 0; l < nn.arch_count; ++l) {
        int layer_vpad1 = nn_height/nn.as[l].cols;
        for (size_t i = 0; i < nn.as[l].cols; ++i) {
            float cx1 = nn_x + l*layer_hpad + layer_hpad/2; 
            float cy1 = nn_y + i*layer_vpad1 + layer_vpad1/2;
            if (l+1 < nn.arch_count) {
                float layer_vpad2 = nn_height/nn.as[l+1].cols;
                for (size_t j = 0; j < nn.as[l+1].cols; ++j) {
                    float cx2 = nn_x + (l+1)*layer_hpad + layer_hpad/2; 
                    float cy2 = nn_y + j*layer_vpad2 + layer_vpad2/2;
                    float value = nf_sigmoidf(NF_MAT_AT(nn.ws[l], j, i));
                    high_color.a = floorf(255.f*value);
                    float thicc = r.h*0.004f;

                    Vector2 start = {cx1, cy1};
                    Vector2 end = {cx2, cy2};

                    DrawLineEx(
                        start,
                        end,
                        thicc,
                        ColorAlphaBlend(low_color, high_color, WHITE)
                    );
                }
            }
            if (l > 0) {
                high_color.a = floorf(255.f*nf_sigmoidf(NF_MAT_AT(nn.bs[l-1], 0, i)));
                DrawCircle(cx1, cy1, neuron_rad, ColorAlphaBlend(low_color, high_color, WHITE));
            } else {
                DrawCircle(cx1, cy1, neuron_rad, GRAY);
            }
        }
    }
}

void nui_plot(NUI_Plot plot, NUI_Rect r) 
{
    float min = FLT_MAX;
    float max = FLT_MIN;
    for (size_t i = 0; i < plot.count; ++i) {
        if (max < plot.items[i]) { max = plot.items[i]; }
        if (min > plot.items[i]) { min = plot.items[i]; }
    }
    if (min > 0) min = 0;

    size_t n = plot.count;

    if (n < 100) n = 100;

    for (size_t i = 0; i+1 < plot.count; ++i) {
        float x1 = r.x + (float)r.w/n * i; 
        float y1 = r.y + (1-(plot.items[i] - min)/(max-min))*r.h;
        float x2 = r.x + (float)r.w/n * (i+1); 
        float y2 = r.y + (1-(plot.items[i+1] - min)/(max-min))*r.h;

        DrawLineEx((Vector2){x1,y1}, (Vector2){x2,y2}, r.h*0.0035f, YELLOW);
        DrawLine(r.x-50, r.y+r.h, r.x+r.w+60, r.y+r.h, RAYWHITE);
        DrawLine(r.x, r.y+r.h+50, r.x, y1, RAYWHITE);
        DrawCircle(r.x, r.y+r.h, r.h*0.008f, RAYWHITE);
        DrawText("0", r.x-r.h*0.03f, r.y+r.h+2, r.h*0.03f, RAYWHITE);
    }
}

void nui_slider(float *value, bool *is_dragging, float rx, float ry, float rw, float rh)
{
    float knob_radius = rh;
    Vector2 bar_size = {
        .x = rw - 2*knob_radius,
        .y = rh*0.25,
    };
    Vector2 bar_position = {
        .x = rx + knob_radius,
        .y = ry + rh/2 - bar_size.y/2
    };
    DrawRectangleV(bar_position, bar_size, WHITE);

    Vector2 knob_position = {
        .x = bar_position.x + bar_size.x*(*value),
        .y = ry + rh/2
    };
    DrawCircleV(knob_position, knob_radius, RED);

    if (*is_dragging) {
        float x = GetMousePosition().x;
        if (x < bar_position.x) x = bar_position.x;
        if (x > bar_position.x + bar_size.x) x = bar_position.x + bar_size.x;
        *value = (x - bar_position.x)/bar_size.x;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_position = GetMousePosition();
        if (Vector2Distance(mouse_position, knob_position) <= knob_radius) {
            *is_dragging = true;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        *is_dragging = false;
    }
}

void nui_render_mat_as_heatmap(NF_Mat m, NUI_Rect r, size_t max_width)
{
    Color low_color  = { 0x00, 0x00, 0xFF, 0xFF };
    Color high_color = { 0xFF, 0x00, 0x00, 0xFF };

    float cell_width  = r.w*m.cols/max_width/m.cols;
    float cell_height = r.h/m.rows;

    float full_width  = r.w*m.cols/max_width;

    for (size_t y = 0; y < m.rows; ++y) {
        for (size_t x = 0; x < m.cols; ++x) {
            float alpha = nf_sigmoidf(NF_MAT_AT(m, y, x));
            high_color.a = floorf(255.f*alpha);
            Color clr = ColorAlphaBlend(low_color, high_color, WHITE);
            NUI_Rect slot = {
                r.x + r.w/2 - full_width/2 + x*cell_width,
                r.y + y*cell_height,
                cell_width,
                cell_height,
            };
            DrawRectangle(ceilf(slot.x), ceilf(slot.y), ceilf(slot.w), ceilf(slot.h), clr);
        }
    }
}

void nui_render_nn_weights_heatmap(NF_NN nn, NUI_Rect r)
{
    size_t max_width = 0;
    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        if (max_width < nn.ws[i].cols) {
            max_width = nn.ws[i].cols;
        }
    }
    nui_layout_begin(NLO_VERT, r, nn.arch_count-1, 10);
    for (size_t i = 0; i < nn.arch_count-1; ++i) {
        nui_render_mat_as_heatmap(nn.ws[i], nui_layout_slot(), max_width);
    }
    nui_layout_end();
}

void nui_render_nn_activations_heatmap(NF_NN nn, NUI_Rect r)
{
    size_t max_width = 0;
    for (size_t i = 0; i < nn.arch_count; ++i) {
        if (max_width < nn.as[i].cols) {
            max_width = nn.as[i].cols;
        }
    }
    nui_layout_begin(NLO_VERT, r, nn.arch_count, 10);
    for (size_t i = 0; i < nn.arch_count; ++i) {
        nui_render_mat_as_heatmap(nn.as[i], nui_layout_slot(), max_width);
    }
    nui_layout_end();
}

#endif // NUI_IMPLEMENTATION
#endif // NUI_H_
