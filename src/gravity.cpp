#include <math.h>
#include <complex>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "fft.hh"
#include "params.hh"
#include "random.hh"
#include "arrays.hh"
#include "gravity.hh"
#include "cosmology.hh"
#include "vars.hh"
#include "tidal.hh"

void solve_poisson_in_fourier_space(FFTW::complex_type* overdensity_fft, 
									FFTW::complex_type* potential_fft,
									Parameters params) {
	int n = params.get<int>("n");
	REAL dk = params.get<REAL>("dk");
	size_t size_n = static_cast<size_t>(n);
	for (int i = 0; i < n; ++i) {
		const REAL kx = (i < n/2) ? i * dk : (i - n) * dk;
		size_t size_i = static_cast<size_t>(i);
		
		for (int j = 0; j < n; ++j) {
			const REAL ky = (j < n/2) ? j * dk : (j - n) * dk;
			size_t size_j = static_cast<size_t>(j);
			
			for (int k = 0; k <= n/2; ++k) {  // Only non-negative kz
				const REAL kz = k * dk;
				size_t size_k = static_cast<size_t>(k);
				
				REAL k_mag2 = kx*kx + ky*ky + kz*kz;

				// Solve the Poisson
				const size_t idx = size_k + (size_n/2 + 1) * (size_j + size_n * size_i);
				REAL re = overdensity_fft[idx][0];
				REAL im = overdensity_fft[idx][1];
				if (k_mag2 == 0.0) {
					re=0.0; im=0.0;
				} else {
					re = - 4 * M_PI * G_COSM * re / k_mag2;
					im = - 4 * M_PI * G_COSM * im / k_mag2;
				}
				potential_fft[idx][0] = re;
				potential_fft[idx][1] = im;
			}
		}
	}
}

void calculate_and_save_fields_from_overdensity(REAL* overdensity, 
												Parameters params,
												bool print_header) {
	int n = params.get<int>("n");
	int n_eff = params.get<int>("n_eff");
	int write_to_file = params.get<int>("write_fields_to_files");
	int write_output  = params.get<int>("output_logs");
	std::string postfix = ".";
	postfix.append(std::to_string(n_eff));
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

	if (write_to_file==1) {
		std::string overdensity_fname = "out/overdensity";
		std::string potential_fname   = "out/potential";
		overdensity_fname.append(postfix).append(".bin");
		potential_fname.append(postfix).append(".bin");
	
		write_field_to_binary_file(overdensity,    n, overdensity_fname, write_output);
		write_field_to_binary_file(grav_potential, n, potential_fname,   write_output);
		TidalTensor.write_tensor_to_binary_files(n_eff, postfix, write_output);
	}
	int center_idx = n/2 * ( n * n + n + 1 );
	if (print_header==true) 
		TidalTensor.print_tensor_table_header();
	TidalTensor.print_tensor_table_at_loc(center_idx, n_eff);

	FFTW::destroy_plan(plan);
	FFTW::destroy_plan(iplan_dens);
	FFTW::free(grav_potential_fft);
	delete[] grav_potential;
}

void generate_overdensity_field(REAL*& overdensity, Parameters& params) {

	int n = params.get<int>("n");
	int print_array = params.get<int>("print_array");

	// FFT arrays
	int fftw_c_size = n * n * (n/2 +1); // Size of half-complex fft
	FFTW::complex_type *overdensity_fft = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);
	FFTW::complex_type *tf_array_fft   = (FFTW::complex_type*)FFTW::malloc(sizeof(FFTW::complex_type) * fftw_c_size);

	// Random number generation
	std::mt19937 rand_gen(params.get<int>("seed"));
	std::uniform_real_distribution<> uniform_dist(-1.0, 1.0);

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

