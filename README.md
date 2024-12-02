To run the program:
1. Compile: gcc psort.c -o psort -Wall -Werror -pthread -O
2. Run: ./psort <input_file_name>.bin <output_file_name>.bin
Requires two files:
1. Binary input file (.bin) with 100-byte records with 4-byte key
2. Binary output file (.bin) to store the output of psort
You can generate an input file to test by running createFile.c
1. Compile: gcc createFile.c -o createFile
2. Run: ./createFile <file_name>.bin
3. Opens the file specified in "./createFile <file_name>.bin"
4. Create 2 million 100-byte records (randomized)
