#pragma once
#include "vars.hh"
#include "params.hh"

Parameters cut_boundaries(REAL* pre_cut_arr, 
						  REAL* post_cut_arr,
						  Parameters params);
int* find_loc_of_max_in_array(REAL* array, int n, bool max_bool);

template<typename T>
void shift_array_to_pos(T*& array, int* pos, int n) {
	T *shifted_array = new T[n*n*n];
	int shift_i = n/2 - pos[0];
	int shift_j = n/2 - pos[1];
	int shift_k = n/2 - pos[2];
	int shifted_i, shifted_j, shifted_k;
	for (int i=0; i<n; i++) {
		shifted_i = (i + shift_i + n) % n ;
		for (int j=0; j<n; j++) {
			shifted_j = (j + shift_j + n) % n ;
			for (int k=0; k<n; k++) {
				shifted_k = (k + shift_k + n) % n ;
				const int idx = i*n*n + j*n + k;
				const int shifted_idx = shifted_i*n*n + shifted_j*n + shifted_k;
				shifted_array[shifted_idx] = array[idx];
			}
		}
	}
	delete[] array; 
	array = shifted_array;
}
