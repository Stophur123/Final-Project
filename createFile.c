#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL)); // <--- random num generator
    size_t numRecords = 2000000;
    FILE *file = fopen("input.bin", "wb");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    char record[100]; // because each record is 100 bytes long

    // Generate the records
    for (size_t i = 0; i < numRecords; ++i) {
        for (size_t j = 0; j < sizeof(record); ++j) {
            record[j] = rand() % 256; // 256 is range of values for byte
        }
        fwrite(record, 1, sizeof(record), file);
    }

    fclose(file);

    return 0;
}
