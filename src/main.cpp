#include <cstdlib>
#include <complex>
#include <iostream>
#include "vars.hh"
#include "params.hh"
#include "write.hh"
#include "fft.hh"
#include "cosmology.hh"
#include "gravity.hh"
#include "random.hh"
#include "arrays.hh"
#include "tidal.hh"

int main(int argc, char *argv[]) {
	using namespace std::complex_literals;
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

	// Real data arrays
	int n = params.get<int>("n");
	REAL *rand_array = new REAL[n*n*n];
	REAL *grav_potential = new REAL[n*n*n];
	TensorField<REAL> TidalTensor(n, n, n);

	// FFT arrays
	int fftw_c_size = n * n * (n/2 +1); // Size of half-complex fft
	FFTW::complex_type *rand_array_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::complex_type *tf_array_fft   = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::complex_type *grav_potential_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	TensorField<std::complex<REAL>> TidalTensor_fft(n, n, n/2+1);
	
	populate_array_with_random_values(rand_array, n, uniform_dist, rand_gen, print_array);
	
	// Get FFT of random noise
	FFTW::plan_type plan, iplan_dens, iplan_potential;
	plan = FFTW::dft_r2c_3d(n, n, n, rand_array, rand_array_fft, FFTW_ESTIMATE);
	FFTW::execute(plan);

	// Generate transfer function
	TransferFunc tf = TransferFunc(params);
	tf.get_3d_tf_array(tf_array_fft);
	tf.convolve_array_with_tf(rand_array_fft, tf_array_fft);
	
	// Solve for the gravitational potential and tidal fields in the Fourer space
	solve_poisson_in_fourier_space(rand_array_fft, grav_potential_fft, params);
	tidal_tensor_from_potential(grav_potential_fft, TidalTensor_fft, params);
	ifft_tidal_tensor(TidalTensor_fft, TidalTensor, params);

	// Obtain the gravitational potential
	iplan_potential = FFTW::dft_c2r_3d(n, n, n, grav_potential_fft, grav_potential, FFTW_ESTIMATE);
	FFTW::execute(iplan_potential);

	// Get back the overdensity field
	iplan_dens = FFTW::dft_c2r_3d(n, n, n, rand_array_fft, rand_array, FFTW_ESTIMATE);
	FFTW::execute(iplan_dens);

	renormalize_post_fft_array(rand_array, n);
	renormalize_post_fft_array(grav_potential, n);
	write_field_to_binary_file(rand_array,     n, "out/overdensity.bin");
	write_field_to_binary_file(grav_potential, n, "out/potential.bin");
	TidalTensor.write_tensor_to_binary_files();
	//print_complex_array(grav_potential_fft, n, n, n/2+1);
	
	// Clean up
	FFTW::destroy_plan(plan);
	FFTW::destroy_plan(iplan_dens);
	FFTW::destroy_plan(iplan_potential);
	FFTW::free(rand_array_fft);
	FFTW::free(grav_potential_fft);
	delete[] rand_array; 
	delete[] grav_potential;
	
	return 0;
}
