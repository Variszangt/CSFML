#include "complex.h"

#include <assert.h>
#include <ctype.h>

extern const Complex cxZERO = { 0, 0, true,  0, 0, true  };
extern const Complex cxNEW  = { 0, 0, false, 0, 0, false }; 

/*------------------------------------------------------------------*/
// Basics

// z = a + i * b
Complex cxFromAlgebraic(double real, double imaginary)
{
    Complex res = cxNEW;

    res.real = real;
    res.imag = imaginary;
    res._algebraic = true;

    double r = cxGetR(res);
    if (r == 0)
    {
        res.mod = 0;
        res.arg = 0;
        res._polar = true;
        return res;
    }

    double sin_of_phi = res.imag / r;
    double phi = deg(asin(sin_of_phi));
    if (res.real < 0)
    {
        // If we're in sectors I/IV (a > 0), phi is good.
        // However for sectors II/III (a < 0), phi must be adjusted.
        phi = 180 - phi;
    }
    res.mod = r;
    res.arg = phi;
    res._polar = true;

    return res;
}

// z = r(cos(phi) + i * sin(phi))
Complex cxFromPolar(double modulus, double argument)
{
    Complex res = cxNEW;

    res.mod = modulus;
    res.arg = argument;
    res._polar = true;

    res.real = modulus * cos(rad(argument));
    res.imag = modulus * sin(rad(argument));
    res._algebraic = true;

    return res;
}

typedef struct
{
    double d;
    char* s;
} _Pair;

// List of common numbers, by which its worth to divide a fraction, in order to represent it as an integer.
// ie 1.06066  == 3 * sqrt(2) * 1/4; therefore, by dividing 1.06066 by sqrt(2) and 1/4, we get the integer 3.
const _Pair base_divisors[] =
{
    { 1.4142135623730951, "sqrt(2)" },
    { 1.7320508075688772, "sqrt(3)" },
    //{ 2.2360679774997898, "sqrt(5)" },
    //{ 2.4494897427831779, "sqrt(6)" },
    //{ 2.6457513110645907, "sqrt(7)" },

    { 1.0 / 2, "1/2" },
    { 1.0 / 3, "1/3" },
    { 1.0 / 4, "1/4" },
    //{ 1.0 / 5, "1/5" },
    //{ 1.0 / 6, "1/6" },
    //{ 1.0 / 7, "1/7" },
    //{ 1.0 / 8, "1/8" },
    //{ 1.0 / 9, "1/9" },
    //{ 1.0 / 10, "1/10" },
};

// Size of base_divisors array:
#define base_divisors_n 5

// How many base divisors are applied at once. e.g. (1.06066 / (sqrt(2) * (1/4)) = 3) requires at least two base divisors to result in an integer (division by BOTH sqrt(2) AND 1/4). In fact, most numbers should be resolvable using just 2 such divisors.
#define extended_divisor_k 2

// Ways to pick k base divisors out of the whole pool (n):
#define extended_divisors_n 10

int extended_combinations[extended_divisors_n][extended_divisor_k];
int extended_combinations_i = 0; // The current combination being written.

// Divisors that consist of several base divisors. e.g. (sqrt(2) * (1/2)), (sqrt(3) * (1/4)),...
_Pair extended_divisors[extended_divisors_n];

// Fills the extended_combinations array with all possible combinations of the indexes [0..base_divisors_n)
// 'chosen' (a bitfield that keeps track of chosen numbers) and 'at' (which keeps track of the current number) are implementation details for recursion. Set both to 0 on first call.
void comb(const int n, const int k, unsigned long chosen, int at)
{
    if (n < k + at)
        return;

    if (!k)
    {
        int j = 0;
        for (at = 0; at < n; at++)
            if (chosen & (1 << at))
                extended_combinations[extended_combinations_i][j++] = at;
        extended_combinations_i++;
        return;
    }

    comb(n, k - 1, chosen | (1 << at), at + 1);
    comb(n, k, chosen, at + 1);
}

const double error_margin = 0.00001; // 10^-5

