#pragma once
#include "vars.hh"
#include "params.hh"

Parameters cut_boundaries(REAL* pre_cut_arr, 
						  REAL* post_cut_arr,
						  Parameters params,
						  int boundary_change);
int* find_loc_of_max_in_array(REAL* array, int n);
