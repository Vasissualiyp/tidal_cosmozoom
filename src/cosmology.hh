#pragma once
#include "vars.hh"
#include "params.hh"
#include "fft.hh"

class TransferFunc {
public:
	TransferFunc(Parameters params);
	void get_3d_tf_array(FFTW::complex_type* tf_fourier);
	void convolve_array_with_tf(FFTW::complex_type* array,
								FFTW::complex_type* tf_array_fft);
private:
	REAL Omega_mh;
	REAL Omega_m;
	REAL h;
	REAL dk;
	int n;
	inline REAL bbks(REAL k);
};
