#include <iomanip>
#include <iterator>
#include "core.hpp"

#ifndef H_CORE_CRIPT
#define H_CORE_CRIPT

template<typename T, typename U>
T dh_public_key(const Group<T>& grp, T g, U a) {
    return binpow(grp, g, a);
}

template<typename T, typename U>
T dh_secret_key(const Group<T>& grp, U a, const T& kB) {
    return binpow(grp, kB, a);
}

template<typename T, typename U>
void diffie_hellman_demo(const Group<T>& grp, const T& g, const U& a, const U& b) {
    T kA = dh_public_key(grp, g, a);
    T kB = dh_public_key(grp, g, b);
    T sA = dh_secret_key(grp, a, kB);
    T sB = dh_secret_key(grp, b, kA);

    std::cout << "[ DIFFIE HELLMAN DEMO ]\n";
    std::cout << std::setw(10) << "(public) " << "g = " << g << "\n";
    std::cout << std::setw(10) << "(Alice) " << "private key Alice: " << a << "\n";
    std::cout << std::setw(10) << "(Bob) " << "private key Bob: " << b << "\n";
    std::cout << std::setw(10) << "(public) " << "public key Alice: " << kA << "\n";
    std::cout << std::setw(10) << "(public) " << "public key Bob: " << kB << "\n";
    std::cout << std::setw(10) << "(Alice) " << "secret key Alice: " << sA << "\n";
    std::cout << std::setw(10) << "(Bob) " << "secret key Bob: " << sB << "\n";
    std::cout << "resultado: " << (sA == sB ? "SUCESSO" : "FRACASSO") << '\n';
}

template<typename T, typename U>
T el_gamal_public_key(const Group<T>& grp, const T& g, const U& x) {
    return binpow(grp, g, x);
}

template<typename T>
struct ElGamalEncryptedMessage {
    T c1, c2;
};

template<typename T, typename U>
ElGamalEncryptedMessage<T> el_gamal_encrypt(const Group<T>& grp, const T& g, const T& h, const U& y, const T& m) {
    T s = binpow(grp, h, y);
    return { binpow(grp, g, y), grp.compose(m, s) };
}

template<typename T, typename U>
T el_gamal_decrypt(const Group<T>& grp, ElGamalEncryptedMessage<T> e, const U& x) {
    T s = binpow(grp, e.c1, x);
    return grp.compose(e.c2, grp.inverse(s));
}

template<typename T, typename U>
void el_gamal_demo(const Group<T>& grp, const T& g, const U& a, const U& b, const T& m) {
    T kA = el_gamal_public_key(grp, g, a);
    T kB = el_gamal_public_key(grp, g, b);

    ElGamalEncryptedMessage e = el_gamal_encrypt(grp, g, kA, b, m);
    T d = el_gamal_decrypt(grp, e, a);

    std::cout << "[ EL GAMAL DEMO ]\n";
    std::cout << std::setw(10) << "(public) " << "g = " << g << "\n";
    std::cout << std::setw(10) << "(Alice) " << "private key Alice: " << a << "\n";
    std::cout << std::setw(10) << "(public) " << "public key Alice: " << kA << "\n";
    std::cout << std::setw(10) << "(Bob) " << "private key Bob: " << b << "\n";
    std::cout << std::setw(10) << "(public) " << "public key Bob: " << kB << "\n";
    std::cout << std::setw(10) << "(Alice) " << "message: " << m << '\n';
    std::cout << std::setw(10) << "(public) " << "encrypted c1: " << e.c1 << '\n';
    std::cout << std::setw(10) << "(public) " << "encrypted c2: " << e.c2 << '\n';
    std::cout << std::setw(10) << "(Bob) " << "decrypted: " << d << '\n';
    std::cout << "resultado: " << (m == d ? "SUCESSO" : "FRACASSO") << '\n';
}


template<typename T>
struct AAGPublicKey {
    int n;
    std::vector<T> base;
};

template<typename T>
struct AAGPrivateKey {
    int L;
    T transform;
    std::vector<int> i;
    std::vector<bool> inv;
};

template<typename T>
struct AAGTuple {
    int n;
    std::vector<T> t_base;
};

template<typename T>
AAGPublicKey<T> aag_random_public_key(const Group<T>& grp, int n) {
    AAGPublicKey<T> a = { n, std::vector<T>(n) };
    for (int i=0; i<n; i++) a.base[i] = grp.random();
    return a;
}

