#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char *data;     // pointer to buffer
    int len;        // current length
    int cap;        // allocated capacity
} StringBuffer;

void sb_init(StringBuffer *sb, int initial_cap) {
    sb->data = malloc(initial_cap);
    sb->len = 0;
    sb->cap = initial_cap;
}

void sb_free(StringBuffer *sb) {
    free(sb->data);
    sb->data = NULL;
    sb->len = 0;
    sb->cap = 0;
}

void sb_append_string(StringBuffer *sb, const char *str) {
    int str_len = strlen(str);
    if (sb->len + str_len >= sb->cap) {
        int new_cap = sb->cap * 2;
        while (new_cap <= sb->len + str_len) {
            new_cap *= 2;
        }
        char *tmp = realloc(sb->data, new_cap);
        if (!tmp) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        sb->data = tmp;
        sb->cap = new_cap;
    }

    memcpy(sb->data + sb->len, str, str_len);
    sb->len += str_len;
}

void sb_append_char(StringBuffer *sb, char c) {
    if (sb->len + 1 >= sb->cap) {
        sb->cap *= 2;
        sb->data = realloc(sb->data, sb->cap);
    }
    sb->data[sb->len++] = c;
}

void sb_append_int(StringBuffer *sb, int value) {
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%d", value);
    if (len > 0) {
        sb_append_string(sb, buf);
    }
}

#endif
