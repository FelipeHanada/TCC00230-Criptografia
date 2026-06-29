#include <gmpxx.h>
#include "core.hpp"

#ifndef H_CORE_MPZ
#define H_CORE_MPZ

Group<mpz_class> make_Zp_mpz(const mpz_class& p, gmp_randclass& rng) {
    return Group<mpz_class>{
        mpz_class(1),
        [p](const mpz_class& a, const mpz_class& b) -> mpz_class {
            return (a * b) % p; 
        },
        [p](const mpz_class& a) -> mpz_class {
            mpz_class ans;
            if (mpz_invert(ans.get_mpz_t(), a.get_mpz_t(), p.get_mpz_t()) == 0) {
                return mpz_class(0); 
            }
            return ans;
        },
        [p,& rng]() mutable -> mpz_class {
            return rng.get_z_range(p - 1) + 1;
        }
    };
}

Group<mpz_class> make_lamplighter(const unsigned long& n) {
    return Group<mpz_class>{
        mpz_class(0),
        [n](const mpz_class& a, const mpz_class& b) -> mpz_class {
            // xor of the first n bits
            // sum the rest of the bits and take mod m
            mpz_class mask = (mpz_class(1) << n) - 1;

            mpz_class stateA = a & mask, posA = a >> n;
            mpz_class stateB = b & mask, posB = b >> n;
            mpz_class state = stateA ^ stateB, pos = (posA + posB) % n;

            return state | (pos << n);
        },
        [n](const mpz_class& a) -> mpz_class {
            mpz_class mask = (mpz_class(1) << n) - 1;
            
            mpz_class state = a & mask, pos = a >> n;

            mpz_class inv_state = state;
            mpz_class inv_pos = (pos == 0) ? mpz_class(0) : (n - pos);

            return inv_state | (inv_pos << n);
        }
    };
}

Field<mpz_class> make_Fp_mpz(const mpz_class& p, gmp_randclass& rng) {
    return Field<mpz_class>{
        mpz_class(0),
        mpz_class(1),
        [p](const mpz_class& a, const mpz_class& b) -> mpz_class { return (a + b) % p; },
        [p](const mpz_class& a, const mpz_class& b) -> mpz_class { return (a * b) % p; },
        [p](const mpz_class& a) -> mpz_class { return p - (a % p); },
        [p](const mpz_class& a) -> mpz_class {
            mpz_class ans;
            if (mpz_invert(ans.get_mpz_t(), a.get_mpz_t(), p.get_mpz_t()) == 0) {
                return mpz_class(0); 
            }
            return ans;
        },
        [p,& rng]() mutable -> mpz_class {
            return rng.get_z_range(p - 1) + 1;
        }
    };
}

bool primality_fermat_mpz(const mpz_class& p, int n, gmp_randclass& rng) {
    if (cmp(p, 2) < 0) return false;
    if (p == 2 || p == 3) return true;
    if (p % 2 == 0) return false;

    Group<mpz_class> grp = make_Zp_mpz(p, rng);

    for (int i = 0; i < n; ++i) {
        mpz_class a = rng.get_z_range(p - 2) + 2; 
        if (binpow(grp, a, p) != a) return false;
    }
    
    return true;
}

mpz_class gen_random_prime_mpz(gmp_randclass& rng, unsigned int bits, int n_tests = 100) {
    mpz_class candidate;
    do {
        candidate = rng.get_z_bits(bits); 
        mpz_setbit(candidate.get_mpz_t(), bits - 1); 
        mpz_setbit(candidate.get_mpz_t(), 0);        
        
    } while (!primality_fermat_mpz(candidate, n_tests, rng));

    return candidate;
}

mpz_class eval_pol(const Field<mpz_class>& F, const std::vector<mpz_class>& a, const mpz_class& x) {
    mpz_class ans = F.add_identity;
    mpz_class xx = F.mult_identity;
    for (const mpz_class& aa : a) {
        ans = F.add(ans, F.mult(aa, xx));
        xx = F.mult(xx, x);
    }
    return ans;
}

std::set<std::pair<mpz_class, mpz_class>> ss_shamir_gen_keys(const Field<mpz_class>& F, const mpz_class& S, int n, int k) {
    std::vector<mpz_class> a(k);
    a[0] = S;
    for (int i = 1; i < k; i++) a[i] = F.random();

    std::set<std::pair<mpz_class, mpz_class>> keys;
    while (keys.size() < (size_t)n) {
        mpz_class x = F.random();
        mpz_class y = eval_pol(F, a, x);
        keys.insert(std::make_pair(x, y));
    }

    return keys;
}

mpz_class ss_shamir_interpol(const Field<mpz_class>& F, const std::set<std::pair<mpz_class, mpz_class>>& points, const mpz_class& x) {
    int k = points.size();
    
    std::vector<std::pair<mpz_class, mpz_class>> ps;
    for (const auto& point : points) ps.push_back(point);

    std::vector<mpz_class> l(k, F.mult_identity);
    for (int j = 0; j < k; j++) {
        for (int i = 0; i < k; i++) {
            if (i != j) {
                mpz_class num = F.add(x, F.add_inverse(ps[i].first));
                mpz_class den = F.add(ps[j].first, F.add_inverse(ps[i].first));
                l[j] = F.mult(l[j], F.mult(num, F.mult_inverse(den)));
            }
        }
    }

    mpz_class L = F.add_identity;
    for (int j = 0; j < k; j++) {
        L = F.add(L, F.mult(ps[j].second, l[j]));
    }

    return L;
}

#endif
