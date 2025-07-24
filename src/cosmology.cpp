#include "vars.hh"
#include "params.hh"
#include "fft.hh"
#include "cosmology.hh"
#include <math.h>

// Constructor
TransferFunc::TransferFunc(Parameters params) {
	Omega_m = params.get<REAL>("Omega_m");
	h = params.get<REAL>("h");
	dk = params.get<REAL>("dk");
	n = params.get<int>("n");
	Omega_mh = Omega_m * h;
}

void TransferFunc::get_3d_tf_array(FFTW::complex_type* tf_fourier) {
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
	            
	            // Compute transfer function T(k)
	            const REAL Tk = bbks(k_mag);
    
	            // Store T(k) in complex array (real part only)
	            const size_t idx = size_k + (size_n/2 + 1) * (size_j + size_n * size_i);
	            tf_fourier[idx][0] = Tk;  // Real component
	            tf_fourier[idx][1] = 0.0; // Imaginary component (TF is real)
	        }
	    }
	}
}

void TransferFunc::convolve_array_with_tf(FFTW::complex_type* array, 
										  FFTW::complex_type* tf_array_fft) {
	size_t size_n = static_cast<size_t>(n);
	for (size_t idx = 0; idx < size_n * size_n * (size_n/2 + 1); ++idx) {
		const REAL re_in = array[idx][0];
		const REAL im_in = array[idx][1];
		const REAL Tk = tf_array_fft[idx][0];
		
		array[idx][0] = re_in * Tk;
		array[idx][1] = im_in * Tk;
	}
}
// This BBKS function implementation was taken from MUSIC and modified
inline REAL TransferFunc::bbks(REAL k){
	REAL q, f1, f2;
	
	if(k < 1e-7 )
		return 1.0;
	
	q = k/(Omega_mh);
	f1 = log(1.0 + 2.34*q)/(2.34*q);
	f2 = 1.0 + q*(3.89 + q*(259.21 + q*(162.771336 + q*2027.16958081)));
	
	return f1/sqrt(sqrt(f2));
}
