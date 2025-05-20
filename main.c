
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "filesystem.h"

/* путь к контейнеру ФС */
static const char *FS_PATH = "disk.filesystem";

static void trim_line(char *s) {
    size_t n = strlen(s);
    if (n && (s[n-1] == '\n' || s[n-1] == '\r'))
        s[n-1] = '\0';
}

static void print_help() {
    puts("Commands:");
    puts("  HELP");
    puts("  LIST");
    puts("  OPEN <path>");
    puts("  ADD <path>");
    puts("  UPDATE <path>");
    puts("       (last line must be '.')");
    puts("  REMOVE <path>");
    puts("  SAVE");
    puts("  EXIT");
}

int main(void) {
    FileSystem fs = {0};

    if (fs_load(FS_PATH, &fs) != 0) {
        fprintf(stderr, "Unable to load %s\n", FS_PATH);
        return 1;
    }

    puts("BASFS loaded. Enter HELP for help.");

    char line[8192];
    int must_save = 0;
    while (1 == 1) {
        printf("\nEnter command\n> ");
        if (!fgets(line, sizeof line, stdin))
            break;
        trim_line(line);

        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        for (char *p = cmd; *p; ++p) *p = toupper(*p);

        if (strcmp(cmd, "HELP") == 0) {
            print_help();
        } else if (strcmp(cmd, "EXIT") == 0) {
            break;
        } else if (strcmp(cmd, "OPEN") == 0) {
            char *path = strtok(NULL, "");
            if (!path) { puts("Need a path"); continue; }
            char *content = NULL;
            if (fs_open(&fs, path, &content) == 0)
                printf("%s\n", content);
            else
                printf("file '%s' not found\n", path);
            free(content);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *path = strtok(NULL, "");
            if (!path) { puts("Need a path"); continue; }
            puts(fs_add(&fs, path) == 0 ? "OK" : "File already exists");
            must_save = 1;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *path = strtok(NULL, "");
            if (!path) { puts("Need a path"); continue; }
            puts(fs_remove(&fs, path) == 0 ? "Deleted" : "File not found");
            must_save = 1;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *path = strtok(NULL, "");
            if (!path) { puts("Need a path"); continue; }
            puts("Enter new content (last line must be '.'):");
            char *buf = NULL;
            size_t cap = 0, len = 0;
            while (fgets(line, sizeof line, stdin)) {
                if (strcmp(line, ".\n") == 0 || strcmp(line, ".\r\n") == 0)
                    break;
                size_t l = strlen(line);
                if (len + l + 1 > cap) {
                    cap = (cap ? cap * 2 : 4096) + l;
                    buf = realloc(buf, cap);
                }
                memcpy(buf + len, line, l);
                len += l;
            }
            if (buf) buf[len] = '\0';
            puts(fs_update(&fs, path, buf ? buf : "") == 0 ? "Updated" : "File not found");
            free(buf);
            must_save = 1;
        } else if (strcmp(cmd, "LIST") == 0) {
            fs_list(&fs);
/*
        } else if (strcmp(cmd, "SAVE") == 0) {
            fs_save(FS_PATH, &fs);
            must_save = 0;
            puts("Saved");
*/
        } else {
            puts("Unknown command");
        }
    }

    fs_save(FS_PATH, &fs);
    puts("\nBye!\n");
    return 0;
}