// Checks if x is roughly divisible by divisor (resulting in an integer).
// Returns the multiple (x / divisor) if possible; otherwise returns 0.
int checkDivisibility(double x, double divisor)
{
    assert(divisor != 0);
    double multiple = x / divisor;
    double rounded = round(multiple);

    if (fabs(multiple - rounded) < error_margin)
        return (int)round(multiple);
    else
        return 0;
}

// Returns a floating point number as a round multiple of other known numbers, if possible.
char* getKnownRepresentation(double x)
{
    char* repr = (char*)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
    
    double rounded = round(x);
    if (fabs(x - rounded) > error_margin) // Only look for a different representation if x isn't an integer:
    {
        // Go over all known base divisors:
        // If x is a round multiple of any of them, return that multiple.
        for (size_t i = 0; i != base_divisors_n; ++i)
        {
            int multiple = checkDivisibility(x, base_divisors[i].d);
            if (multiple)
            {
                int repr_len = sprintf(repr, "%d*%s", multiple, base_divisors[i].s);
                return realloc(repr, (size_t)repr_len + 1);
            }
        }

        // Go over all known extended divisors:
        // If x is a round multiple of any of them, return that multiple.
        for (size_t i = 0; i != extended_divisors_n; ++i)
        {
            int multiple = checkDivisibility(x, extended_divisors[i].d);
            if (multiple)
            {
                int repr_len = sprintf(repr, "%d*%s", multiple, extended_divisors[i].s);
                return realloc(repr, (size_t)repr_len + 1);
            }
        }
    }

    // No representation found, return as is:
    int repr_len = sprintf(repr, "%g", rounded);
    return realloc(repr, (size_t)repr_len + 1);
}

typedef enum
{
    Algebraic,
    Polar,
    AlgebraicAndPolar
} Print;

// z is simply formatted into the string as zx = a + bi; use 0 for no index.
char* cxAsString(Complex cx, Print print)
{
    // Algebraic form:
    char* algebraic = (char*)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);

    char sign = '+';
    if (cx.imag < 0)
    {
        sign = '-';
        cx.imag *= -1;
    }
    char* real_repr = getKnownRepresentation(cx.real);
    char* imag_repr = getKnownRepresentation(cx.imag);
    char* mod_repr = getKnownRepresentation(cx.mod);
    char* arg_repr = getKnownRepresentation(cx.arg);

    int len_a;
    if (cx.imag == 1)
        len_a = sprintf(algebraic, "z = %s %c i", real_repr, sign);
    else
        len_a = sprintf(algebraic, "z = %s %c %si", real_repr, sign, imag_repr);
    algebraic = realloc(algebraic, (size_t)len_a + 1);

    // Polar form:
    char* polar = (char*)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);

    int len_p = sprintf(polar, "z = %s(cos(%s) + isin(%s))", mod_repr, arg_repr, arg_repr);
    polar = realloc(polar, (size_t)len_p + 1);

    // Combined both forms:
    char* res = (char*)malloc(sizeof(char) * ((size_t)len_a + (size_t)len_p + DEFAULT_BUFFER_SIZE));
    int len;
    if (print == Algebraic)
        len = sprintf(res, "\tALGEB: %s", algebraic);
    else if (print == Polar)
        len = sprintf(res, "\tPOLAR: %s", polar);
    else if (print == AlgebraicAndPolar)
        len = sprintf(res, "\tALGEB: %s\n\tPOLAR: %s", algebraic, polar);

    free(algebraic);
    free(polar);
    return realloc(res, (size_t)len + 1);
}

void cxPrint(Complex cx, Print print)
{
    printf("%s\n", cxAsString(cx, print));
}

bool cxEquals(Complex cx1, Complex cx2)
{
    return ((cx1._algebraic && cx2._algebraic) && (cx1.real == cx2.real) && (cx1.imag == cx2.imag)) ||
           ((cx1._polar     && cx2._polar)     && (cx1.mod  == cx2.mod)  && (cx1.arg  == cx2.arg));
}

