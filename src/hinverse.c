/* vines: R package for multivariate dependence modeling with vines
 * Copyright (C) 2010-2011 Yasser González-Fernández
 * Copyright (C) 2010-2011 Marta Soto
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>

#include "hinverse.h"


SEXP hinverseIndepCopula(SEXP U, SEXP V) {
    return U;
}

/* See Aas, K., Czado, C., Frigessi, A. & Bakken, H. Pair-copula constructions
 * of multiple dependence. Insurance Mathematics and Economics, 2009, Vol. 44,
 * pp. 182-198. for the expression for the Gaussian, Student's t, Clayton and
 * Gumbel copulas.
 */

SEXP hinverseNormalCopula(SEXP Rho, SEXP U, SEXP V) {
    double eps = R_pow(DOUBLE_EPS, 0.5);
    int n = LENGTH(U);
    double rho = asReal(Rho);
    double *u = REAL(U), *v = REAL(V);
    double vi, xi;
    double *x;
    SEXP X;

    u = REAL(U);
    v = REAL(V);
    PROTECT(X = allocVector(REALSXP, n));
    x = REAL(X);

    for (int i = 0; i < n; i++) {
        if (u[i] <= eps) {
            x[i] = eps;
        } else if (1 - u[i] <= eps) {
            x[i] = 1 - eps;
        } else {
            vi = (v[i] <= eps) ? eps : ((v[i] >= 1 - eps) ? 1 - eps : v[i]);
            xi = pnorm(qnorm(u[i], 0, 1, TRUE, FALSE) * sqrt(1 - rho * rho) +
                    rho * qnorm(vi, 0, 1, 1, 0), 0, 1, TRUE, FALSE);
            x[i] = (xi <= eps) ? eps : ((xi >= 1 - eps) ? 1 - eps : xi);
        }
    }

    UNPROTECT(1);

    return X;
}

SEXP hinverseTCopula(SEXP Rho, SEXP Df, SEXP U, SEXP V) {
    double eps = R_pow(DOUBLE_EPS, 0.5);
    int n = LENGTH(U);
    double rho = asReal(Rho);
    double df = asReal(Df);
    double *u = REAL(U), *v = REAL(V);
    double vi, xi, b2;
    double *x;
    SEXP X;

    u = REAL(U);
    v = REAL(V);
    PROTECT(X = allocVector(REALSXP, n));
    x = REAL(X);

    for (int i = 0; i < n; i++) {
        if (u[i] <= eps) {
            x[i] = eps;
        } else if (1 - u[i] <= eps) {
            x[i] = 1 - eps;
        } else {
            vi = (v[i] <= eps) ? eps : ((v[i] >= 1 - eps) ? 1 - eps : v[i]);
            b2 = qt(vi, df, TRUE, FALSE);
            xi = pt(qt(u[i], df + 1, TRUE, FALSE) *
                    sqrt(((df + b2 * b2) * (1 - rho * rho)) / (df + 1)) +
                    rho * b2, df, TRUE, FALSE);
            x[i] = (xi <= eps) ? eps : ((xi >= 1 - eps) ? 1 - eps : xi);
        }
    }

    UNPROTECT(1);

    return X;
}

SEXP hinverseClaytonCopula(SEXP Theta, SEXP U, SEXP V) {
    double eps = R_pow(DOUBLE_EPS, 0.15);
    int n = LENGTH(U);
    double theta = asReal(Theta);
    double *u = REAL(U), *v = REAL(V);
    double vi, xi;
    double *x;
    SEXP X;

    if (theta <= eps) {
        return U;
    }

    PROTECT(X = allocVector(REALSXP, n));
    x = REAL(X);

    for (int i = 0; i < n; i++) {
        if (u[i] <= eps) {
            x[i] = eps;
        } else if (1 - u[i] <= eps) {
            x[i] = 1 - eps;
        } else {
            vi = (v[i] <= eps) ? eps : ((v[i] >= 1 - eps) ? 1 - eps : v[i]);
            xi = R_pow(R_pow(u[i] * R_pow(vi, theta + 1), -theta/(theta+1)) +
                    1 - R_pow(vi, -theta), -1 / theta);
            x[i] = (xi <= eps) ? eps : ((xi >= 1 - eps) ? 1 - eps : xi);
        }
    }

    UNPROTECT(1);

    return X;
}
