#ifndef _LINALG_H_
#define _LINALG_H_

#include <ostream>
#include <stdexcept>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>

namespace mygl
{
    template<typename T, size_t N>              class Vector;
    template<typename T, size_t M, size_t N>    class Matrix;
    template<typename T>                        class Quaternion;

    template <typename IntegralType>
    typename std::enable_if<std::is_integral<IntegralType>::value, bool>::type IsEqual(const IntegralType& a, const IntegralType& b)
    {
        return a == b;
    }

    template <typename FloatingType>
    typename std::enable_if<std::is_floating_point<FloatingType>::value, bool>::type IsEqual(const FloatingType& a, const FloatingType& b)
    {
        return std::fabs(a - b) < std::numeric_limits<FloatingType>::epsilon();
    }

    template<typename T, size_t N>
    class Vector
    {
    public:
        Vector() { _init_data(); }
        Vector(std::initializer_list<T> l);
        template<typename ...Args> Vector(Args... args) : Vector({args...}) {}
        Vector(const Vector& v);
        ~Vector() { delete[] a; }

        bool operator==(const Vector& v) const;

        Vector& operator=(const Vector& v);
        Vector& operator+=(const Vector& v);
        Vector& operator-=(const Vector& v);
        Vector& operator*=(T s);
        T operator*=(const Vector& v) const;
        Vector& operator/=(T s);

        T operator[](int index) const;
        T& operator[](int index);

        Vector Unit() const;

        T Magnitude() const;

        // Find the component (or scalar projection) of this vector onto v (ie. comp_v this)
        T Component(const Vector& v) const;

        // Find the vector projection of this vector onto v (ie. proj_v this)
        Vector Project(const Vector& v) const;

        // Reduce the dimension of this vector
        Vector<T, N - 1> Demote() const;

        size_t Dimensions() const { return N; }
    private:
        T* a;

        void _init_data();
    };

    template<typename T, size_t N>
    Vector<T, N>::Vector(std::initializer_list<T> l)
    {
        if (l.size() != N)
        {
            throw std::length_error("wrong number of arguments");
        }

        _init_data();

        int i = 0;

        for (auto elem : l)
        {
            a[i++] = elem;
        }
    }

    template<typename T, size_t N>
    Vector<T, N>::Vector(const Vector<T, N>& v)
    {
        _init_data();
        *this = v;
    }

