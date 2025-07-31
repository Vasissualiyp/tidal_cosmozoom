#include "params.hh"
#include "tidal.hh"

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
				TidalTensor_fft.yy_set(idx, Tyy);
				TidalTensor_fft.yz_set(idx, Tyz);
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
	int num_components = TidalTensor.get_num_components();
	for (int i=0; i<num_components; i++) {
		int idx = TidalTensor.tensor_idx_from_component_id(i);
		REAL* real_pointer = TidalTensor.ptr_by_id_in_tensor(idx);
		complex<REAL>* fft_pointer = TidalTensor_fft.ptr_by_id_in_tensor(idx);
		perform_single_tensor_component_fft(real_pointer, fft_pointer, n);
	}
}
