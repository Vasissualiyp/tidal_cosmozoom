#include "vars.hh"
#include "params.hh"
#include "cosmology.hh"
#include <math.h>

// This BBKS function implementation was taken from MUSIC and modified
inline REAL bbks(REAL k, REAL m_Gamma){
	REAL q, f1, f2;
	
	if(k < 1e-7 )
		return 1.0;
	
	q = k/(m_Gamma);
	f1 = log(1.0 + 2.34*q)/(2.34*q);
	f2 = 1.0 + q*(3.89 + q*(259.21 + q*(162.771336 + q*2027.16958081)));
	
	return f1/sqrt(sqrt(f2));
	
}
