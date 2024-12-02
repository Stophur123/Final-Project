To run the program:\n
	Compile: gcc psort.c -o psort -Wall -Werror -pthread -O\n
	Run: ./psort <input_file_name>.bin <output_file_name>.bin\n
Requires two files:\n
	Binary input file (.bin) with 100-byte records with 4-byte key\n
	Binary output file (.bin) to store the output of psort\n
You can generate an input file to test by running createFile.c\n
	Compile: gcc createFile.c -o createFile\n
	Run: ./createFile <file_name>.bin\n
	Opens the file specified in "./createFile <file_name>.bin"\n
	Create 2 million 100-byte records (randomized)\n
