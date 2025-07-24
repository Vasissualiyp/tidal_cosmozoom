#include <fstream>
#include <iostream>
#include <string>
#include "write.hh"
#include "vars.hh"

void write_field_to_binary_file(REAL* rand_array, int n, std::string filename) {
	// Write to binary file
	std::ofstream outFile(filename, std::ios::binary);
	if (outFile) {
		outFile.write(reinterpret_cast<const char*>(rand_array), n*n*n * sizeof(REAL));
		outFile.close();
		std::cout << "File saved successfully.\n";
	} else {
		std::cerr << "Error opening file!\n";
	}
}
