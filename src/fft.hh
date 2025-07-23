#pragma once
#include "vars.hh"
#include <fftw3.h>
#include <string>
#include <cstddef>

/**
 * Template traits class for FFTW type-specific functions and types.
 * Provides a unified interface for both float and double precision FFTW operations.
 */
template<typename T>
struct FFTWTraits;

/**
 * Specialization for single precision (float) FFTW operations
 */
template<>
struct FFTWTraits<float> {
    using real_type = float;
    using plan_type = fftwf_plan;
    using complex_type = fftwf_complex;
    
    // Memory management
    static void* malloc(size_t size) { 
        return fftwf_malloc(size); 
    }
    
    static void free(void* ptr) { 
        fftwf_free(ptr); 
    }
    
    // Plan execution
    static void execute(fftwf_plan plan) { 
        fftwf_execute(plan); 
    }
    
    static void destroy_plan(fftwf_plan plan) {
        fftwf_destroy_plan(plan);
    }
    
    // 3D transforms
    static fftwf_plan dft_r2c_3d(int n0, int n1, int n2, 
								float* in, fftwf_complex* out, 
								unsigned flags) {
        return fftwf_plan_dft_r2c_3d(n0, n1, n2, in, out, flags);
    }
    
    static fftwf_plan dft_c2r_3d(int n0, int n1, int n2, 
								fftwf_complex* in, float* out, unsigned flags) {
        return fftwf_plan_dft_c2r_3d(n0, n1, n2, in, out, flags);
    }
    
    static fftwf_plan dft_3d(int n0, int n1, int n2, 
							fftwf_complex* in, fftwf_complex* out, int sign,
								unsigned flags) {
        return fftwf_plan_dft_3d(n0, n1, n2, in, out, sign, flags);
    }
    
    // String conversion
    static float strtoreal(const std::string& str) { 
        return std::stof(str); 
    }
    
    // Cleanup
    static void cleanup() {
        fftwf_cleanup();
    }
};

/**
 * Specialization for double precision (double) FFTW operations
 */
template<>
struct FFTWTraits<double> {
    using real_type = double;
    using plan_type = fftw_plan;
    using complex_type = fftw_complex;
    
    // Memory management
    static void* malloc(size_t size) { 
        return fftw_malloc(size); 
    }
    
    static void free(void* ptr) { 
        fftw_free(ptr); 
    }
    
    // Plan execution
    static void execute(fftw_plan plan) { 
        fftw_execute(plan); 
    }
    
    static void destroy_plan(fftw_plan plan) {
        fftw_destroy_plan(plan);
    }
    
    // 3D transforms
    static fftw_plan dft_r2c_3d(int n0, int n1, int n2, 
								double* in, fftw_complex* out,
								unsigned flags) {
        return fftw_plan_dft_r2c_3d(n0, n1, n2, in, out, flags);
    }
    
    static fftw_plan dft_c2r_3d(int n0, int n1, int n2, 
								fftw_complex* in, double* out,
								unsigned flags) {
        return fftw_plan_dft_c2r_3d(n0, n1, n2, in, out, flags);
    }
    
    static fftw_plan dft_3d(int n0, int n1, int n2, 
								fftw_complex* in, fftw_complex* out, int sign,
								unsigned flags) {
        return fftw_plan_dft_3d(n0, n1, n2, in, out, sign, flags);
    }
    
    // String conversion
    static double strtoreal(const std::string& str) { 
        return std::stod(str); 
    }
    
    // Cleanup
    static void cleanup() {
        fftw_cleanup();
    }
};

using FFTW = FFTWTraits<REAL>;
