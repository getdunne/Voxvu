// Declarations for fixed-point data type (16.16 format)
//
/////////////////////////////////////////////////////////////////////////////

typedef long Fixed;

#define FPone 0x10000		// 1.0 in 16.16 format
#define Fone 65536.0		// as above, expressed as a float
#define FPhalf 0x8000		// 0.5 in 16.16 format (use for rounding)

// extract the integer part of a 16.16 format number n
#define FPint(n) ((n) >> 16)

// extract the fractional part of a 16.16 format number n
#define FPfrac(n) ((n) & 0xFFFF)

// round a 16.16 format number n to the nearest integer, returned in ordinary int
// format (NOTE: this is NOT a symmetric round; works ONLY for positive values)
#define FPround(n) FPint((n) + FPhalf)

// convert an integer i to 16.16 format
#define ItoFP(i) (((long)(i))<<16)

// convert a floating-point number f to 16.16 format with rounding
// format (NOTE: this is NOT a symmetric round; works ONLY for positive values)
#define FtoFP(f) (long)((f) * Fone + 0.5)

// convert a 16.16 format number to a float
#define FPtoF(f) ((f)/Fone)