    template<typename T, size_t N>
    bool Vector<T, N>::operator==(const Vector<T, N>& v) const
    {
        for (int i = 0; i < N; ++i)
        {
            if (!IsEqual<T>(a[i], v[i]))
            {
                return false;
            }
        }

        return true;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator=(const Vector<T, N>& v)
    {
        for (int i = 0; i < N; ++i)
        {
            a[i] = v[i];
        }

        return *this;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator+=(const Vector<T, N>& v)
    {
        for (int i = 0; i < N; ++i)
        {
            a[i] += v[i];
        }

        return *this;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator-=(const Vector<T, N>& v)
    {
        for (int i = 0; i < N; ++i)
        {
            a[i] -= v[i];
        }

        return *this;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator*=(T s)
    {
        for (int i = 0; i < N; ++i)
        {
            a[i] *= s;
        }

        return *this;
    }

    template<typename T, size_t N>
    T Vector<T, N>::operator*=(const Vector<T, N>& v) const
    {
        T total = 0;

        for (int i = 0; i < N; ++i)
        {
            total += a[i] * v[i];
        }

        return total;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator/=(T s)
    {
        if (IsEqual<T>(s, 0))
        {
            // throw std::logic_error("[vector] division by zero");
            return *this; // TODO how to deal with division by zero?
        }

        for (int i = 0; i < N; ++i)
        {
            a[i] /= s;
        }

        return *this;
    }

    template<typename T, size_t N>
    T Vector<T, N>::operator[](int index) const
    {
        if (index < 0 || index >= N) throw std::out_of_range("index is out of bounds");
        else return a[index];
    }

    template<typename T, size_t N>
    T& Vector<T, N>::operator[](int index)
    {
        if (index < 0 || index >= N) throw std::out_of_range("index is out of bounds");
        else return a[index];
    }

    /* TODO division by zero? */
    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::Unit() const
    {
        return *this / Magnitude();
    }

    template<typename T, size_t N>
    T Vector<T, N>::Magnitude() const
    {
        return std::sqrt(*this * *this);
    }

    template<typename T, size_t N>
    T Vector<T, N>::Component(const Vector<T, N>& v) const
    {
        return *this * v / v.Magnitude();
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::Project(const Vector<T, N>& v) const
    {
        return Component(v) * v.Unit();
    }

    template<typename T, size_t N>
    Vector<T, N - 1> Vector<T, N>::Demote() const
    {
        return Vector<T, N - 1>(a[0], a[1], a[2]);
    }

    template<typename T, size_t N>
    void Vector<T, N>::_init_data()
    {
        a = new T[N];

        for (int i = 0; i < N; ++i)
        {
            a[i] = 0;
        }
    }

    template<typename T, size_t N> Vector<T, N> operator+(const Vector<T, N>& v) { return v; }
    template<typename T, size_t N> Vector<T, N> operator-(const Vector<T, N>& v) { return Vector<T, N>() - v; }

    template<typename T, size_t N> Vector<T, N> operator+(Vector<T, N> l, const Vector<T, N>& r) { return l += r; }
    template<typename T, size_t N> Vector<T, N> operator-(Vector<T, N> l, const Vector<T, N>& r) { return l -= r; }

    template<typename T, typename U, size_t N> Vector<T, N> operator*(Vector<T, N> v, U s) { return v *= s; }
    template<typename T, typename U, size_t N> Vector<T, N> operator*(U s, Vector<T, N> v) { return v *= s; }
    template<typename T, size_t N> T operator*(Vector<T, N> a, Vector<T, N> b) { return a *= b; }

    template<typename T, typename U, size_t N> Vector<T, N> operator/(Vector<T, N> v, U s) { return v /= s; }

    template<typename T, size_t N>
    std::ostream& operator<<(std::ostream& os, const Vector<T, N>& v)
    {
        os << '[';

        for (int i = 0; i < N; ++i)
        {
            os << v[i] << ((i == N - 1) ? "]" : ", ");
        }

        return os;
    }

    using vec2i = Vector<int, 2>;
    using vec3i = Vector<int, 3>;
    using vec4i = Vector<int, 4>;
    using vec2f = Vector<float, 2>;
    using vec3f = Vector<float, 3>;
    using vec4f = Vector<float, 4>;
    using vec2d = Vector<double, 2>;
    using vec3d = Vector<double, 3>;
    using vec4d = Vector<double, 4>;

    template<typename T> const Vector<T, 3> CrossProduct(const Vector<T, 3>& a, const Vector<T, 3>& b);
    template<typename T> const Vector<T, 3> Rotate3D(const Vector<T, 3>& v, const Quaternion<T>& q); // Rotate this vector about arbitrary axis and angle. Note that q must be a unit quaternion.

    template<typename T>
    const Vector<T, 3> CrossProduct(const Vector<T, 3>& a, const Vector<T, 3>& b)
    {
        Vector<T, 3> c;

        c[0] = a[1] * b[2] - a[2] * b[1];
        c[1] = a[2] * b[0] - a[0] * b[2];
        c[2] = a[0] * b[1] - a[1] * b[0];

        return c;
    }

    template<typename T>
    const Vector<T, 3> Rotate3D(const Vector<T, 3>& v, const Quaternion<T>& q)
    {
        Quaternion<T> p(0, v); // pure quaternion for v

        p = q * p * q.Conjugate(); // conjugate and inverse are the same in this case

        return p.VectorComponent();
    }

    template<typename T, size_t M, size_t N>
    class Matrix {
    public:
        Matrix() { vecs_ = new Vector<T, N>[M]; }
        Matrix(const Vector<T, N> vecs[]);
        Matrix(std::initializer_list<std::initializer_list<T>> l);
        Matrix(const Matrix& m);
        ~Matrix() { delete[] vecs_; }

        Matrix& operator=(const Matrix& m);
        Matrix& operator+=(const Matrix& m);
        Matrix& operator-=(const Matrix& m);
        Matrix& operator*=(T c);
        Matrix& operator/=(T c);

        Vector<T, N> operator[](int row) const;
        Vector<T, N>& operator[](int row);

        Vector<T, M> operator*(const Vector<T, N>& b) const;
        template<size_t P> Matrix<T, M, P> operator*(const Matrix<T, N, P>& b) const;

        Matrix<T, N, M> Transpose() const;

        size_t Rows() const { return M; }
        size_t Columns() const { return N; }
    protected:
        Vector<T, N>* vecs_;
    };

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>::Matrix(const Vector<T, N> vecs[])
    {
        vecs_ = new Vector<T, N>[M];

        for (int i = 0; i < M; ++i)
        {
            this->vecs_[i] = vecs[i];
        }
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>::Matrix(std::initializer_list<std::initializer_list<T>> l)
    {
        if (l.size() != M)
        {
            throw std::out_of_range("row count does not match");
        }

        if (l.begin()->size() != N)
        {
            throw std::out_of_range("column count does not match");
        }

        vecs_ = new Vector<T, N>[M];

        int i = 0, j = 0;

        for (auto row : l)
        {
            for (auto elem : row)
            {
                vecs_[i][j++] = elem;
            }

            i++;
            j = 0;
        }
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>::Matrix(const Matrix<T, M, N>& m)
    {
        vecs_ = new Vector<T, N>[M];
        *this = m;
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>& Matrix<T, M, N>::operator=(const Matrix<T, M, N>& m)
    {
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                vecs_[i][j] = m[i][j];
            }
        }
        return *this;
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>& Matrix<T, M, N>::operator+=(const Matrix<T, M, N>& m)
    {
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                vecs_[i][j] += m[i][j];
            }
        }
        return *this;
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>& Matrix<T, M, N>::operator-=(const Matrix<T, M, N>& m)
    {
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                vecs_[i][j] -= m[i][j];
            }
        }
        return *this;
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>& Matrix<T, M, N>::operator*=(T c)
    {
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                vecs_[i][j] *= c;
            }
        }
        return *this;
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, M, N>& Matrix<T, M, N>::operator/=(T c)
    {
        if (IsEqual<T>(c, 0))
        {
            throw std::logic_error("[matrix] division by zero");
        }

        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                vecs_[i][j] /= c;
            }
        }
        return *this;
    }

    template<typename T, size_t M, size_t N>
    Vector<T, N> Matrix<T, M, N>::operator[](int row) const
    {
        if (row < 0 || row >= M)
        {
            throw std::out_of_range("const Matrix subscript out of bounds");
        }
        return vecs_[row];
    }

    template<typename T, size_t M, size_t N>
    Vector<T, N>& Matrix<T, M, N>::operator[](int row)
    {
        if (row < 0 || row >= M)
        {
            throw std::out_of_range("const Matrix subscript out of bounds");
        }
        return vecs_[row];
    }

    template<typename T, size_t M, size_t N>
    Vector<T, M> Matrix<T, M, N>::operator*(const Vector<T, N>& b) const
    {
        Vector<T, M> c;

        for (int i = 0; i < M; ++i)
        {
            c[i] = vecs_[i] * b;
        }

        return c;
    }

    template<typename T, size_t M, size_t N>
    template <size_t P>
    Matrix<T, M, P> Matrix<T, M, N>::operator*(const Matrix<T, N, P>& b) const
    {
        Matrix<T, M, P> c;

        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < P; ++j)
            {
                for (int k = 0; k < N; ++k)
                {
                    c[i][j] += vecs_[i][k] * b[k][j];
                }
            }
        }

        return c;
    }

    template<typename T, size_t M, size_t N>
    Matrix<T, N, M> Matrix<T, M, N>::Transpose() const
    {
        Matrix<T, N, M> t;

        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                t[j][i] = vecs_[i][j];
            }
        }

