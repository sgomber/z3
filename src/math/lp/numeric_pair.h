/*++
  Copyright (c) 2017 Microsoft Corporation

  Module Name:

  <name>

  Abstract:

  <abstract>

  Author:

  Lev Nachmanson (levnach)

  Revision History:


  --*/
#pragma once
#define lp_for_z3
#include <string>
#include <cmath>
#include <algorithm>
#ifdef lp_for_z3
#include "util/rational.h"
#include "util/z3_exception.h"
#else
// include "util/numerics/mpq.h"
// include "util/numerics/numeric_traits.h"
#endif
namespace lp {
#ifdef lp_for_z3 // rename rationals
typedef rational mpq;
#else
typedef lp::mpq mpq;
#endif


template <typename T>
std::string T_to_string(const T & t); // forward definition
#ifdef lp_for_z3
template <typename T> class numeric_traits {};

template <>  class numeric_traits<unsigned> {
public:
    static unsigned zero() { return 0; }
    static unsigned one() { return 1; }
    static bool is_zero(unsigned v) { return v == 0; }
    static double get_double(unsigned const & d) { return d; }
    static bool is_int(unsigned) {return true;}
    static bool is_pos(unsigned) {return true;}
};

template <>  class numeric_traits<int> {
public:
    static int zero() { return 0; }
    static int one() { return 1; }
    static bool is_zero(int v) { return v == 0; }
    static double get_double(int const & d) { return d; }
    static bool is_int(int) {return true;}
    static bool is_pos(int j) {return j > 0;}
    static bool is_neg(int j) {return j < 0;}
    static int ceil_ratio(int a, int b) { return static_cast<int>(ceil(mpq(a, b)).get_int32());}
    static int floor_ratio(int a, int b) { return static_cast<int>(floor(mpq(a, b)).get_int32());}
};


template <>  class numeric_traits<double> {
public:
    static double g_zero;
    static double const &zero() { return g_zero;  }
    static double g_one;
    static double const &one() { return g_one; }
    static bool is_zero(double v) { return v == 0.0; }
    static double const & get_double(double const & d) { return d;}
    static double log(double const & d) { NOT_IMPLEMENTED_YET(); return d;}
    static double from_string(std::string const & str) { return atof(str.c_str()); }
    static bool is_pos(const double & d) {return d > 0.0;}
    static bool is_neg(const double & d) {return d < 0.0;}
    static bool is_big(const double & d) { return false; }
};

template<>
class numeric_traits<rational> {
public:
    static rational const & zero() { return rational::zero(); }
    static rational const & one() { return rational::one(); }
    static bool is_zero(const rational & v) { return v.is_zero(); }
    static double get_double(const rational  & d) { return d.get_double();}
    static rational log(rational const& r) { UNREACHABLE(); return r; }
    static rational from_string(std::string const & str) { return rational(str.c_str()); }
    static bool is_pos(const rational & d) {return d.is_pos();}
    static bool is_neg(const rational & d) {return d.is_neg();}
    static bool is_int(const rational & d) {return d.is_int();}
    static bool is_big(const rational & d) {return d.is_big();}
    static mpq ceil_ratio(const mpq & a, const mpq & b) {
        return ceil(a / b);
    }
    static mpq floor_ratio(const mpq & a, const mpq & b) {
        return floor(a / b);
    }
};
#endif

template <typename X, typename Y>
struct convert_struct {
    static X convert(const Y & y){ return X(y);}
    static bool below_bound_numeric(const X &, const X &, const Y &) { /*UNREACHABLE();*/ return false;}
    static bool above_bound_numeric(const X &, const X &, const Y &) { /*UNREACHABLE();*/ return false; }
};



template <typename T>
struct numeric_pair {
    T x;
    T y;
    numeric_pair() = default;
    numeric_pair(T xp, T yp) : x(xp), y(yp) {}

    template <typename X>
    explicit numeric_pair(const X & n) : x(n), y(0) {
    }

    template <typename X, typename Y>
    numeric_pair(X xp, Y yp) : x(convert_struct<T, X>::convert(xp)), y(convert_struct<T, Y>::convert(yp)) {}

    unsigned hash() const { return combine_hash(x.hash(), y.hash()); }
    
    bool operator<(const T& a) const {
        return x < a || (x == a && y < 0);
    }

    bool operator>(const T& a) const {
        return x > a || (x == a && y > 0);
    }

    bool operator==(const T& a) const  {
        return a == x && y == 0;
    }

    bool operator!=(const T& a) const  {
        return !(*this == a);
    }

    bool operator<=(const T& a) const  {
        return *this < a || *this == a;
    }

    bool operator>=(const T& a) const  {
        return *this > a || *this == a;
    }

    bool operator<(const numeric_pair& a) const {
        return x < a.x || (x == a.x && y < a.y);
    }

    bool operator>(const numeric_pair& a) const {
        return x > a.x || (x == a.x && y > a.y);
    }

    bool operator==(const numeric_pair& a) const  {
        return a.x == x &&  a.y == y;
    }

