#pragma once
#include "vars.hh"
#include "params.hh"
#include "fft.hh"

class TransferFunc {
public:
	TransferFunc(Parameters params);
	void get_3d_tf_array(FFTW::complex_type* tf_fourier);
private:
	REAL Omega_mh;
	REAL Omega_m;
	REAL h;
	REAL dk;
	int n;
	inline REAL bbks(REAL k);
};