        return t;
    }

    template<typename T, size_t M, size_t N> Matrix<T, M, N> operator+(const Matrix<T, M, N>& m) { return m; }
    template<typename T, size_t M, size_t N> Matrix<T, M, N> operator-(const Matrix<T, M, N>& m) { return Matrix<T, M, N>() - m; }

    template<typename T, size_t M, size_t N> Matrix<T, M, N> operator+(const Matrix<T, M, N>& a, const Matrix<T, M, N>& b) { return a += b; }
    template<typename T, size_t M, size_t N> Matrix<T, M, N> operator-(const Matrix<T, M, N>& a, const Matrix<T, M, N>& b) { return a -= b; }

    template<typename T, typename U, size_t M, size_t N> Matrix<T, M, N> operator*(Matrix<T, M, N> m, U c) { return m *= c; }
    template<typename T, typename U, size_t M, size_t N> Matrix<T, M, N> operator*(U c, Matrix<T, M, N> m) { return m *= c; }

    template<typename T, typename U, size_t M, size_t N> Matrix<T, M, N> operator/(Matrix<T, M, N> m, U c) { return m /= c; }

    template<typename T, size_t M, size_t N>
    std::ostream& operator<<(std::ostream& os, const Matrix<T, M, N>& m)
    {
        os << '[';

        for (int i = 0; i < M; ++i)
        {
            os << m[i] << ((i == M - 1) ? "]" : ", ");
        }

        return os;
    }

    template<typename T, size_t N>
    using SquareMatrix = Matrix<T, N, N>;

    using mat2i = SquareMatrix<int, 2>;
    using mat2f = SquareMatrix<float, 2>;
    using mat2d = SquareMatrix<double, 2>;
    using mat3i = SquareMatrix<int, 3>;
    using mat3f = SquareMatrix<float, 3>;
    using mat3d = SquareMatrix<double, 3>;
    using mat4i = SquareMatrix<int, 4>;
    using mat4f = SquareMatrix<float, 4>;
    using mat4d = SquareMatrix<double, 4>;

    template<typename T, size_t N> const SquareMatrix<T, N> CreateIdentity();

    template<typename T> const SquareMatrix<T, 2> CreateScalingMatrix2(T scaleX, T scaleY);
    template<typename T> const SquareMatrix<T, 2> CreateRotationMatrix2(T angle);

    template<typename T> const SquareMatrix<T, 3> CreateScalingMatrix3(T scaleX, T scaleY, T scaleZ);

    template<typename T> const SquareMatrix<T, 3> CreateRotationXMatrix3(T angle);
    template<typename T> const SquareMatrix<T, 3> CreateRotationYMatrix3(T angle);
    template<typename T> const SquareMatrix<T, 3> CreateRotationZMatrix3(T angle);
    template<typename T> const SquareMatrix<T, 3> CreateRotationMatrix3(T yaw, T pitch, T roll);
    template<typename T> const SquareMatrix<T, 3> CreateRotationMatrix3(const Quaternion<T>& q); // q is a unit quaternion

    template<typename T> const SquareMatrix<T, 4> CreateTranslationMatrix4(T dx, T dy, T dz);
    template<typename T> const SquareMatrix<T, 4> CreateScalingMatrix4(T scaleX, T scaleY, T scaleZ);

    template<typename T> const SquareMatrix<T, 4> CreateOrthographic4(T left, T right, T bottom, T top, T near, T far);
    template<typename T> const SquareMatrix<T, 4> CreateViewingFrustum4(T left, T right, T bottom, T top, T near, T far);
    template<typename T> const SquareMatrix<T, 4> CreatePerspective4(T fovy, T aspect, T near, T far);

    template<typename T> const SquareMatrix<T, 4> CreateRotationXMatrix4(T angle);
    template<typename T> const SquareMatrix<T, 4> CreateRotationYMatrix4(T angle);
    template<typename T> const SquareMatrix<T, 4> CreateRotationZMatrix4(T angle);
    template<typename T> const SquareMatrix<T, 4> CreateRotationMatrix4(T yaw, T pitch, T roll);
    template<typename T> const SquareMatrix<T, 4> CreateRotationMatrix4(const Quaternion<T>& q); // q is a unit quaternion

    template<typename T> const SquareMatrix<T, 4> Inverse4(const SquareMatrix<T, 4>& m); // experimental

    // TODO impl inverse, gaussian elimination, solve systems of eqns, determinant, find eigenvalues and eigenvectors etc... Nice reference: https://ubcmath.github.io/MATH307/index.html

    template<typename T, size_t N>
    const SquareMatrix<T, N> CreateIdentity()
    {
        SquareMatrix<T, N> id;

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (i == j)
                {
                    id[i][j] = 1;
                }
            }
        }

        return id;
    }

    template<typename T>
    const SquareMatrix<T, 2> CreateScalingMatrix2(T scaleX, T scaleY)
    {
        SquareMatrix<T, 2> S;

        S[0][0] = scaleX;
        S[1][1] = scaleY;

        return S;
    }

    template<typename T>
    const SquareMatrix<T, 2> CreateRotationMatrix2(T angle)
    {
        SquareMatrix<T, 2> R = {{std::cos(angle), -std::sin(angle)},
                                {std::sin(angle),  std::cos(angle)}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 3> CreateScalingMatrix3(T scaleX, T scaleY, T scaleZ)
    {
        SquareMatrix<T, 3> S;

        S[0][0] = scaleX;
        S[1][1] = scaleY;
        S[2][2] = scaleZ;

        return S;
    }

    template<typename T>
    const SquareMatrix<T, 3> CreateRotationXMatrix3(T angle)
    {
        SquareMatrix<T, 3> R = {{1,               0,                0},
                                {0, std::cos(angle), -std::sin(angle)},
                                {0, std::sin(angle),  std::cos(angle)}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 3> CreateRotationYMatrix3(T angle)
    {
        SquareMatrix<T, 3> R = {{std::cos(angle),  0, std::sin(angle)},
                                {0,                1,               0},
                                {-std::sin(angle), 0, std::cos(angle)}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 3> CreateRotationZMatrix3(T angle)
    {
        SquareMatrix<T, 3> R = {{std::cos(angle), -std::sin(angle),  0},
                                {std::sin(angle),  std::cos(angle),  0},
                                {0,                              0,  1}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 3> CreateRotationMatrix3(T yaw, T pitch, T roll)
    {
        SquareMatrix<T, 3> R;

        R[0][0] = std::cos(yaw) * std::cos(pitch);
        R[0][1] = std::cos(yaw) * std::sin(pitch) * std::sin(roll) - std::sin(yaw) * std::cos(roll);
        R[0][2] = std::cos(yaw) * std::sin(pitch) * std::cos(roll) + std::sin(yaw) * std::sin(roll);

        R[1][0] = std::sin(yaw) * std::cos(pitch);
        R[1][1] = std::sin(yaw) * std::sin(pitch) * std::sin(roll) + std::cos(yaw) * std::cos(roll);
        R[1][2] = std::sin(yaw) * std::sin(pitch) * std::cos(roll) - std::cos(yaw) * std::sin(roll);

        R[2][0] = -std::sin(pitch);
        R[2][1] = std::cos(pitch) * std::sin(roll);
        R[2][2] = std::cos(pitch) * std::cos(roll);

        return R;
    }

    template<typename T>
    const SquareMatrix<T, 3> CreateRotationMatrix3(const Quaternion<T>& q)
    {
        SquareMatrix<T, 3> R;

        R[0][0] = 1 - 2 * q[2] * q[2] - 2 * q[3] * q[3];
        R[0][1] = 2 * q[1] * q[2] - 2 * q[0] * q[3];
        R[0][2] = 2 * q[1] * q[3] + 2 * q[0] * q[2];

        R[1][0] = 2 * q[1] * q[2] + 2 * q[0] * q[3];
        R[1][1] = 1 - 2 * q[1] * q[1] - 2 * q[3] * q[3];
        R[1][2] = 2 * q[2] * q[3] - 2 * q[0] * q[1];

        R[2][0] = 2 * q[1] * q[3] - 2 * q[0] * q[2];
        R[2][1] = 2 * q[2] * q[3] + 2 * q[0] * q[1];
        R[2][2] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];

        return R;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateTranslationMatrix4(T dx, T dy, T dz)
    {
        SquareMatrix<T, 4> Tr = CreateIdentity<T, 4>();

        Tr[0][3] = dx;
        Tr[1][3] = dy;
        Tr[2][3] = dz;

        return Tr;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateScalingMatrix4(T scaleX, T scaleY, T scaleZ)
    {
        SquareMatrix<T, 4> S;

        S[0][0] = scaleX;
        S[1][1] = scaleY;
        S[2][2] = scaleZ;
        S[3][3] = 1;

        return S;
    }

    // Check out: http://learnwebgl.brown37.net/08_projections/projections_ortho.html
    template<typename T>
    const SquareMatrix<T, 4> CreateOrthographic4(T left, T right, T bottom, T top, T near, T far)
    {
        if (IsEqual<T>(left, right) || IsEqual<T>(bottom, top) || IsEqual<T>(near, far))
        {
            throw std::invalid_argument("Possible division by zero");
        }

        SquareMatrix<T, 4> P;

        P[0][0] = 2 / (right - left);
        P[1][1] = 2 / (top - bottom);
        P[2][2] = -2 / (far - near);
        P[3][3] = 1;

        P[0][3] = -(right + left) / (right - left);
        P[1][3] = -(top + bottom) / (top - bottom);
        P[2][3] = -(far + near) / (far - near);

        return P;
    }

    // Check out: http://learnwebgl.brown37.net/08_projections/projections_perspective.html
    template<typename T>
    const SquareMatrix<T, 4> CreateViewingFrustum4(T left, T right, T bottom, T top, T near, T far)
    {
        if (IsEqual<T>(left, right) || IsEqual<T>(bottom, top) || IsEqual<T>(near, far))
        {
            throw std::invalid_argument("Possible division by zero");
        }

        SquareMatrix<T, 4> P;

        P[0][0] = 2 * near / (right - left);
        P[1][1] = 2 * near / (top - bottom);
        P[2][2] = -(far + near) / (far - near);
        P[3][2] = -1;

        P[0][2] = -near * (left + right) / (right - left);
        P[1][2] = -near * (bottom + top) / (top - bottom);
        P[2][3] = -2 * far * near / (far - near);

        return P;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreatePerspective4(T fovy, T aspect, T near, T far)
    {
        if (fovy <= 0 || fovy >= 180 || aspect <= 0 || near >= far || near <= 0)
        {
            throw std::invalid_argument("Bad arguments");
        }

        T half_fovy = fovy / 2;

        T top = near * std::tan(half_fovy);
        T bottom = -top;
        T right = top * aspect;
        T left = -right;

        return CreateViewingFrustum4<T>(left, right, bottom, top, near, far);
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateRotationXMatrix4(T angle)
    {
        SquareMatrix<T, 4> R = {{1,               0,                0, 0},
                                {0, std::cos(angle), -std::sin(angle), 0},
                                {0, std::sin(angle),  std::cos(angle), 0},
                                {0,               0,                0, 1}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateRotationYMatrix4(T angle)
    {
        SquareMatrix<T, 4> R = {{std::cos(angle),  0, std::sin(angle), 0},
                                {0,                1,               0, 0},
                                {-std::sin(angle), 0, std::cos(angle), 0},
                                {0,                0,               0, 1}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateRotationZMatrix4(T angle)
    {
        SquareMatrix<T, 4> R = {{std::cos(angle), -std::sin(angle),  0, 0},
                                {std::sin(angle),  std::cos(angle),  0, 0},
                                {0,                              0,  1, 0},
                                {0,                              0,  0, 1}};
        return R;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateRotationMatrix4(T yaw, T pitch, T roll)
    {
        SquareMatrix<T, 4> R;

        R[0][0] = std::cos(yaw) * std::cos(pitch);
        R[0][1] = std::cos(yaw) * std::sin(pitch) * std::sin(roll) - std::sin(yaw) * std::cos(roll);
        R[0][2] = std::cos(yaw) * std::sin(pitch) * std::cos(roll) + std::sin(yaw) * std::sin(roll);

        R[1][0] = std::sin(yaw) * std::cos(pitch);
        R[1][1] = std::sin(yaw) * std::sin(pitch) * std::sin(roll) + std::cos(yaw) * std::cos(roll);
        R[1][2] = std::sin(yaw) * std::sin(pitch) * std::cos(roll) - std::cos(yaw) * std::sin(roll);

        R[2][0] = -std::sin(pitch);
        R[2][1] = std::cos(pitch) * std::sin(roll);
        R[2][2] = std::cos(pitch) * std::cos(roll);

        R[3][3] = 1;

        return R;
    }

    template<typename T>
    const SquareMatrix<T, 4> CreateRotationMatrix4(const Quaternion<T>& q)
    {
        SquareMatrix<T, 4> R;

        R[0][0] = 1 - 2 * q[2] * q[2] - 2 * q[3] * q[3];
        R[0][1] = 2 * q[1] * q[2] - 2 * q[0] * q[3];
        R[0][2] = 2 * q[1] * q[3] + 2 * q[0] * q[2];

        R[1][0] = 2 * q[1] * q[2] + 2 * q[0] * q[3];
        R[1][1] = 1 - 2 * q[1] * q[1] - 2 * q[3] * q[3];
        R[1][2] = 2 * q[2] * q[3] - 2 * q[0] * q[1];

        R[2][0] = 2 * q[1] * q[3] - 2 * q[0] * q[2];
        R[2][1] = 2 * q[2] * q[3] + 2 * q[0] * q[1];
        R[2][2] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];

        R[3][3] = 1;

        return R;
    }

    // https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    template<typename T>
    const SquareMatrix<T, 4> Inverse4(const SquareMatrix<T, 4>& m)
    {
        SquareMatrix<T, 4> im;

        T A2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
        T A1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
        T A1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
        T A0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
        T A0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
        T A0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
        T A2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
        T A1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
        T A1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
        T A2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
        T A1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
        T A1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        T A0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
        T A0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
        T A0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
        T A0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
        T A0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
        T A0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

        T det = m[0][0] * ( m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223 )
              - m[0][1] * ( m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223 )
              + m[0][2] * ( m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123 )
              - m[0][3] * ( m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123 );

        if (IsEqual<T>(det, 0))
        {
            throw std::logic_error("Uninvertible matrix!");
        }

        det = 1 / det;

        im[0][0] = det *   ( m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223 );
        im[0][1] = det * - ( m[0][1] * A2323 - m[0][2] * A1323 + m[0][3] * A1223 );
        im[0][2] = det *   ( m[0][1] * A2313 - m[0][2] * A1313 + m[0][3] * A1213 );
        im[0][3] = det * - ( m[0][1] * A2312 - m[0][2] * A1312 + m[0][3] * A1212 );
        im[1][0] = det * - ( m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223 );
        im[1][1] = det *   ( m[0][0] * A2323 - m[0][2] * A0323 + m[0][3] * A0223 );
        im[1][2] = det * - ( m[0][0] * A2313 - m[0][2] * A0313 + m[0][3] * A0213 );
        im[1][3] = det *   ( m[0][0] * A2312 - m[0][2] * A0312 + m[0][3] * A0212 );
        im[2][0] = det *   ( m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123 );
        im[2][1] = det * - ( m[0][0] * A1323 - m[0][1] * A0323 + m[0][3] * A0123 );
        im[2][2] = det *   ( m[0][0] * A1313 - m[0][1] * A0313 + m[0][3] * A0113 );
        im[2][3] = det * - ( m[0][0] * A1312 - m[0][1] * A0312 + m[0][3] * A0112 );
        im[3][0] = det * - ( m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123 );
        im[3][1] = det *   ( m[0][0] * A1223 - m[0][1] * A0223 + m[0][2] * A0123 );
        im[3][2] = det * - ( m[0][0] * A1213 - m[0][1] * A0213 + m[0][2] * A0113 );
        im[3][3] = det *   ( m[0][0] * A1212 - m[0][1] * A0212 + m[0][2] * A0112 );

        return im;
    }

    template<typename T>
    class Quaternion
    {
    public:
        Quaternion(bool unit = false) : s_(unit) {}
        Quaternion(T s, const Vector<T, 3>& v) : s_(s), v_(v) {}
        Quaternion(const Vector<T, 3>& axis, T angle); // Create a unit quaternion from rotation axis and angle
        Quaternion(const Quaternion& q) { *this = q; }
        ~Quaternion() {}

        Quaternion& operator=(const Quaternion& q);
        Quaternion& operator+=(const Quaternion& q);
        Quaternion& operator-=(const Quaternion& q);
        Quaternion& operator*=(T r);
        Quaternion& operator*=(const Quaternion& q);
        Quaternion& operator/=(T r);
        Quaternion& operator/=(const Quaternion& q);

        T operator[](int index) const;
        T& operator[](int index);

        Quaternion Conjugate() const;
        Quaternion Inverse() const;
        Quaternion Unit() const;

        T InnerProduct(const Quaternion& q) const;
        T Magnitude() const;

        T ScalarComponent() const { return s_; }
        Vector<T, 3> VectorComponent() const { return v_; }
    private:
        T s_;
        Vector<T, 3> v_;
    };

    template<typename T>
    Quaternion<T>::Quaternion(const Vector<T, 3>& axis, T angle)
    {
        s_ = std::cos(angle / 2);
        v_ = axis.Unit() * std::sin(angle / 2);
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator=(const Quaternion<T>& q)
    {
        s_ = q.s_;
        v_ = q.v_;
        return *this;
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator+=(const Quaternion<T>& q)
    {
        s_ += q.s_;
        v_ += q.v_;
        return *this;
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator-=(const Quaternion<T>& q)
    {
        s_ -= q.s_;
        v_ -= q.v_;
        return *this;
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator*=(T r)
    {
        s_ *= r;
        v_ *= r;
        return *this;
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<T>& q)
    {
        T s = s_ * q.s_ - v_ * q.v_;
        Vector<T, 3> v = s_ * q.v_ + q.s_ * v_ + CrossProduct(v_, q.v_);

        s_ = s;
        v_ = v;

        return *this;
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator/=(T r)
    {
        if (r == 0)
        {
            throw std::logic_error("[quaternion] division by zero");
        }

        s_ /= r;
        v_ /= r;
        return *this;
    }

    template<typename T>
    Quaternion<T>& Quaternion<T>::operator/=(const Quaternion<T>& q)
    {
        return *this * Inverse();
    }

    template<typename T>
    T Quaternion<T>::operator[](int index) const
    {
        switch (index)
        {
        case 0: return s_;
        case 1: return v_[0];
        case 2: return v_[1];
        case 3: return v_[2];
        default: throw std::out_of_range("index is out of bounds");
        }
    }

    template<typename T>
    T& Quaternion<T>::operator[](int index)
    {
        switch (index)
        {
        case 0: return s_;
        case 1: return v_[0];
        case 2: return v_[1];
        case 3: return v_[2];
        default: throw std::out_of_range("index is out of bounds");
        }
    }

    template<typename T>
    Quaternion<T> Quaternion<T>::Conjugate() const
    {
        return Quaternion<T>(s_, -v_);
    }

    template<typename T>
    Quaternion<T> Quaternion<T>::Inverse() const
    {
        return Conjugate() / InnerProduct(*this);
    }

    template<typename T>
    Quaternion<T> Quaternion<T>::Unit() const
    {
        return *this / Magnitude();
    }

    template<typename T>
    T Quaternion<T>::InnerProduct(const Quaternion<T>& q) const
    {
        return s_ * q.s_ + v_ * q.v_;
    }

    template<typename T>
    T Quaternion<T>::Magnitude() const
    {
        return std::sqrt(InnerProduct(*this));
    }

    template<typename T> Quaternion<T> operator+(const Quaternion<T>& q) { return q; }
    template<typename T> Quaternion<T> operator-(const Quaternion<T>& q) { return Quaternion<T>() - q; }

    template<typename T> Quaternion<T> operator+(Quaternion<T> l, const Quaternion<T>& r) { return l += r; }
    template<typename T> Quaternion<T> operator-(Quaternion<T> l, const Quaternion<T>& r) { return l -= r; }
    template<typename T> Quaternion<T> operator*(Quaternion<T> l, const Quaternion<T>& r) { return l *= r; }

    template<typename T, typename U> Quaternion<T> operator*(Quaternion<T> q, U s) { return q *= s; }
    template<typename T, typename U> Quaternion<T> operator*(U s, Quaternion<T> q) { return q *= s; }

    template<typename T, typename U> Quaternion<T> operator/(Quaternion<T> q, U s) { return q /= s; }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Quaternion<T>& q)
    {
        os << '[' << q.ScalarComponent() << ", " << q.VectorComponent() << ']';
        return os;
    }
}

#endif /* _LINALG_H_ */