#include <functional>

#ifndef H_CORE
#define H_CORE

template<typename T>
struct Group {
    T identity;
    std::function<T(const T&, const T&)> compose;
    std::function<T(const T&)> inverse;
    std::function<T()> random;
};

template<typename T>
struct Field {
    T add_identity;
    T mult_identity;
    std::function<T(const T&, const T&)> add;
    std::function<T(const T&, const T&)> mult;
    std::function<T(const T&)> add_inverse;
    std::function<T(const T&)> mult_inverse;
    std::function<T()> random;
};

template<typename T, typename U>
T binpow(const Group<T>& grp, T a, U b) {
    T ans = grp.identity;
    while (b > 0) {
        if ((b & 1) == 1) ans = grp.compose(ans, a);
        a = grp.compose(a, a);
        b >>= 1;
    }
    return ans;
}

template<typename T, int n>
struct Matrix {
    std::array<T, n*n> data;
    bool operator==(const Matrix<T, n>& other) const {
        for (int i=0; i<n*n; i++) {
            if (data[i] != other.data[i]) return 0;
        }
        return 1;
    }
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T, n>& matrix) {
        os << "[";
        for (int i=0; i<n; i++) {
            os << "[";
            for (int j=0; j<n; j++) {
                os << matrix.data[i*n+j] << ", ";
            }
            os << "], ";
        }
        os << "]";
        return os;
    }
};

template<typename T, int n>
Group<Matrix<T, n>> make_GL(Field<T> field) {
    Matrix<T, n> identity = { std::array<T, n*n>() };    
    for (int i=0; i<n*n; i++) {
        identity.data[i] = ((i/n) == (i%n) ? field.mult_identity : field.add_identity);
    }

    return {
        identity,
        [field](const Matrix<T, n>& a, const Matrix<T, n>& b) -> Matrix<T, n> {
            Matrix<T, n> ans;
            std::fill(ans.data.begin(), ans.data.end(), 0);
            for (int i=0; i<n; i++) {
                for (int k=0; k<n; k++) {
                    for (int j=0; j<n; j++) {
                        ans.data[i*n+j] = field.add(
                            ans.data[i*n+j],
                            field.mult(
                                a.data[i*n+k],
                                b.data[k*n+j]
                            )
                        );
                    }
                }
            }
            return ans;
        },
        [field, identity](const Matrix<T, n>& a) -> Matrix<T, n> {
            Matrix<T, n> tmp = a;
            Matrix<T, n> ans = identity;

            for (int i = 0; i < n; i++) {
                int pivot = i;
                while (pivot < n && tmp.data[pivot*n + i] == field.add_identity) {
                    pivot++;
                }
                if (pivot == n) {
                    throw std::runtime_error("Matrix is not invertible");
                }
                
                if (i != pivot) {
                    for (int j = 0; j < n; j++) {
                        std::swap(tmp.data[i*n + j], tmp.data[pivot*n + j]);
                        std::swap(ans.data[i*n + j], ans.data[pivot*n + j]);
                    }
                }
                
                T inv_pivot = field.mult_inverse(tmp.data[i*n + i]);
                for (int j = 0; j < n; j++) {
                    tmp.data[i*n + j] = field.mult(tmp.data[i*n + j], inv_pivot);
                    ans.data[i*n + j] = field.mult(ans.data[i*n + j], inv_pivot);
                }
                
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        T factor = tmp.data[j*n + i];
                        if (factor != field.add_identity) {
                            for (int k = 0; k < n; k++) {
                                tmp.data[j*n + k] = field.add(
                                    tmp.data[j*n + k],
                                    field.add_inverse(field.mult(factor, tmp.data[i*n + k]))
                                );
                                ans.data[j*n + k] = field.add(
                                    ans.data[j*n + k],
                                    field.add_inverse(field.mult(factor, ans.data[i*n + k]))
                                );
                            }
                        }
                    }
                }
            }
            return ans;
        },
        [field]() -> Matrix<T, n> {
            Matrix<T, n> ans;
            bool invertible;
            do {
                for (int i = 0; i < n * n; i++) {
                    ans.data[i] = field.random();
                }
                
                Matrix<T, n> tmp = ans;
                invertible = true;
                for (int i = 0; i < n; i++) {
                    int pivot = i;
                    while (pivot < n && tmp.data[pivot * n + i] == field.add_identity) {
                        pivot++;
                    }
                    if (pivot == n) {
                        invertible = false;
                        break;
                    }
                    if (i != pivot) {
                        for (int j = 0; j < n; j++) {
                            std::swap(tmp.data[i * n + j], tmp.data[pivot * n + j]);
                        }
                    }
                    T inv_pivot = field.mult_inverse(tmp.data[i * n + i]);
                    for (int j = i + 1; j < n; j++) {
                        T factor = tmp.data[j * n + i];
                        if (factor != field.add_identity) {
                            for (int k = i; k < n; k++) {
                                tmp.data[j * n + k] = field.add(
                                    tmp.data[j * n + k],
                                    field.add_inverse(field.mult(factor, tmp.data[i * n + k]))
                                );
                            }
                        }
                    }
                }
            } while (!invertible);

            return ans;
        }
    };
}

#endif
