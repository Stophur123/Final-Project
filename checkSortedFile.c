#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char record[100];
    size_t record_index = 0;

    // Read the records
    while (fread(record, sizeof(char), 100, file)) {
        printf("Key: ");
        for (int i = 0; i < 4; ++i) {
            printf("%02x", (unsigned char)record[i]);
        }
        printf(" | Next 6 bytes: ");
        for (int j = 4; j < 10; ++j) {
            printf("%02x", (unsigned char)record[j]);
        }
        printf("\n");
        record_index++;
    }

    fclose(file);

    return 0;
}
