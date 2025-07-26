#include <math.h>
#include <complex>
#include <iostream>
#include "fft.hh"
#include "params.hh"
#include "gravity.hh"
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

void tidal_tensor_from_potential(FFTW::complex_type* potential_fft,
								 TensorField<std::complex<REAL>>& TidalTensor_fft,
								 Parameters params) {
	using namespace std;

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
				REAL Txx_re, Txy_re, Txz_re, Tyy_re, Tyz_re, Tzz_re;
				REAL Txx_im, Txy_im, Txz_im, Tyy_im, Tyz_im, Tzz_im;
				
				const size_t idx = size_k + (size_n/2 + 1) * (size_j + size_n * size_i);
				const REAL potential_re = potential_fft[idx][0];
				const REAL potential_im = potential_fft[idx][1];

				REAL k_mag2 = kx*kx + ky*ky + kz*kz;

				// Real values for the tensor
				Txx_re = - kx * kx * potential_re - k_mag2 * potential_re / 3;
				Tyy_re = - ky * ky * potential_re - k_mag2 * potential_re / 3;
				Tzz_re = - kz * kz * potential_re - k_mag2 * potential_re / 3;
				Txy_re = - kx * ky * potential_re;
				Txz_re = - kx * kz * potential_re;
				Tyz_re = - ky * kz * potential_re;
				// Imaginary values for the tensor
				Txx_im = - kx * kx * potential_im - k_mag2 * potential_im / 3;
				Tyy_im = - ky * ky * potential_im - k_mag2 * potential_im / 3;
				Tzz_im = - kz * kz * potential_im - k_mag2 * potential_im / 3;
				Txy_im = - kx * ky * potential_im;
				Txz_im = - kx * kz * potential_im;
				Tyz_im = - ky * kz * potential_im;
				// Full complex tensor values
				complex<REAL> Txx(Txx_re, Txx_im);
				complex<REAL> Tyy(Tyy_re, Tyy_im);
				complex<REAL> Tzz(Tzz_re, Tzz_im);
				complex<REAL> Txy(Txy_re, Txy_im);
				complex<REAL> Txz(Txz_re, Txz_im);
				complex<REAL> Tyz(Tyz_re, Tyz_im);
				// Assign the values to the tensor
				TidalTensor_fft.xx_set(idx, Txx);
				TidalTensor_fft.xy_set(idx, Txy);
				TidalTensor_fft.xz_set(idx, Txz);
				TidalTensor_fft.yx_set(idx, Txy);
				TidalTensor_fft.yy_set(idx, Tyy);
				TidalTensor_fft.yz_set(idx, Tyz);
				TidalTensor_fft.zx_set(idx, Txz);
				TidalTensor_fft.zy_set(idx, Tyz);
				TidalTensor_fft.zz_set(idx, Tzz);
			}
		}
	}
}

void ifft_tidal_tensor(TensorField<std::complex<REAL>>& TidalTensor_fft,
					   TensorField<REAL>& TidalTensor,
					   Parameters params) {
	using namespace std;
	int n = params.get<int>("n");
	REAL* Txx_ptr = TidalTensor.xx_ptr();
	REAL* Txy_ptr = TidalTensor.xy_ptr();
	REAL* Txz_ptr = TidalTensor.xz_ptr();
	REAL* Tyx_ptr = TidalTensor.yx_ptr();
	REAL* Tyy_ptr = TidalTensor.yy_ptr();
	REAL* Tyz_ptr = TidalTensor.yz_ptr();
	REAL* Tzx_ptr = TidalTensor.zx_ptr();
	REAL* Tzy_ptr = TidalTensor.zy_ptr();
	REAL* Tzz_ptr = TidalTensor.zz_ptr();

	complex<REAL>* Txx_ptr_fft = TidalTensor_fft.xx_ptr();
	complex<REAL>* Txy_ptr_fft = TidalTensor_fft.xy_ptr();
	complex<REAL>* Txz_ptr_fft = TidalTensor_fft.xz_ptr();
	complex<REAL>* Tyx_ptr_fft = TidalTensor_fft.yx_ptr();
	complex<REAL>* Tyy_ptr_fft = TidalTensor_fft.yy_ptr();
	complex<REAL>* Tyz_ptr_fft = TidalTensor_fft.yz_ptr();
	complex<REAL>* Tzx_ptr_fft = TidalTensor_fft.zx_ptr();
	complex<REAL>* Tzy_ptr_fft = TidalTensor_fft.zy_ptr();
	complex<REAL>* Tzz_ptr_fft = TidalTensor_fft.zz_ptr();
	perform_single_tensor_component_fft(Txx_ptr, Txx_ptr_fft, n);
	perform_single_tensor_component_fft(Txy_ptr, Txy_ptr_fft, n);
	perform_single_tensor_component_fft(Txz_ptr, Txz_ptr_fft, n);
	perform_single_tensor_component_fft(Tyx_ptr, Tyx_ptr_fft, n);
	perform_single_tensor_component_fft(Tyy_ptr, Tyy_ptr_fft, n);
	perform_single_tensor_component_fft(Tyz_ptr, Tyz_ptr_fft, n);
	perform_single_tensor_component_fft(Tzx_ptr, Tzx_ptr_fft, n);
	perform_single_tensor_component_fft(Tzy_ptr, Tzy_ptr_fft, n);
	perform_single_tensor_component_fft(Tzz_ptr, Tzz_ptr_fft, n);
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

void renormalize_post_fft_array(REAL* rand_array, int n) {
	size_t size_n = static_cast<size_t>(n);
	for (size_t i = 0; i < size_n * size_n * size_n; i++) {
		rand_array[i] = rand_array[i] / (n*n*n);
	}
}

