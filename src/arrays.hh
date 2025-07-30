#pragma once
#include "vars.hh"
#include "params.hh"

Parameters remove_boundaries(REAL* pre_cut_arr, 
							 REAL* post_cut_arr,
							 Parameters params,
							 int boundary_change);
