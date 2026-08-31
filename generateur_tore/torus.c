#include "torus.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Convertit une direction (0–7) en 3 bits ASCII */
static void print_direction_bits(char *out, uint8_t dir) {
    out[0] = ((dir >> 2) & 1) + '0';
    out[1] = ((dir >> 1) & 1) + '0';
    out[2] = (dir & 1) + '0';
    out[3] = '\0';
}

/* Génère le fichier binaire des bits normaux */
int generate_binary_bits(const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    const size_t byte_count = TORUS_CELL_COUNT / 8;
    uint8_t *buffer = malloc(byte_count);
    if (!buffer) {
        fclose(f);
        return -2;
    }

    srand((unsigned int)time(NULL));

    for (size_t i = 0; i < byte_count; i++) {
        uint8_t b = 0;
        for (int bit = 0; bit < 8; bit++) {
            b |= ((rand() % 2) << bit);
        }
        buffer[i] = b;
    }

    fwrite(buffer, 1, byte_count, f);
    free(buffer);
    fclose(f);
    return 0;
}

/* Mélange aléatoirement un tableau de chaînes */
static void shuffle_lines(char **lines, size_t count) {
    for (size_t i = 0; i < count - 1; i++) {
        size_t j = i + rand() % (count - i);
        char *tmp = lines[i];
        lines[i] = lines[j];
        lines[j] = tmp;
    }
}

/* Génère le fichier ASCII des chemins + mélange des lignes */
int generate_ascii_paths(const char *filename, uint16_t torus_id) {

    char **lines = malloc(sizeof(char*) * TORUS_CELL_COUNT);
    if (!lines) return -1;

    srand((unsigned int)time(NULL) ^ 0xBEEF);

    for (uint32_t i = 0; i < TORUS_CELL_COUNT; i++) {

        char buffer[128];
        char dirbits[4];

        uint16_t case_id = (uint16_t)(i & 0xFFFF);

        int pos = snprintf(buffer, sizeof(buffer), "%04X ", case_id);

        for (int p = 0; p < 4; p++) {
            uint8_t dir = rand() % 8;
            uint8_t base = (rand() % 2) ? 2 : 3;
            uint8_t exp  = (rand() % 63) + 1;

            print_direction_bits(dirbits, dir);

            pos += snprintf(buffer + pos, sizeof(buffer) - pos,
                            "%s %u^%u%s",
                            dirbits, base, exp,
                            (p < 3 ? ", " : "."));
        }

        lines[i] = strdup(buffer);
    }

    /* Mélange des lignes */
    shuffle_lines(lines, TORUS_CELL_COUNT);

    /* Écriture dans le fichier final */
    FILE *f = fopen(filename, "w");
    if (!f) {
        for (size_t i = 0; i < TORUS_CELL_COUNT; i++) free(lines[i]);
        free(lines);
        return -2;
    }

    for (size_t i = 0; i < TORUS_CELL_COUNT; i++) {
        fprintf(f, "%s\n", lines[i]);
        free(lines[i]);
    }

    free(lines);
    fclose(f);
    return 0;
}
