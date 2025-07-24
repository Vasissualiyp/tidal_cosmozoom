#pragma once
#define USE_DOUBLE 0  // Set to 1 for double, 0 for float
#define G_SI 6.67408e-11  // Gravitational constant (SI)
#define G_COSM 4.301e-9  // Gravitational constant (km^2 Mpc Msun^-1 s^-1)

#if USE_DOUBLE
    #define REAL double
    #define STRTOREAL stod
#else
    #define REAL float
    #define STRTOREAL stof
#endif
