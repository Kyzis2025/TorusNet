#include "torus.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Convert direction (0–7) into 3-bit ASCII */
static void direction_bits(char *out, uint8_t d) {
    out[0] = ((d >> 2) & 1) + '0';
    out[1] = ((d >> 1) & 1) + '0';
    out[2] = (d & 1) + '0';
    out[3] = '\0';
}

/* Shuffle lines */
static void shuffle_lines(char **lines, size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() % (n - i);
        char *tmp = lines[i];
        lines[i] = lines[j];
        lines[j] = tmp;
    }
}

/* Generate 32,768 case IDs + 32,768 unused IDs */
int generate_case_ids(uint16_t *case_ids, uint16_t *unused_ids, uint16_t *torus_id) {

    uint16_t used[CASE_ID_COUNT];
    uint16_t unused[CASE_ID_COUNT];

    /* Choose a number between 1 and 32768 */
    uint16_t start = (rand() % CASE_ID_COUNT) + 1;
    uint16_t val   = start;

    /* Generate 32,768 case IDs using +9 increment and wrap in [1..32768] */
    for (int i = 0; i < CASE_ID_COUNT; i++) {
        used[i] = val;

        val += 9;
        if (val > CASE_ID_COUNT) {
            val -= CASE_ID_COUNT;  /* wrap back into 1–32768 */
        }
    }

    /* Mark used IDs in full 16-bit space */
    uint8_t mark[65536] = {0};
    for (int i = 0; i < CASE_ID_COUNT; i++)
        mark[used[i]] = 1;

    /* Collect 32,768 unused IDs from full 0..65535 space */
    int idx = 0;
    for (int i = 0; i < 65536 && idx < CASE_ID_COUNT; i++) {
        if (!mark[i]) {
            unused[idx++] = (uint16_t)i;
        }
    }

    /* Copy results */
    memcpy(case_ids,   used,   CASE_ID_COUNT * sizeof(uint16_t));
    memcpy(unused_ids, unused, CASE_ID_COUNT * sizeof(uint16_t));

    /* Choose torus ID from unused IDs */
    *torus_id = unused[rand() % CASE_ID_COUNT];

    return 0;
}

/* Save unused IDs */
int save_unused_ids(const char *filename, uint16_t *unused_ids) {
    FILE *f = fopen(filename, "w");
    if (!f) return -1;

    for (int i = 0; i < CASE_ID_COUNT; i++)
        fprintf(f, "%04X\n", unused_ids[i]);

    fclose(f);
    return 0;
}

/* Generate binary file with 1,000,000 normal bits */
int generate_binary_bits(const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    size_t bytes = TORUS_CELL_COUNT / 8;
    uint8_t *buf = malloc(bytes);
    if (!buf) { fclose(f); return -2; }

    for (size_t i = 0; i < bytes; i++) {
        uint8_t b = 0;
        for (int bit = 0; bit < 8; bit++)
            b |= ((rand() % 2) << bit);
        buf[i] = b;
    }

    fwrite(buf, 1, bytes, f);
    free(buf);
    fclose(f);
    return 0;
}

/* Generate ASCII directional paths for 1,000,000 cells */
int generate_ascii_paths(const char *filename, uint16_t *case_ids, uint16_t torus_id) {

    char **lines = malloc(sizeof(char*) * TORUS_CELL_COUNT);
    if (!lines) return -1;

    /* First 32,768 cells: with case IDs */
    for (int i = 0; i < CASE_ID_COUNT; i++) {

        char buf[128];
        char bits[4];

        int pos = snprintf(buf, sizeof(buf), "%04X ", case_ids[i]);

        for (int p = 0; p < 4; p++) {
            uint8_t dir  = rand() % 8;
            uint8_t base = (rand() % 2) ? 2 : 3;
            uint8_t exp  = (rand() % 63) + 1;

            direction_bits(bits, dir);

            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%s %u^%u%s",
                            bits, base, exp,
                            (p < 3 ? ", " : "."));
        }

        lines[i] = strdup(buf);
    }

    /* Remaining cells: no case ID (0000) but same path structure */
    for (int i = CASE_ID_COUNT; i < TORUS_CELL_COUNT; i++) {

        char buf[128];
        char bits[4];

        int pos = snprintf(buf, sizeof(buf), "0000 ");

        for (int p = 0; p < 4; p++) {
            uint8_t dir  = rand() % 8;
            uint8_t base = (rand() % 2) ? 2 : 3;
            uint8_t exp  = (rand() % 63) + 1;

            direction_bits(bits, dir);

            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%s %u^%u%s",
                            bits, base, exp,
                            (p < 3 ? ", " : "."));
        }

        lines[i] = strdup(buf);
    }

    /* Shuffle ALL 1,000,000 lines together */
    shuffle_lines(lines, TORUS_CELL_COUNT);

    FILE *f = fopen(filename, "w");
    if (!f) return -2;

    /* Torus ID at top */
    fprintf(f, "TORUS_ID %04X\n\n", torus_id);

    for (int i = 0; i < TORUS_CELL_COUNT; i++) {
        fprintf(f, "%s\n", lines[i]);
        free(lines[i]);
    }

    free(lines);
    fclose(f);
    return 0;
}
