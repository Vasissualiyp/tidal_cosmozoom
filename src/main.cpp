#include <cstdlib>
#include <iostream>
#include <ostream>
#include <fftw3.h>
#include <random>
#include "params.hh"
#include "vars.hh"
#include "fft.hh"

using FFTW = FFTWTraits<REAL>;

void populate_array_with_random_values(REAL* rand_array, int n, 
									   std::uniform_real_distribution<> uniform_dist,
									   std::mt19937 rand_gen,
									   short print_array = 0) {
	REAL element;
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			for (int k=0; k<n; k++) {
				*(rand_array + i * n * n + j * n + k) = uniform_dist(rand_gen);
				element = rand_array[i*n*n + j*n + k];
				if (print_array == 1) {
					std::cout << element << " ";
				}
			}
			if (print_array == 1) {
				std::cout << std::endl;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	short print_array = 0;
	const char* filename;

	// CLI: pass path to the parameter file and read the file
	if (argc < 2)
		filename = "params.txt";
	else
		filename = argv[1];
	Parameters params;
	params.read_params_from_file(filename);

	// Random number generation
	std::mt19937 rand_gen(params.get_seed());
	std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);

	int n = params.get_n();
	REAL *rand_array = new REAL[n*n*n];
    FFTW::complex_type *rand_array_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * n * n * n);
    
    populate_array_with_random_values(rand_array, n, uniform_dist, rand_gen, print_array);
    
    FFTW::plan_type plan;
    plan = FFTW::dft_r2c_3d(n, n, n, rand_array, rand_array_fft, FFTW_ESTIMATE);
    FFTW::execute(plan);
    
    // Clean up
    FFTW::destroy_plan(plan);
    FFTW::free(rand_array_fft);
    delete[] rand_array;
	
	return 0;
}