    bool operator!=(const numeric_pair& a) const  {
        return !(*this == a);
    }

    bool operator<=(const numeric_pair& a) const  {
        return *this < a || *this == a;
    }

    bool operator>=(const numeric_pair& a) const  {
        return *this > a || a == *this;
    }

    numeric_pair operator*(const T & a) const {
        return numeric_pair(a * x, a * y);
    }

    numeric_pair operator/(const T & a) const {
        T a_as_T(a);
        return numeric_pair(x / a_as_T, y / a_as_T);
    }

    numeric_pair operator/(const numeric_pair &) const {
        // UNREACHABLE();
    }


    numeric_pair operator+(const numeric_pair & a) const  {
        return numeric_pair(a.x + x, a.y + y);
    }

    numeric_pair operator*(const numeric_pair & /*a*/) const  {
        // UNREACHABLE();
    }

    numeric_pair&  operator+=(const numeric_pair & a) {
        x += a.x;
        y += a.y;
        return *this;
    }

    numeric_pair&  operator-=(const numeric_pair & a) {
        x -= a.x;
        y -= a.y;
        return *this;
    }

    numeric_pair&  operator/=(const T & a) {
        x /= a;
        y /= a;
        return *this;
    }

    numeric_pair&  operator*=(const T & a) {
        x *= a;
        y *= a;
        return *this;
    }

    numeric_pair operator-(const numeric_pair & a) const {
        return numeric_pair(x - a.x, y - a.y);
    }

    numeric_pair operator-() const {
        return numeric_pair(-x, -y);
    }

    bool is_zero() const { return x.is_zero() && y.is_zero(); }

    bool is_pos() const { return x.is_pos() || (x.is_zero() && y.is_pos());}

    bool is_neg() const { return x.is_neg() || (x.is_zero() && y.is_neg());}

    void neg() { x.neg(); y.neg(); }
    
    std::string to_string() const {
        if (y.is_zero())
            return T_to_string(x);
        return std::string("(") + T_to_string(x) + ", "  + T_to_string(y) + ")";
    }

    bool is_int() const {
        return x.is_int() && y.is_zero();
    }
    
};


template <typename T>
std::ostream& operator<<(std::ostream& os, numeric_pair<T> const & obj) {
    os << obj.to_string();
    return os;
}

template <typename T, typename X>
numeric_pair<T> operator*(const X & a, const numeric_pair<T> & r) {
    return numeric_pair<T>(a * r.x, a * r.y);
}

template <typename T, typename X>
numeric_pair<T> operator*(const numeric_pair<T> & r, const X & a) {
    return numeric_pair<T>(a * r.x, a * r.y);
}


template <typename T, typename X>
numeric_pair<T> operator/(const numeric_pair<T> & r, const X & a) {
    return numeric_pair<T>(r.x / a,  r.y / a);
}

template <typename T> double get_double(const lp::numeric_pair<T> & ) { /* UNREACHABLE(); */ return 0;}
template <typename T>
class numeric_traits<lp::numeric_pair<T>> {
public:
    static lp::numeric_pair<T> zero() { return lp::numeric_pair<T>(numeric_traits<T>::zero(), numeric_traits<T>::zero()); }
    static bool is_zero(const lp::numeric_pair<T> & v) { return numeric_traits<T>::is_zero(v.x) && numeric_traits<T>::is_zero(v.y); }
    static double get_double(const lp::numeric_pair<T> & v){ return numeric_traits<T>::get_double(v.x); } // just return the double of the first coordinate
    static double one() { /*UNREACHABLE();*/ return 0;}
    static bool is_pos(const numeric_pair<T> &p) {
        return numeric_traits<T>::is_pos(p.x) ||
            (numeric_traits<T>::is_zero(p.x) && numeric_traits<T>::is_pos(p.y));
    }
    static bool is_neg(const numeric_pair<T> &p) {
        return numeric_traits<T>::is_neg(p.x) ||
            (numeric_traits<T>::is_zero(p.x) && numeric_traits<T>::is_neg(p.y));
    }
    static bool is_int(const numeric_pair<T> & p) {
        return numeric_traits<T>::is_int(p.x) && numeric_traits<T>::is_zero(p.y);
    }
};

typedef numeric_pair<mpq> impq;

template <typename X> bool below_bound_numeric(const X & x, const X & bound, const double& eps) { return convert_struct<X, double>::below_bound_numeric(x, bound, eps);}
template <typename X> bool above_bound_numeric(const X & x, const X & bound, const double& eps) { return convert_struct<X, double>::above_bound_numeric(x, bound, eps);}
template  <typename T>  T floor(const numeric_pair<T> & r) {
    if (r.x.is_int()) {
        if (r.y.is_nonneg()) {
            return r.x;
        }
        return r.x - mpq::one();
    }
    
    return floor(r.x);
}

template <typename T> T ceil(const numeric_pair<T> & r) {
    if (r.x.is_int()) {
        if (r.y.is_nonpos()) {
            return r.x;
        }
        return r.x + mpq::one();
    }
    
    return ceil(r.x);
}

}
