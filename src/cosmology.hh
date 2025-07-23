#pragma once
#include "vars.hh"
#include "params.hh"
#include "fft.hh"

class TransferFunc {
public:
	TransferFunc(Parameters params);
	void get_3d_tf_array(FFTW::complex_type* tf_fourier);
private:
	REAL m_Gamma;
	REAL dk;
	int n;
	inline REAL bbks(REAL k);
};
