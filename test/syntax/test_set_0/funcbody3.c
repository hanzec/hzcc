
void print(int z);

void empty()
{
}

int alpha(int beta, int gamma, int delta)
{
    int epsilon, zeta, eta, theta, iota;
    int kappa;
    int lambda, mu, nu, xi, omicron, pi, rho, sigma, tau, upsilon;
    int phi, chi, psi, omega;

    char    a, b, c, d, e, f, g, h, i, j, k, l, m, n,
            o, p, q, r, s, t, u, v, w, x, y, z;

    float   A[50], B[50], C[50], D[50];

    int     E, F[50], G;
}

char error ( int a , int b )
;   /* <----- that's bad */
{
  int
  x
  ;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 25 near text {
//CHECK-NEXT:         Expected type name

