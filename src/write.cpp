#include <fstream>
#include <iostream>
#include <string>
#include "write.hh"
#include "fft.hh"
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

void print_complex_array(FFTW::complex_type* array, int n0, int n1, int n2) {
	for (int i = 0; i < n0; ++i) {
		for (int j = 0; j < n1; ++j) {
			for (int k = 0; k < n2; ++k) {
				std::cout << "(" << array[i * n1 * n2 + j * n2 + k][0] << ", "
						  << array[i * n1 * n2 + j * n2 + k][1] << ") ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

