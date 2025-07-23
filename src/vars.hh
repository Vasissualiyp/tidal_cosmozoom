#define USE_DOUBLE 1  // Set to 1 for double, 0 for float

#if USE_DOUBLE
    #define REAL double
    #define STRTOREAL stod
#else
    #define REAL float
    #define STRTOREAL stof
#endif
