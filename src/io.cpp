#include <fstream>
#include <iostream>
#include <string>
#include "io.hh"
#include "fft.hh"
#include "vars.hh"

void write_field_to_binary_file(REAL* rand_array, int n, std::string filename,
								int write_output) {
	// Write to binary file
	std::ofstream outFile(filename, std::ios::binary);
	if (outFile) {
		outFile.write(reinterpret_cast<const char*>(rand_array), n*n*n * sizeof(REAL));
		outFile.close();
		if (write_output==1)
			std::cout << "File " << filename << " saved successfully.\n";
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

Parameters obtain_params_from_cli(int argc, char *argv[]) {
	const char* filename;
	int seed;
	if (argc < 2) {
		filename = "params.txt";
		seed = 0;
	} else if (argc == 2) {
		filename = argv[1];
		seed = 0;
	} else {
		filename = argv[1];
		seed = atoi(argv[2]);
	}
	Parameters params;
	params.read_params_from_file(filename);
	if (seed != 0)
		params.set_seed(seed);
	return params;
}

