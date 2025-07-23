#include "vars.hh"
#include "params.hh"
#include "fft.hh"
#include "cosmology.hh"
#include <math.h>

// Constructor
TransferFunc::TransferFunc(Parameters params) {
	m_Gamma = params.get<REAL>("m_Gamma");
	dk = params.get<REAL>("dk");
	n = params.get<int>("n");
}

void TransferFunc::get_3d_tf_array(FFTW::complex_type* tf_fourier) {
	size_t size_n = static_cast<size_t>(n);
	for (size_t i = 0; i < size_n; ++i) {
	    const REAL kx = (i < size_n/2) ? i * dk : (i - size_n) * dk;
	    
	    for (size_t j = 0; j < size_n; ++j) {
	        const REAL ky = (j < size_n/2) ? j * dk : (j - size_n) * dk;
	        
	        for (size_t k = 0; k <= size_n/2; ++k) {  // Only non-negative kz
	            const REAL kz = k * dk;
	            
	            // Wavevector magnitude
	            const REAL k_mag = std::sqrt(kx*kx + ky*ky + kz*kz);
	            
	            // Compute transfer function T(k)
	            const REAL Tk = bbks(k_mag);
	            
	            // Store T(k) in complex array (real part only)
	            const size_t idx = k + (size_n/2 + 1) * (j + size_n * i);
	            tf_fourier[idx][0] = Tk;  // Real component
	            tf_fourier[idx][1] = 0.0; // Imaginary component (TF is real)
	        }
	    }
	}
}
// This BBKS function implementation was taken from MUSIC and modified
inline REAL TransferFunc::bbks(REAL k){
	REAL q, f1, f2;
	
	if(k < 1e-7 )
		return 1.0;
	
	q = k/(m_Gamma);
	f1 = log(1.0 + 2.34*q)/(2.34*q);
	f2 = 1.0 + q*(3.89 + q*(259.21 + q*(162.771336 + q*2027.16958081)));
	
	return f1/sqrt(sqrt(f2));
}