/*------------------------------------------------------------------*/
// Conjugates

Complex cxGetConjugate(Complex cx)
{
    assert(cx._algebraic);
    return cxFromAlgebraic(cx.real, -cx.imag);
}

// Returns the product of a complex number and its conjugate. |z|^2
double cxGetR2(Complex cx)
{
    assert(cx._algebraic);
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
    assert(cx1._algebraic && cx2._algebraic);
    return cxFromAlgebraic(cx1.real + cx2.real, cx1.imag + cx2.imag);
}

Complex cxSub(Complex cx1, Complex cx2)
{
    assert(cx1._algebraic && cx2._algebraic);
    return cxFromAlgebraic(cx1.real - cx2.real, cx1.imag - cx2.imag);
}

Complex cxMul(Complex cx1, Complex cx2)
{
    assert(cx1._polar && cx2._polar);
    return cxFromPolar(cx1.mod * cx2.mod, cx1.arg + cx2.arg);

    // Could also be used; discarded for simplicity:
    // return cxFromAlgebraic((cx1.real * cx2.real) - (cx1.imag * cx2.imag),
    //                        (cx1.real * cx2.imag) + (cx1.imag * cx2.real));
}

Complex cxDiv(Complex cx1, Complex cx2)
{
    assert(cx1._polar && cx2._polar);
    if (cx2.mod == 0)
    {
        printf("Division by 0. Returning 0.");
        return cxZERO;
    }
    return cxFromPolar(cx1.mod / cx2.mod, cx1.arg - cx2.arg);

    // Could also be used; discarded for simplicity:
    // Complex dividend = cxMul(cx1, cxGetConjugate(cx2));
    // double divisor = cxGetR2(cx2);
    // if (divisor == 0)
    // {
    //    printf("Divison by 0. Returning 0.");
    //    return cxZERO;
    // }
    // Complex res = { dividend.real / divisor, dividend.imag / divisor };
    // return res;
}

Complex cxPow(Complex cx1, double to)
{
    assert(cx1._polar);
    return cxFromPolar(pow(cx1.mod, to), cx1.arg * to);
}

/*------------------------------------------------------------------*/
// Everything together

void cxAnalyze(Complex cx)
{
    cxPrint(cx, AlgebraicAndPolar);
}

#define max_globals 100
Complex globals[max_globals];
int globals_i = 0;

