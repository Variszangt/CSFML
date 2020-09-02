#ifndef COMPLEX_H
#define COMPLEX_H

#include "global.h"

/*------------------------------------------------------------------*/
// Basics

typedef struct
{
    double real;
    double imag;
} Complex;

extern const Complex ZERO;

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

/*------------------------------------------------------------------*/
// Polar form

// TODO

/*------------------------------------------------------------------*/
// Everything together

void cxAnalyze(Complex cx);

#endif