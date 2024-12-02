To run the program:
  - Compile: gcc psort.c -o psort -Wall -Werror -pthread -O
  - Run: ./psort <input_file_name>.bin <output_file_name>.bin
Requires two files:
  - Binary input file (.bin) with 100-byte records with 4-byte key
  - Binary output file (.bin) to store the output of psort
You can generate an input file to test by running createFile.c
  - Compile: gcc createFile.c -o createFile
  - Run: ./createFile <file_name>.bin
  - Opens the file specified in "./createFile <file_name>.bin"
  - Create 2 million 100-byte records (randomized)
