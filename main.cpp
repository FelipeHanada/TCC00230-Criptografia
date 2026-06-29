#include <iostream>
#include <random>
#include <set>
#include "core_mpz.hpp"
#include "core_cript.hpp"


int main() {
    using namespace std;
    
    gmp_randclass rng(gmp_randinit_default);
    rng.seed(time(nullptr));

    mpz_class p = gen_random_prime_mpz(rng, 128);
    cout << "p = " << p << "\n\n";

    Field<mpz_class> F = make_Fp_mpz(p, rng);

    int op = 1;
    do {
        cout << "0: Sair\n";
        cout << "1: Gerar chaves\n";
        cout << "2: Revelar o segredo\n";
        cout << "3: Criar mais chaves\n";
        cin >> op;
    
        if (op == 1) {
            mpz_class S;
            int n, k = INT32_MAX;
            cout << "Segredo: "; cin >> S;
            cout << "n: "; cin >> n;
            do {
                cout << "k: "; cin >> k;
            } while (k > n);
            
            cout << "Chaves geradas:\n";
            set<pair<mpz_class, mpz_class>> keys = ss_shamir_gen_keys(F, S, n, k);
            for (const auto& key : keys) {
                cout << key.first << " " << key.second << "\n";
            }
        } else if (op == 2) {
            int k;
            cout << "k: "; cin >> k;
            cout << "Espera " << k << " chaves (x y):\n";

            set<pair<mpz_class, mpz_class>> keys;
            while (keys.size() < (size_t)k) {
                mpz_class x, y;
                cin >> x >> y;
                keys.insert(make_pair(x, y));
            }

            cout << "Segredo revelado: " << ss_shamir_interpol(F, keys, 0) << "\n";
        } else if (op == 3) {
            int k; cout << "k: "; cin >> k;
            int n; cout << "n de chaves novas: "; cin >> n;

            cout << "Espera " << k << " chaves (x y):\n";
            set<pair<mpz_class, mpz_class>> keys;
            while (keys.size() < (size_t)k) {
                mpz_class x, y;
                cin >> x >> y;
                keys.insert(make_pair(x, y));
            }

            cout << "Chaves geradas:\n";
            for (int i = 0; i < n; i++) {
                mpz_class x = F.random();
                mpz_class y = ss_shamir_interpol(F, keys, x);
                cout << x << " " << y << "\n";
            }
        }

        cout << "\n";
    } while (op);

    return 0;
}

int main2() {
    gmp_randclass rng(gmp_randinit_mt);
    rng.seed(time(NULL));

    // {
    //     unsigned int precision_bits = 256;
    //     mpz_class p = gen_random_prime_mpz(rng, precision_bits);
    //     Group<mpz_class> grp = make_Zp_mpz(p, rng);
    //     mpz_class g = rng.get_z_bits(precision_bits);
    //     mpz_class a = rng.get_z_bits(precision_bits - 1);
    //     mpz_class b = rng.get_z_bits(precision_bits - 1);
    //     std::cout << "Grupo Zp (p = " << p << ")\n";
    //     diffie_hellman_demo(grp, g, a, b);
    //     std::cout << '\n';
    // }

    // {
    //     unsigned int precision_bits = 256;
    //     mpz_class p = gen_random_prime_mpz(rng, precision_bits);
    //     Group<mpz_class> grp = make_Zp_mpz(p, rng);
    //     mpz_class g = rng.get_z_bits(precision_bits);
    //     mpz_class a = rng.get_z_bits(precision_bits - 1);
    //     mpz_class b = rng.get_z_bits(precision_bits - 1);
    //     mpz_class m = mpz_class("6505709861945056");
    //     std::cout << "Grupo Zp (p = " << p << ")\n";
    //     el_gamal_demo(grp, g, a, b, m);
    //     std::cout << '\n';
    // }

    // {
    //     unsigned int precision_bits = 256;
    //     unsigned int n = 224;
    //     Group grp = make_lamplighter(n);
    //     mpz_class g = rng.get_z_bits(precision_bits);
    //     mpz_class a = rng.get_z_bits(precision_bits - 1);
    //     mpz_class b = rng.get_z_bits(precision_bits - 1);
    //     std::cout << "Grupo Lamplighter (n = " << n << ")\n";
    //     diffie_hellman_demo(grp, g, a, b);
    //     std::cout << '\n';
    // }

    // {
    //     std::mt19937 mt;
    //     unsigned int precision_bits = 256;
    //     mpz_class p = gen_random_prime_mpz(rng, precision_bits);
    //     Group<mpz_class> grp = make_Zp_mpz(p, rng);
    //     std::cout << "Grupo Zp (p = " << p << ")\n";
    //     anshel_anshel_goldfeld_demo(grp, mt, 16, 8);
    //     std::cout << '\n';
    // }

    {
        std::mt19937 mt;
        unsigned int precision_bits = 16;
        mpz_class p = gen_random_prime_mpz(rng, precision_bits);
        Field<mpz_class> Fp = make_Fp_mpz(p, rng);
        Group grp = make_GL<mpz_class, 4>(Fp);

        std::cout << "Grupo GL(Fp) (p = " << p << ")\n";
        anshel_anshel_goldfeld_demo(grp, mt, 2, 3);
        std::cout << '\n';
    }

    {
        std::mt19937 mt;
        unsigned int precision_bits = 64;
        mpz_class p = gen_random_prime_mpz(rng, precision_bits);
        mpz_class a = gen_random_prime_mpz(rng, precision_bits - 1);
        mpz_class b = gen_random_prime_mpz(rng, precision_bits - 1);

        const int n = 2;
        Field<mpz_class> Fp = make_Fp_mpz(p, rng);
        Group grp = make_GL<mpz_class, n>(Fp);
        auto g = grp.random();

        std::cout << "Grupo GL(n, Fp) (n = " << n << ", p = " << p << ")\n";
        diffie_hellman_demo(grp, g, a, b);
        std::cout << '\n';
    }


    return 0;
}