bool interpret(char* str)
{
    // Trim whitespace:
    while (isspace((unsigned char) *str)) ++str;
    if (*str == 0)
        return true;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;
    end[1] = '\0';

    // Convert to lowercase for uniformity:
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);

    char* command = strtok(str, "(,)");
    char* arg1 = strtok(NULL, "(,)");
    char* arg2 = strtok(NULL, "(,)");

    if (strcmp(command, "exit") == 0)
    {
        return false;
    }
    else if (strcmp(command, "help") == 0)
    {
        printf("Complex number calculator.\nUsage:\n\thelp -> display this\n\texit -> exit\n\n\t[a]lgebraic(real, imag) -> add a new complex number from algebraic form\n\t[p]olar(mod, arg)       -> add a new complex number from polar form\n\n\tshow([a/p]) -> display all added numbers in desired form (displays both by default)\n\tclear       -> clear all added numbers\n\n\tadd(z1, z2) -> z1 + z2\n\tsub(z1, z2) -> z1 - z2\n\tmul(z1, z2) -> z1 * z2\n\tdiv(z1, z2) -> z1 / z2\n\tpow(z1, x)  -> z1^x");
    }
    else if (strcmp(command, "show") == 0)
    {
        for (size_t i = 0; i != globals_i; ++i)
        {
            printf("Z%d:", (int)i);

            if (arg1 != NULL)
            {
                if (arg1[0] == 'a')
                    cxPrint(globals[i], Algebraic);
                else
                    cxPrint(globals[i], Polar);
                continue;
            }
            cxPrint(globals[i], AlgebraicAndPolar);
        }
    }
    else if (strcmp(command, "clear") == 0)
    {
        globals_i = 0;
    }
    else if (command[0] == 'a' || command[0] == 'p' ||
             (strcmp(command, "add") == 0) || (strcmp(command, "sub") == 0) ||
             (strcmp(command, "mul") == 0) || (strcmp(command, "div") == 0) ||
             strcmp(command, "pow") == 0)
    {
        if (arg1 == NULL || arg2 == NULL)
        {
            printf("Not enough arguments! See 'help'.\n");
            return true;
        }

        if (globals_i == max_globals)
        {
            printf("Out of space. Use 'clear'.\n");
            return true;
        }

        Complex cx = cxNEW;

        // Calculate new complex number:
        if ((strcmp(command, "add") == 0) || (strcmp(command, "sub") == 0) ||
            (strcmp(command, "mul") == 0) || (strcmp(command, "div") == 0) ||
            (strcmp(command, "pow") == 0))
        {
            size_t arg1_i = atoi(arg1);
            size_t arg2_i = atoi(arg2);

            if ((arg1_i >= globals_i || (arg2_i >= globals_i && strcmp(command, "pow") != 0)))
            {
                
                printf("Invalid number indexing. Max index: %d", globals_i - 1);
                return true;
            }

            if (strcmp(command, "add") == 0)
                cx = cxAdd(globals[arg1_i], globals[arg2_i]);
            if (strcmp(command, "sub") == 0)
                cx = cxSub(globals[arg1_i], globals[arg2_i]);
            if (strcmp(command, "mul") == 0)
                cx = cxMul(globals[arg1_i], globals[arg2_i]);
            if (strcmp(command, "div") == 0)
                cx = cxDiv(globals[arg1_i], globals[arg2_i]);

            if (strcmp(command, "pow") == 0)
            {
                double arg2_f = atof(arg2);
                cx = cxPow(globals[arg1_i], arg2_f);
            }
        }
        // Add new complex number:
        else if (command[0] == 'a' || command[0] == 'p')
        {
            double arg1_f = atof(arg1);
            double arg2_f = atof(arg2);

            if (command[0] == 'a')
                cx = cxFromAlgebraic(arg1_f, arg2_f);
            else if (command[0] == 'p')
                cx = cxFromPolar(arg1_f, arg2_f);
        }

        printf("Z%d:%s\n", globals_i, cxAsString(cx, AlgebraicAndPolar));
        globals[globals_i++] = cx;
    }
    else
    {
        printf("Unknown command: '%s'. See 'help'.\n", command);
    }

    printf("\n");
    return true;
}

void cxRun()
{
    /*------------------------------------------------------------------*/
    // Initialize:

    // Generates extended_combinations array:
    comb(base_divisors_n, extended_divisor_k, 0, 0);

    // Generates extended_divisors array, 1:1 to extended_combinations:
    for (size_t i = 0; i != extended_divisors_n; ++i)
    {
        int* combination = extended_combinations[i];

        _Pair extended_divisor;
        extended_divisor.d = 1;
        extended_divisor.s = (char*)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
        strcpy(extended_divisor.s, "(");
        // For example: If the combination was [1, 2], then this would pick the 1st and 2nd base divisor, and combine them into a common "extended" divisor. i.e. sqrt(2) and 1/4 => (sqrt(2) * (1/4))
        for (size_t j = 0; j != extended_divisor_k; ++j)
        {
            extended_divisor.d *= base_divisors[combination[j]].d;
            strcat(extended_divisor.s, base_divisors[combination[j]].s);
            if (j != extended_divisor_k - 1)
                strcat(extended_divisor.s, "*");
        }
        strcat(extended_divisor.s, ")");
        extended_divisors[i] = extended_divisor;
    }

    /*------------------------------------------------------------------*/
    // Run:

    char help_command[] = "help";
    interpret(help_command);
    char* input = (char*)malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);
    while (true)
    {
        fgets(input, DEFAULT_BUFFER_SIZE, stdin);
        if (!interpret(input))
            break;
    }
}