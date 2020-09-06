#ifndef COMPLEX_H
#define COMPLEX_H

#include "global.h"

/*------------------------------------------------------------------*/
// Basics

// The algebraic form of a complex number
typedef struct
{
    // Algebraic form:
    double real; // "a"
    double imag; // "b"
    bool _algebraic;

    // Polar form:
    double mod; // "r"
    double arg; // "phi"
    bool _polar;

} Complex;

Complex cxFromAlgebraic(double real, double imaginary);
Complex cxFromPolar(double modulus, double argument);

extern const Complex cxZERO;
extern const Complex cxNEW;

char* cxAsString(Complex cx);

void cxPrint(Complex cx);

bool cxEquals(Complex cx1, Complex cx2);

/*------------------------------------------------------------------*/
// Conjugates

Complex cxGetConjugate(Complex cx);

// Returns the product of a complex number and its conjugate. |z|^2
double cxGetR2(Complex cx);

// Returns the modulus of a complex number. |z|
double cxGetR(Complex cx);

/*------------------------------------------------------------------*/
// Addition, Subtraction, Multiplication, Division

Complex cxAdd(Complex cx1, Complex cx2);
Complex cxSub(Complex cx1, Complex cx2);
Complex cxMul(Complex cx1, Complex cx2);
Complex cxDiv(Complex cx1, Complex cx2);
Complex cxPow(Complex cx1, double to);

/*------------------------------------------------------------------*/
// Everything together

void cxAnalyze(Complex cx);

void cxRun();

#endif