template<typename T, typename Generator>
AAGPrivateKey<T> aag_random_private_key(const Group<T>& grp, Generator& g, const AAGPublicKey<T>& a, int L) {
    AAGPrivateKey<T> A = { L, grp.identity, std::vector<int>(L), std::vector<bool>(L) };
    for (int j=0; j<L; j++) {
        int i = g()%a.n, e = g()&1;
        A.i[j] = i;
        A.inv[j] = e;
        A.transform = grp.compose(A.transform, e ? grp.inverse(a.base[i]) : a.base[i]);
    }
    return A;
}

template<typename T>
AAGTuple<T> aag_tuple(const Group<T>& grp, const AAGPrivateKey<T>& A, const AAGPublicKey<T>& b) {
    AAGTuple<T> tuple = { b.n, std::vector<T>(b.n) };
    for (int i=0; i<b.n; i++) {
        tuple.t_base[i] = grp.compose(grp.inverse(A.transform), grp.compose(b.base[i], A.transform));
    }
    return tuple;
}

template<typename T>
T agg_secret_key(const Group<T>& grp, const AAGPrivateKey<T>& A, const AAGTuple<T>& b_tuple) {
    T s = grp.inverse(A.transform);
    for (int i=0; i<A.L; i++) {
        T x = A.inv[i] ? grp.inverse(b_tuple.t_base[A.i[i]]) : b_tuple.t_base[A.i[i]];
        s = grp.compose(s, x);
    }
    return s;
}

template<typename T>
T agg_secret_key_r(const Group<T>& grp, const AAGPrivateKey<T>& B, AAGTuple<T>& a_tuple) {
    T s = grp.identity;
    for (int i=B.L-1; i>=0; i--) {
        T x = B.inv[i] ? a_tuple.t_base[B.i[i]] : grp.inverse(a_tuple.t_base[B.i[i]]);
        s = grp.compose(s, x);
    }
    s = grp.compose(s, B.transform);
    return s;
}

template<typename T, typename Generator>
void anshel_anshel_goldfeld_demo(const Group<T>& grp, Generator& g, int n, int L) {
    AAGPublicKey a = aag_random_public_key(grp, n);
    AAGPublicKey b = aag_random_public_key(grp, n);

    AAGPrivateKey A = aag_random_private_key(grp, g, a, L);
    AAGPrivateKey B = aag_random_private_key(grp, g, b, L);

    AAGTuple a_tuple = aag_tuple(grp, A, b);
    AAGTuple b_tuple = aag_tuple(grp, B, a);

    T sA = agg_secret_key(grp, A, b_tuple);
    T sB = agg_secret_key_r(grp, B, a_tuple);

    std::cout << "[ ANSHEL ANSHEL GOLDFELD DEMO ]\n";

    std::cout << std::setw(10) << "(public) " << "public key Alice:\n";
    for (int i=0; i<n; i++) {
        std::cout << std::setw(10) << "> " << "a" << i << " = " << a.base[i] << '\n';
    }

    std::cout << std::setw(10) << "(public) " << "public key Bob:\n";
    for (int i=0; i<n; i++) {
        std::cout << std::setw(10) << "> " << "b" << i << " = " << b.base[i] << '\n';
    }

    std::cout << std::setw(10) << "(Alice) " << "private key Alice:\n";
    std::cout << std::setw(10) << "> " << "T = " << A.transform << '\n';
    for (int i=0; i<L; i++) {
        std::cout << std::setw(10) << "> ";
        std::cout << "i" << i << " = " << A.i[i] << ", ";
        std::cout << "e" << i << " = " << "-"[!A.inv[i]] << "1\n";
    }

    std::cout << std::setw(10) << "(Bob) " << "private key Bob:\n";
    std::cout << std::setw(10) << "> " << "T = " << B.transform << '\n';
    for (int i=0; i<L; i++) {
        std::cout << std::setw(10) << "> ";
        std::cout << "i" << i << " = " << B.i[i] << ", ";
        std::cout << "e" << i << " = " << "-"[!B.inv[i]] << "1\n";
    }

    std::cout << std::setw(10) << "(public) " << "tuple from Alice:\n";
    for (int i=0; i<n; i++) {
        std::cout << std::setw(10) << "> ";
        std::cout << "A^-1 b" << i << " A = " << a_tuple.t_base[i] << '\n';
    }

    std::cout << std::setw(10) << "(public) " << "tuple from Bob:\n";
    for (int i=0; i<n; i++) {
        std::cout << std::setw(10) << "> ";
        std::cout << "B^-1 a" << i << " B = " << b_tuple.t_base[i] << '\n';
    }

    std::cout << std::setw(10) << "(Alice) " << "secret key Alice: " << sA << '\n';
    std::cout << std::setw(10) << "(Bob) " << "secret key Bob: " << sB << '\n';


    std::cout << "Resultado: " << (sA == sB ? "SUCESSO" : "FRACASSO") << '\n';
}

#endif
