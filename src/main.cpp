#include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>
#include "vars.hh"
#include "params.hh"
#include "fft.hh"
#include "cosmology.hh"

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
void write_field_to_binary_file(REAL* rand_array, int n, std::string filename="array3d.bin") {
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
	std::mt19937 rand_gen(params.get<int>("seed"));
	std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);

	// Allocate arrays
	int n = params.get<int>("n");
	REAL *rand_array = new REAL[n*n*n];
	int fftw_c_size = n * n * (n/2 +1); // Size of half-complex fft
	FFTW::complex_type *rand_array_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::complex_type *tf_array_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	
	populate_array_with_random_values(rand_array, n, uniform_dist, rand_gen, print_array);
	
	// Get FFT of random noise
	FFTW::plan_type plan, iplan;
	plan = FFTW::dft_r2c_3d(n, n, n, rand_array, rand_array_fft, FFTW_ESTIMATE);
	iplan = FFTW::dft_c2r_3d(n, n, n, rand_array_fft, rand_array, FFTW_ESTIMATE);
	FFTW::execute(plan);

	// Generate transfer function
	TransferFunc tf = TransferFunc(params);
	tf.get_3d_tf_array(tf_array_fft);

	// Convolve white noise with TF in Fourier space 
	size_t size_n = static_cast<size_t>(n);
	for (size_t idx = 0; idx < size_n * size_n * (size_n/2 + 1); ++idx) {
		const REAL re_in = rand_array_fft[idx][0];
		const REAL im_in = rand_array_fft[idx][1];
		const REAL Tk = tf_array_fft[idx][0];
		
		rand_array_fft[idx][0] = re_in * Tk;
		rand_array_fft[idx][1] = im_in * Tk;
	}
	FFTW::execute(iplan);
	for (size_t i = 0; i < size_n * size_n * size_n; i++) {
		rand_array[i] = rand_array[i]/n/n/n;
	}
	write_field_to_binary_file(rand_array, n, "array3d.bin");
	//print_complex_array(rand_array_fft, n, n, n/2+1);
	
	// Clean up
	FFTW::destroy_plan(plan);
	FFTW::free(rand_array_fft);
	delete[] rand_array;
	
	return 0;
}
