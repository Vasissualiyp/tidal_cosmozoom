#include <math.h>
#include "fft.hh"
#include "params.hh"
#include "gravity.hh"
#include "vars.hh"

void solve_poisson_in_fourier_space(FFTW::complex_type* overdensity_fft, 
									FFTW::complex_type* potential_fft,
									Parameters params) {
	int n = params.get<int>("n");
	int dk = params.get<REAL>("dk");
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
				
				// Wavevector magnitude
				const REAL k_mag = std::sqrt(kx*kx + ky*ky + kz*kz);

				// Solve the Poisson
				const size_t idx = size_k + (size_n/2 + 1) * (size_j + size_n * size_i);
				REAL re = overdensity_fft[idx][0];
				REAL im = overdensity_fft[idx][1];
				re = - 4 * M_PI * G_COSM * re / k_mag / k_mag;
				im = - 4 * M_PI * G_COSM * im / k_mag / k_mag;
				potential_fft[idx][0] = re;
				potential_fft[idx][1] = im;
			}
		}
	}
}
