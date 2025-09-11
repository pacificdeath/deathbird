#ifndef ATLAS_GENERATOR_H
#define ATLAS_GENERATOR_H

#include "string_buffer.h"
#include "../raylib/include/raylib.h"

const static int padding = 2;

void sanitize_name(const char *input, char *output, int max_len) {
    int last_dot = -1;
    int i = 0;

    while (input[i] != '\0') {
        if (input[i] == '.') last_dot = i;
        i++;
    }

    int copy_len = (last_dot == -1) ? i : last_dot;
    if (copy_len > max_len - 1) copy_len = max_len - 1;

    for (int j = 0; j < copy_len; j++) {
        char c = input[j];
        if (c >= 'a' && c <= 'z') c = c - ('a' - 'A');
        else if (c == '-') c = '_';
        output[j] = c;
    }

    output[copy_len] = '\0';
}

int generate_atlas() {
    FilePathList files = LoadDirectoryFiles(TEXTURE_PATH);

    const int atlas_width = 2048;
    const int atlas_height = 2048;
    Image atlas = GenImageColor(atlas_width, atlas_height, BLANK);

    int x = 0;
    int y = 0;
    int largest_y_in_row = 0;

    StringBuffer enum_sb;
    sb_init(&enum_sb, 64);
    sb_append_string(&enum_sb,
        "#ifndef ATLAS_DATA_GENERATED_H\n"
        "#define ATLAS_DATA_GENERATED_H\n"
        "#include \"../raylib/include/raylib.h\"\n"
        "typedef enum Sprite {\n"
    );

    StringBuffer map_sb;
    sb_init(&map_sb, 64);
    sb_append_string(&map_sb, "static const Rectangle sprite_rectangles[SPRITE_COUNT] = {\n");

    for (int i = 0; i < files.count; i++) {
        x += padding;

        Image image = LoadImage(files.paths[i]);

        if (image.width > (atlas.width - (2 * padding)) || image.height > (atlas.height - (2 * padding))) {
            TraceLog(LOG_ERROR, "Image \"%s\" is too big", files.paths[i]);
            return 1;
        }

        if (x + image.width + padding > atlas_width - padding) {
            x = padding;
            y += largest_y_in_row + padding;
            largest_y_in_row = 0;
        }

        if (y + image.height > atlas.height - padding) {
            TraceLog(LOG_ERROR, "Atlas height exceeded");
            return 1;
        }

        if (image.height > largest_y_in_row) {
            largest_y_in_row = image.height;
        }

        Rectangle src = { 0, 0, image.width, image.height };
        Rectangle dst = { x, y, image.width, image.height };
        ImageDraw(&atlas, image, src, dst, (Color){0xff,0xff,0xff,0xff});

        int split_count;
        const char **split = TextSplit(files.paths[i], '\\', &split_count);

        char image_name[64];
        sanitize_name(split[split_count - 1], image_name, 64);

        sb_append_string(&enum_sb, "SPRITE_");
        sb_append_string(&enum_sb, image_name);
        sb_append_char(&enum_sb, ',');
        sb_append_char(&enum_sb, '\n');

        sb_append_string(&map_sb, "[SPRITE_");
        sb_append_string(&map_sb, image_name);
        sb_append_string(&map_sb, "]={");
        sb_append_int(&map_sb, x);
        sb_append_char(&map_sb, ',');
        sb_append_int(&map_sb, y);
        sb_append_char(&map_sb, ',');
        sb_append_int(&map_sb, image.width);
        sb_append_char(&map_sb, ',');
        sb_append_int(&map_sb, image.height);
        sb_append_string(&map_sb, "},\n");

        UnloadImage(image);
        x += image.width;
    }

    sb_append_string(&enum_sb, "SPRITE_COUNT\n} Sprite;\n");
    sb_append_char(&enum_sb, '\0');

    sb_append_string(&map_sb, "};\n");
    sb_append_char(&map_sb, '\0');

    StringBuffer atlas_sb;
    sb_init(&atlas_sb, enum_sb.len + map_sb.len + 64);
    sb_append_string(&atlas_sb, enum_sb.data);
    sb_append_string(&atlas_sb, map_sb.data);
    sb_append_string(&atlas_sb, "#endif");
    sb_append_char(&atlas_sb, '\0');

    SaveFileText(ATLAS_DATA_GENERATED_H_PATH, atlas_sb.data);

    ExportImage(atlas, ATLAS_PNG_PATH);

    sb_free(&enum_sb);
    sb_free(&map_sb);
    sb_free(&atlas_sb);

    return 0;
}

#endif
