#include <cstdlib>
#include <iostream>
#include <ostream>
#include "vars.hh"
#include "params.hh"
#include "write.hh"
#include "fft.hh"
#include "cosmology.hh"
#include "random.hh"

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
	size_t size_n = static_cast<size_t>(n);
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
	tf.convolve_array_with_tf(rand_array_fft, tf_array_fft);
	FFTW::execute(iplan);

	// Renormalize the array
	for (size_t i = 0; i < size_n * size_n * size_n; i++) {
		rand_array[i] = rand_array[i] / (n*n*n);
	}
	write_field_to_binary_file(rand_array, n, "array3d.bin");
	//print_complex_array(rand_array_fft, n, n, n/2+1);
	
	// Clean up
	FFTW::destroy_plan(plan);
	FFTW::free(rand_array_fft);
	delete[] rand_array;
	
	return 0;
}
