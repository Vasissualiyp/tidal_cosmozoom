#pragma once
#include "vars.hh"
#include "params.hh"

class TransferFunc {
public:
	TransferFunc(Parameters params);
private:
	inline REAL bbks(REAL k, REAL m_Gamma);
};
