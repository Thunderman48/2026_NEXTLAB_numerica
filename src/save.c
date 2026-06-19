#include "save.h"

#include <stdio.h>

#define SAVE_FILE "save.dat"

int save_read_high_score(void) {
    FILE *f = fopen(SAVE_FILE, "r");
    if (!f) return 0;

    int score = 0;
    if (fscanf(f, "%d", &score) != 1) score = 0;
    fclose(f);
    return score;
}

void save_write_high_score(int score) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (!f) return;

    fprintf(f, "%d\n", score);
    fclose(f);
}
