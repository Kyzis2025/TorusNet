#include "torus.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {

    srand((unsigned int)time(NULL));

    uint16_t case_ids[CASE_ID_COUNT];
    uint16_t unused_ids[CASE_ID_COUNT];
    uint16_t torus_id;

    if (generate_case_ids(case_ids, unused_ids, &torus_id) != 0) {
        printf("Error: case ID generation failed.\n");
        return 1;
    }

    if (generate_binary_bits("torus_bits.bin") != 0) {
        printf("Error: binary bits generation failed.\n");
        return 1;
    }

    if (generate_ascii_paths("torus_paths.txt", case_ids, torus_id) != 0) {
        printf("Error: ASCII paths generation failed.\n");
        return 1;
    }

    if (save_unused_ids("ids_mutation.txt", unused_ids) != 0) {
        printf("Error: unused IDs file generation failed.\n");
        return 1;
    }

    printf("Torus generated:\n");
    printf(" - Normal bits file: torus_bits.bin\n");
    printf(" - Directional structure: torus_paths.txt\n");
    printf(" - Unused IDs (mutation base): ids_mutation.txt\n");
    printf(" - Torus ID: %04X\n", torus_id);

    return 0;
}
