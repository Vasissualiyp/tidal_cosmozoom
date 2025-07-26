#include "vars.hh"
#include "fft.hh"
#include <complex>

void renormalize_post_fft_array(REAL* rand_array, int n) {
	size_t size_n = static_cast<size_t>(n);
	for (size_t i = 0; i < size_n * size_n * size_n; i++) {
		rand_array[i] = rand_array[i] / (n*n*n);
	}
}


void perform_single_tensor_component_fft(REAL* T_ptr, 
										 std::complex<REAL>* T_ptr_fft, 
										 int n) {
	FFTW::plan_type iplan;
	iplan = FFTW::dft_c2r_3d(n, n, n, 
			reinterpret_cast<float (*)[2]>(T_ptr_fft), 
			T_ptr, FFTW_ESTIMATE);
	FFTW::execute(iplan);
	FFTW::destroy_plan(iplan);
	renormalize_post_fft_array(T_ptr, n);
}
