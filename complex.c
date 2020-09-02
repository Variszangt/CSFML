#include "complex.h"

extern const Complex ZERO = { 0, 0 };

/*------------------------------------------------------------------*/
// Basics

char* cxAsString(Complex cx)
{
    char* res = (char*)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);

    char sign = '+';
    if (cx.imag < 0)
    {
        sign = '-';
        cx.imag *= -1;
    }

    int len;
    if (cx.imag == 1)
    {
        len = sprintf_s(res, DEFAULT_BUFFER_SIZE, "z = %g %c i", cx.real, sign);
    }
    else
    {
        len = sprintf_s(res, DEFAULT_BUFFER_SIZE, "z = %g %c %gi", cx.real, sign, cx.imag);
    }

    if (len < 0)
    {
        fatal("Complex number could not be formatted!");
    }

    return realloc(res, (size_t)len + 1);
}

void cxPrint(Complex cx)
{
    printf("%s\n", cxAsString(cx));
}

bool cxEquals(Complex cx1, Complex cx2)
{
    return cx1.real == cx2.real && cx1.imag == cx2.imag;
}

/*------------------------------------------------------------------*/
// Conjugates

Complex cxGetConjugate(Complex cx)
{
    Complex res = { cx.real, -cx.imag };
    return res;
}

// Returns the product of a complex number and its conjugate. |z|^2
double cxGetR2(Complex cx)
{
    return (cx.real * cx.real) + (cx.imag * cx.imag);
}

// Returns the modulus of a complex number. |z|
double cxGetR(Complex cx)
{
    return sqrt(cxGetR2(cx));
}

/*------------------------------------------------------------------*/
// Addition, Subtraction, Multiplication, Division

Complex cxAdd(Complex cx1, Complex cx2)
{
    Complex res = { cx1.real + cx2.real, cx1.imag + cx2.imag };
    return res;
}

Complex cxSub(Complex cx1, Complex cx2)
{
    Complex res = { cx1.real - cx2.real, cx1.imag - cx2.imag };
    return res;
}

Complex cxMul(Complex cx1, Complex cx2)
{
    Complex res;
    res.real = (cx1.real * cx2.real) - (cx1.imag * cx2.imag);
    res.imag = (cx1.real * cx2.imag) + (cx1.imag * cx2.real);
    return res;
}

Complex cxDiv(Complex cx1, Complex cx2)
{
    Complex dividend = cxMul(cx1, cxGetConjugate(cx2));
    double divisor = cxGetR2(cx2);
    if (divisor == 0)
    {
        printf("Divison by 0. Returning 0.");
        return ZERO;
    }

    Complex res = { dividend.real / divisor, dividend.imag / divisor };
    return res;
}

void cxAnalyze(Complex cx)
{
    printf("<----------\nAlgebraic Form:\n%s\n\nConjugate:\n%s\n\nModulus:\n|z|   = %g\n|z|^2 = %g\n\n---------->\n",
           cxAsString(cx), cxAsString(cxGetConjugate(cx)), cxGetR(cx), cxGetR2(cx));
}