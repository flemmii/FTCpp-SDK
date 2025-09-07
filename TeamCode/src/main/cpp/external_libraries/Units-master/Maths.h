#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_

#include "Units.h"

#include <cmath>
#include <algorithm>

namespace maths
{
    template <typename T>
    inline T Min(const T &a, const T &b)
    {
        return std::min(a, b);
    }

    template <typename T1, typename T2, typename = StaticUtilities::EnableIf<units::IsEquivalent<T1, T2>>>
    inline T1 Min(const T1 &a, const T2 &b)
    {
        return Min(a, T1(b));
    }

    template <typename T>
    inline T Max(const T &a, const T &b)
    {
        return std::max(a, b);
    }

    template <typename T1, typename T2, typename = StaticUtilities::EnableIf<units::IsEquivalent<T1, T2>>>
    inline T1 Max(const T1 &a, const T2 &b)
    {
        return Max(a, T1(b));
    }

    template <typename T>
    inline T Pow(const T &v, double p)
    {
        return T(std::pow(v.Value(), p));
    }

    template <typename T, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<T>>>
    inline T Sqrt(const T &v)
    {
        return std::sqrt(v);
    }

    template <int PowNum, int PowDen = 1, typename T>
    inline units::Pow<T, PowNum, PowDen> Pow(const Unit<T> &v)
    {
        return pow<PowNum, PowDen>(v);
    }

    template <typename T>
    inline units::Sqrt<T> Sqrt(const Unit<T> &v)
    {
        return sqrt(v);
    }

    template <typename T>
    inline units::Pow<T, 1, 3> Cbrt(const Unit<T> &v)
    {
        return units::Pow<T, 1, 3>(std::cbrt(v.Value()));
    }

    inline units::GetValueType<Radian> Sin(const Radian &r)
    {
        return std::sin(r.Value());
    }

    inline Radian Arcsin(const Metre &op, const Metre &hypo)
    {
        return Radian(std::asin(op / hypo));
    }

    inline Radian Arcsin(units::GetValueType<Radian> ratio)
    {
        return Radian(std::asin(ratio));
    }

    inline units::GetValueType<Radian> Cos(const Radian &r)
    {
        return std::cos(r.Value());
    }

    inline Radian Arccos(const Metre &adj, const Metre &hypo)
    {
        return Radian(std::acos(adj / hypo));
    }

    inline Radian Arccos(units::GetValueType<Radian> ratio)
    {
        return Radian(std::acos(ratio));
    }

    inline units::GetValueType<Radian> Tan(const Radian &r)
    {
        return std::tan(r.Value());
    }

    inline Radian Arctan(units::GetValueType<Radian> ratio)
    {
        return Radian(std::atan(ratio));
    }

    template <typename T>
    inline Radian Arctan(const T &op, const T &adj)
    {
        return Radian(std::atan2(op.Value(), adj.Value()));
    }

    template <typename T>
    inline T Ceil(const T &v)
    {
        return std::ceil(v);
    }

    template <typename T>
    inline T Ceil(const Unit<T> &v)
    {
        return T(std::ceil(v.Value()));
    }

    template <typename T>
    inline T Floor(const T &v)
    {
        return std::floor(v);
    }

    template <typename T>
    inline T Floor(const Unit<T> &v)
    {
        return T(std::floor(v.Value()));
    }

    template <typename T>
    inline T Mod(const T &a, const T &b)
    {
        return T(std::fmod(a.Value(), b.Value()));
    }

    template <typename T>
    inline units::GetValueType<T> Sgn(const T &v)
    {
        return (v.Value() > 0) - (v.Value() < 0);
    }
};

#endif //_MATHHELPER_H_