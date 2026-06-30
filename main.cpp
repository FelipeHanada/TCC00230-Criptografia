#include <iostream>
#include <random>
#include <set>
#include "core_mpz.hpp"
#include "core_cript.hpp"


int main() {
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
        auto m = grp.random();

        std::cout << "Grupo GL(n, Fp) (n = " << n << ", p = " << p << ")\n";
        el_gamal_demo(grp, g, a, b, m);
        std::cout << '\n';
    }

    return 0;
}
