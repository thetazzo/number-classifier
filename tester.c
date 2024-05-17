#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

#define SILENT_ 

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

int is_comp_file(char *fn)
{
    for (size_t i = 0; i < strlen(fn); ++i) {
        char c = fn[i];
        if (c == '.') {
            if (fn[i+1] == 'c') {
                return 1;
            }
        }
    }
    return 0;
}

void clean_str_buf(char *b)
{
    for (size_t i = 0; i < strlen(b); ++i) {
        b[i] = 0;
    }
}

char *str_reduce(char delim, char *str)
{
    char *buf = "";
    size_t i = 0;
    while (str[i] != delim) {
        asprintf(&buf, "%s%c", buf, str[i]);
        i += 1;
    }
    return buf;
}

typedef enum {
    TEST_ENTRY,
    CMP_FAIL,
    EXE_FAIL,
} SampleType;

char *sample_type_name(SampleType t)
{
    switch (t) {
        case TEST_ENTRY:
            return "test";
        case CMP_FAIL:
            return "compilation error";
        case EXE_FAIL:
            return "execution error";
        default:
            assert(0 && "unreachable");
    }
    return "";
}

typedef struct {
    SampleType type;
    char *cmp_path;
    char *exe_path;
} Sample;

void sample_dump(Sample s)
{
    printf("cmp_path: %s, exe_path: %s, type: %s\n", s.cmp_path, s.exe_path, sample_type_name(s.type)); 
}

typedef struct {
    size_t count;
    size_t capacity;
    Sample *items;
} SDA;

void tests_import(char *dir_path, SDA *tests)
{
    struct dirent *de;

    DIR *fd = opendir(dir_path);

    if (fd == NULL) {
        fprintf(stderr, "Directory `%s` could not be opened", dir_path);
        exit(1);
    }

    char cmp_path[256][256];
    char exe_path[256][256];
    size_t tsti = 0;
    while ((de = readdir(fd)) != NULL) {
        char *file_name = de->d_name;
        if (strcmp(file_name, ".") && strcmp(file_name, "..")) {
            if (is_comp_file(file_name)) {
                char *prog_name = str_reduce('.', file_name);
                strcpy(cmp_path[tsti], dir_path);
                strcat(cmp_path[tsti], prog_name);
                strcat(cmp_path[tsti], ".c");
                strcpy(exe_path[tsti], dir_path);
                strcat(exe_path[tsti], prog_name);
                Sample s = {
                    .type = TEST_ENTRY,
                    .cmp_path = cmp_path[tsti],
                    .exe_path = exe_path[tsti],
                };
                da_append(tests, s);
                tsti++;
#ifndef SILENT_
                printf("imported test `%s`\n", file_name);
#endif
            } else {
                // remove all existing test compilations
                char delete_fp[256];
                snprintf(delete_fp, sizeof(delete_fp), "./tests/%s", file_name);
#ifndef SILENT_
                printf("deleting `%s`\n", delete_fp);
#endif
                remove(delete_fp);
            }
        }
    }

    closedir(fd);
}

void tests_run(SDA tests)
{
    size_t cmp_fail_count = 0;
    size_t exe_fail_count = 0;

    // assume that all samples here are of type tests
    for (size_t k = 0; k < tests.count; ++k) {
        Sample test = tests.items[k];
        // compile test
        char cmp_cmd[256];
        snprintf(
            cmp_cmd,
            sizeof(cmp_cmd),
            "clang -Wall -Wextra -o %s %s -lm",
            test.exe_path, test.cmp_path
        ); 

        printf("[CMP] %s\n", test.cmp_path);

        FILE *cmp_fd = popen(cmp_cmd, "r");
        int cmp_status = pclose(cmp_fd);
        int cmp_exitcode = cmp_status/256;
        if (cmp_exitcode != 0) {
            tests.items[k].type = CMP_FAIL;
            cmp_fail_count += 1;
        } else {
            // execute test
            char exe_cmd[256];
            snprintf(
                exe_cmd,
                sizeof(exe_cmd),
                "%s",
                test.exe_path
            ); 
            printf("[EXE] %s\n", test.exe_path);
            FILE *exe_fd = popen(exe_cmd, "r");
            char tout[256];
            fread(tout, sizeof(tout), ARRAY_LEN(tout), exe_fd);
#ifndef SILENT_
            printf("[stdout] %s\n", tout);
#endif
            int exe_status = pclose(exe_fd);
            int exe_exitcode = exe_status/256;
            if (exe_exitcode != 0) {
                tests.items[k].type = EXE_FAIL;
                exe_fail_count += 1;
            }
        }
    }

    printf("\n");
    printf("Executed %zu tests\n", tests.count);
    printf("Compilation failed: %zu, Execution failed: %zu\n", cmp_fail_count, exe_fail_count);

    printf("--------------------------------------------------\n");

    for (size_t k = 0; k < tests.count; ++k) {
        Sample test = tests.items[k];
        if (test.type == CMP_FAIL) {
            printf("[CMP FAIL] %s\n", test.cmp_path);
        } else if (test.type == EXE_FAIL) {
            printf("[EXE FAIL] %s\n", test.cmp_path);
        }
    }
}

int main(void)
{

    SDA tests = {0};
    tests_import("./tests/", &tests);
    tests_run(tests);

    return 0;
}
