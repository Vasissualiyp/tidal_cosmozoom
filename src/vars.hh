#define USE_DOUBLE 0  // Set to 1 for double, 0 for float

#if USE_DOUBLE
    #define REAL double
    #define STRTOREAL stod
    #define FFTW_PLAN fftw_plan
    #define FFTW_MALLOC fftw_malloc
    #define FFTW_EXECUTE fftw_execute
    #define FFTW_COMPLEX fftw_complex
    #define FFTW_DFT_R2C_3D fftw_plan_dft_r2c_3d
#else
    #define REAL float
    #define STRTOREAL stof
    #define FFTW_PLAN fftwf_plan
    #define FFTW_MALLOC fftwf_malloc
    #define FFTW_EXECUTE fftwf_execute
    #define FFTW_COMPLEX fftwf_complex
    #define FFTW_DFT_R2C_3D fftwf_plan_dft_r2c_3d
#endif
