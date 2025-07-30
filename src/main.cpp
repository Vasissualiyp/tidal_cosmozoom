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

void calculate_and_save_fields_from_overdensity(REAL* overdensity, Parameters params) {
	int n = params.get<int>("n");
	std::string postfix = ".";
	postfix.append(std::to_string(n));
	REAL *grav_potential = new REAL[n*n*n];
	TensorField<REAL> TidalTensor(n, n, n, true);

	int fftw_c_size = n * n * (n/2 +1); // Size of half-complex fft
	FFTW::complex_type *grav_potential_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::complex_type *overdensity_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	TensorField<std::complex<REAL>> TidalTensor_fft(n, n, n/2+1, true);

	FFTW::plan_type plan, iplan_dens, iplan_potential;

	plan = FFTW::dft_r2c_3d(n, n, n, overdensity, overdensity_fft, FFTW_ESTIMATE);
	FFTW::execute(plan);

	solve_poisson_in_fourier_space(overdensity_fft, grav_potential_fft, params);
	tidal_tensor_from_potential(grav_potential_fft, TidalTensor_fft, params);
	ifft_tidal_tensor(TidalTensor_fft, TidalTensor, params);

	// Obtain the gravitational potential
	iplan_potential = FFTW::dft_c2r_3d(n, n, n, grav_potential_fft, grav_potential, FFTW_ESTIMATE);
	FFTW::execute(iplan_potential);

	// Get back the overdensity field
	iplan_dens = FFTW::dft_c2r_3d(n, n, n, overdensity_fft, overdensity, FFTW_ESTIMATE);
	FFTW::execute(iplan_dens);

	renormalize_post_fft_array(overdensity, n);
	renormalize_post_fft_array(grav_potential, n);

	std::string overdensity_fname = "out/overdensity";
	std::string potential_fname   = "out/potential";
	overdensity_fname.append(postfix).append(".bin");
	potential_fname.append(postfix).append(".bin");

	write_field_to_binary_file(overdensity,    n, overdensity_fname);
	write_field_to_binary_file(grav_potential, n, potential_fname  );
	TidalTensor.write_tensor_to_binary_files(postfix);

	FFTW::destroy_plan(plan);
	FFTW::destroy_plan(iplan_dens);
	FFTW::free(grav_potential_fft);
	delete[] grav_potential;
}

int* get_new_center_from_largest_potential(FFTW::complex_type* overdensity_fft, Parameters params) {
	int n = params.get<int>("n");
	REAL *grav_potential = new REAL[n*n*n];

	int fftw_c_size = n * n * (n/2 +1); // Size of half-complex fft
	FFTW::complex_type *grav_potential_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::plan_type iplan_potential;

	// Obtain the gravitational potential
	solve_poisson_in_fourier_space(overdensity_fft, grav_potential_fft, params);
	iplan_potential = FFTW::dft_c2r_3d(n, n, n, grav_potential_fft, grav_potential, FFTW_ESTIMATE);
	FFTW::execute(iplan_potential);

	// Identify the location of max
	int* max_loc = find_loc_of_max_in_array(grav_potential, n, false);
	FFTW::destroy_plan(iplan_potential);
	FFTW::free(grav_potential_fft);
	delete[] grav_potential;

	return max_loc;
}

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

	// FFT arrays
	int fftw_c_size = n * n * (n/2 +1); // Size of half-complex fft
	FFTW::complex_type *overdensity_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::complex_type *tf_array_fft   = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	
	REAL *overdensity = new REAL[n*n*n];
	populate_array_with_random_values(overdensity, n, uniform_dist, rand_gen, print_array);
	
	// Get FFT of random noise
	FFTW::plan_type plan, iplan_dens;
	plan = FFTW::dft_r2c_3d(n, n, n, overdensity, overdensity_fft, FFTW_ESTIMATE);
	FFTW::execute(plan);

	// Generate transfer function
	TransferFunc tf = TransferFunc(params);
	tf.get_3d_tf_array(tf_array_fft);
	tf.convolve_array_with_tf(overdensity_fft, tf_array_fft);
	FFTW::free(tf_array_fft);
	int* new_center = get_new_center_from_largest_potential(overdensity_fft, params);

	// Get the overdensity field 
	iplan_dens = FFTW::dft_c2r_3d(n, n, n, overdensity_fft, overdensity, FFTW_ESTIMATE);
	FFTW::execute(iplan_dens);
	shift_array_to_pos<REAL>(overdensity, new_center, n);

	FFTW::free(overdensity_fft);
	FFTW::destroy_plan(plan);
	FFTW::destroy_plan(iplan_dens);

	calculate_and_save_fields_from_overdensity(overdensity, params);
	
	int dn = 10;
	int smallest_box = 10;
	int num_boundary_cutoffs = n / dn;
	for (int i=0; i<num_boundary_cutoffs; i++) {
		int n_cut = params.get<int>("n") - 2*dn;
		if (n_cut > smallest_box) {
			REAL *overdensity_cut = new REAL[n_cut*n_cut*n_cut];
			Parameters cut_params = cut_boundaries(overdensity, overdensity_cut, params, dn);
			calculate_and_save_fields_from_overdensity(overdensity_cut, cut_params);
			delete[] overdensity;
			overdensity = overdensity_cut;
			params = cut_params;
		}
	}
	// Cutting the array test
	//cut_boundaries(rand_array, rand_array_cut, params, dn);
	//Parameters cut_params = cut_boundaries(rand_array, rand_array_cut, params, dn);
	
	// Clean up
	delete[] overdensity; 
	
	return 0;
}
