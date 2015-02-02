#include "common.h"

#include <algorithm>


/*
 * Quantile function.
 *
 * Returns quantile q of data samples pointed to by base.
 * nmemb is the number of samples. q is a real number from [0, 1].
 */
double
_quantile(double *base, size_t nmemb, double q)
{
    if (nmemb == 0 || !isfinite(q) || q < 0 || q > 1) {
        return NAN;
    }

    double m = (nmemb - 1)*q;
    size_t m1 = floor(m);
    size_t m2 = ceil(m);

    /* Safeguard. */
    if (m1 >= nmemb || m2 >= nmemb) {
        return NAN;
    }

    std::nth_element(base, base + m1, base + nmemb);

    if (m1 == m2) {
        return base[m1];
    } else {
        double v1 = base[m1];
        double v2 = *std::min_element(base + m2, base + nmemb);
        return v1*(m2 - m) + v2*(m - m1);
    }
}

/*
 * Discrete quantile function.
 *
 * Returns k-th smallest element of data samples pointed to by base.
 * nmemb is the number of samples. k is an integer between 0 and nmemb - 1
 * (incl.).
 */
double
_quantile_disc(double *base, size_t nmemb, size_t k)
{
    if (nmemb == 0 || k >= nmemb) {
        return NAN;
    }

    std::nth_element(base, base + k, base + nmemb);
    return base[k];
}
