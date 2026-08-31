#include "torus.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {

    const char *binary_file = "torus_bits.bin";
    const char *ascii_file  = "torus_paths.txt";

    srand((unsigned int)time(NULL));
    uint16_t torus_id = rand() & 0xFFFF;

    if (generate_binary_bits(binary_file) != 0) {
        printf("Erreur: génération du fichier binaire.\n");
        return 1;
    }

    if (generate_ascii_paths(ascii_file, torus_id) != 0) {
        printf("Erreur: génération du fichier ASCII.\n");
        return 1;
    }

    printf("Tore privé généré avec succès.\n");
    printf(" - Bits normaux : %s\n", binary_file);
    printf(" - Chemins ASCII (mélangés) : %s\n", ascii_file);

    return 0;
}
