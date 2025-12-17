// Compile : g++ -O2 binet.cpp -o binet `pkg-config --cflags --libs flint`
#include <iostream>
#include <string>
#include <flint/fmpz.h>
#include <flint/arb.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: fib <n>" << endl;
        return 1;
    }

    fmpz_t n;
    fmpz_init(n);
    if (fmpz_set_str(n, argv[1], 10) != 0) {
        cerr << "Erreur: n invalide" << endl;
        return 2;
    }

    slong prec = 35000;   // ~ 10 000 chiffres décimaux sûrs

    arb_t sqrt5, phi, psi, a, b, fib;
    arb_init(sqrt5);
    arb_init(phi);
    arb_init(psi);
    arb_init(a);
    arb_init(b);
    arb_init(fib);

    /* sqrt(5) */
    arb_set_ui(sqrt5, 5);
    arb_sqrt(sqrt5, sqrt5, prec);

    /* phi = (1 + sqrt(5)) / 2 */
    arb_add_ui(phi, sqrt5, 1, prec);
    arb_div_ui(phi, phi, 2, prec);

    /* psi = (1 - sqrt(5)) / 2 */
    arb_sub_ui(psi, sqrt5, 1, prec);
    arb_neg(psi, psi);
    arb_div_ui(psi, psi, 2, prec);

    /* a = phi^n */
    arb_pow_fmpz(a, phi, n, prec);

    /* b = psi^n */
    arb_pow_fmpz(b, psi, n, prec);

    /* fib = (a - b) / sqrt(5) */
    arb_sub(fib, a, b, prec);
    arb_div(fib, fib, sqrt5, prec);

    arb_t rad;
    arb_init(rad);
    arb_get_rad_arb(rad, fib);

    arb_t threshold;
    arb_init(threshold);
    arb_set_ui(threshold, 1);
    arb_mul_2exp_si(threshold, threshold, -1000);  // ~10^-300

    bool warn = arb_gt(rad, threshold);

    fmpz_t result;
    fmpz_init(result);

    arf_t mid;
    arf_init(mid);

    /* Milieu de l'intervalle */
    arf_set(mid, arb_midref(fib));

    /* Arrondi certifié */
    arf_get_fmpz(result, mid, ARF_RND_NEAR);

    cout << "Fibonacci(n) = ";
    fmpz_print(result);
    cout << endl;

    if (warn) {
        cout << "[AVERTISSEMENT] Incertitude > 10^-300" << endl;
    }

    fmpz_clear(n);
    fmpz_clear(result);

    arb_clear(sqrt5);
    arb_clear(phi);
    arb_clear(psi);
    arb_clear(a);
    arb_clear(b);
    arb_clear(fib);
    arb_clear(rad);
    arb_clear(threshold);
    arf_clear(mid);

    flint_cleanup();
    return warn ? 3 : 0;
}
