// ================================================================================================
// TIA - Training Image Allocator
// Is a STB header only style library that allows to load a lot of image files into a single 
// array-like data structure 
// ================================================================================================

#ifndef TIA_H
#define TIA_H
#define TIA_VERSION "1.0"

#include <dirent.h>
#include "stb_image.h"

#ifndef ARRAY_LEN
#define ARRAY_LEN(xs) sizeof((xs))/sizeof((xs)[0])
#endif //ARRAY_LEN

#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif // DA_INIT_CAP

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

// ============================================================
// This macro defines the modulo of images when filling TIA
//     - example: 12 means every 12th image will be added
// ============================================================
#ifndef TIA_SKIP
#define TIA_SKIP 12
#endif // TIA_SKIP 

// TrainImage is a structure defining a single sample of a traing image
typedef struct {
    int width;
    int height;
    int comp;
    int type; // this field can be used to determine to which subset of training images the image belongs to
    uint8_t *data;
} TrainImage;

// ============================================================
// Construct a single structure of TrainImage from the image 
// file at provided path
//     - img_file_path .... {char*} path to the image 
// ============================================================
TrainImage tia_construct_sample_form_file(char *img_file_path);

typedef struct {
    size_t capacity;
    size_t count;
    TrainImage *items;
} TIA;

// ============================================================
// Loads all images inside the diirectory into TIA structure
//     - tia      .... {TIA} pointer to the TIA structure
//     - dir_path .... {char*} path to the image directory 
// ============================================================
void tia_fill_tia_from_dir(TIA *tia, char *dir_path);

#ifdef TIA_IMPLEMENTATION
TrainImage tia_construct_sample_form_file(char *img_file_path)
{
    TrainImage img = {0};
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
#ifdef TIA_DEBUG
    printf("%s size %dx%d %d bits\n", img_file_path, img_width, img_height, img_comp*8);
#endif // TIA_DEBUG
    img.width = img_width;
    img.height = img_height;
    img.comp = img_comp;
    img.data = img_data;
    return img;
}
void tia_fill_tia_from_dir(TIA *tia, char *dir_path)
{
    DIR *fs = opendir(dir_path);
    struct dirent *t = readdir(fs);
    while (t->d_name[0] == '.') {
        t = readdir(fs);
    }

    while (t != NULL && t->d_type == DT_DIR) {
        if (t->d_name[0] != '.') {
            char buf[256];
            snprintf(buf, sizeof(buf), "%s%s/", dir_path, t->d_name);
#ifdef TIA_DEBUG
            prietf("moving into `%s`\n", buf);
#endif // TIA_DEBUG
            tia_fill_tia_from_dir(tia, buf);
        }
        t = readdir(fs);
    }

    size_t counter = 0;
    while (t != NULL) {
        if (t->d_name[0] != '.') {
            if (counter % TIA_SKIP == 0) {
                char buf[256];
                snprintf(buf, sizeof(buf), "%s%s", dir_path, t->d_name);
                TrainImage train_img = tia_construct_sample_form_file(buf);
                train_img.type = dir_path[strlen(dir_path)-2] - '0';
                da_append(tia, train_img);
            }
        }
        counter += 1;
        t = readdir(fs);
    }
    closedir(fs);
}
#endif // TIA_IMPLEMENTATION

#endif // TIA_H

