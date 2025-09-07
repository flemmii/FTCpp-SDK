/*
Copyright (c) 2019 Vincent Ducharme

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wdeprecated-copy"

#ifndef _UNITS_H_
#define _UNITS_H_

#include <cmath>
#include <cstdint>
#include <type_traits>
#include <string>
#include <sstream>

using uint8 = std::uint8_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using int32 = std::int32_t;
using int64 = std::int64_t;

// Define BASE_VALUE_REPRESENTATION before the first include of Units.h to use another type for unit value representation.
// By default, use double value
#ifndef BASE_VALUE_REPRESENTATION
using BaseValueRepresentation = double;
#else
using BaseValueRepresentation = BASE_VALUE_REPRESENTATION;
#endif
////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////

// Base class for unitless value
template <typename T>
class Unitless;

// Base class for Unit
template <typename U>
class Unit;

// Class for unit in the form of U*(N/D)
template <typename U, int64 N, int64 D, typename UBase>
class ScaledUnit;

// Class for unit in the form of U^(N/D)
template <typename U, int32 N, int32 D, typename UBase>
class PoweredUnit;

// Class for unit in the form of U.V
template <typename U, typename V>
class ComposedUnit;

namespace StaticUtilities
{
	////////////////////////////////////////////
	// V strictement plus grand que T
	////////////////////////////////////////////
	template <uint32 V, uint32 T>
	struct GreaterThan
	{
		constexpr static bool Value = V > T;
	};

	////////////////////////////////////////////
	// V plus grand ou �gal � T
	////////////////////////////////////////////
	template <uint32 V, uint32 T>
	struct GreaterEqualThan
	{
		constexpr static bool Value = V >= T;
	};

	////////////////////////////////////////////
	// V strictement plus petit que T
	////////////////////////////////////////////
	template <uint32 V, uint32 T>
	struct LessThan
	{
		constexpr static bool Value = V < T;
	};

	////////////////////////////////////////////
	// V plus petit ou �gal � T
	////////////////////////////////////////////
	template <uint32 V, uint32 T>
	struct LessEqualThan
	{
		constexpr static bool Value = V <= T;
	};

	template <bool CONDLeft, bool CONDRight>
	struct And
	{
		constexpr static bool Value = CONDLeft && CONDRight;
	};

	template <bool CONDLeft, bool CONDRight>
	struct Or
	{
		constexpr static bool Value = CONDLeft || CONDRight;
	};

	template <bool COND>
	struct Not
	{
		constexpr static bool Value = !COND;
	};

	////////////////////////////////////////////
	// PGCD (Plus grand commun diviseur)
	////////////////////////////////////////////
	template <int32 M, int32 N>
	struct PGCD
	{
		constexpr static int32 Value = PGCD<N, M % N>::Value;
	};

	template <int32 N>
	struct PGCD<N, 0>
	{
		constexpr static int32 Value = N;
	};

	template <>
	struct PGCD<0, 0>
	{
		constexpr static int32 Value = 1;
	};

	////////////////////////////////////////////
	// Same Types
	////////////////////////////////////////////
	template <class T, class T1>
	struct IsSameTypes
	{
		constexpr static bool Value = false;
	};

	template <class T>
	struct IsSameTypes<T, T>
	{
		constexpr static bool Value = true;
	};

	namespace internals
	{
		////////////////////////////////////////////
		// Enable methods if a condition
		////////////////////////////////////////////
		template <bool COND, typename T = void>
		struct EnableIf
		{
			using Type = T;
		};

		template <typename T>
		struct EnableIf<false, T>
		{
		};
	}

	template <typename CONDType, typename T = void>
	using EnableIf = typename internals::EnableIf<CONDType::Value, T>::Type;

	////////////////////////////////////////////
	// Static If Else with Int
	////////////////////////////////////////////
	template <bool COND, int64 T, int64 F>
	struct IfElseValue;

	template <int64 T, int64 F>
	struct IfElseValue<true, T, F>
	{
		constexpr static int64 Value = T;
	};

	template <int64 T, int64 F>
	struct IfElseValue<false, T, F>
	{
		constexpr static int64 Value = F;
	};

	namespace internals
	{
		////////////////////////////////////////////
		// Static If Else with Types
		////////////////////////////////////////////
		template <bool COND, typename T, typename F>
		struct IfElseType;

		template <typename T, typename F>
		struct IfElseType<true, T, F>
		{
			using Type = T;
		};

		template <typename T, typename F>
		struct IfElseType<false, T, F>
		{
			using Type = F;
		};
	}

	template <bool COND, typename T, typename F>
	using IfElse = typename internals::IfElseType<COND, T, F>::Type;

	////////////////////////////////////////////
	// Test if a type is an arithmetic type
	////////////////////////////////////////////
	template <typename V>
	struct IsArithmetic
	{
		constexpr static bool Value = std::is_arithmetic<V>::value;
	};

	template <typename From, typename To>
	struct IsConvertible
	{
		constexpr static bool Value = std::is_convertible_v<From, To>;
	};

	template <typename... A>
	struct Every
	{
		constexpr static bool Value = true;
	};

	template <typename V, typename... A>
	struct Every<V, A...>
	{
		constexpr static bool Value = IfElseValue<V::Value, Every<A...>::Value, false>::Value;
	};
}

namespace units
{
	////////////////////////////////////////////
	// Traits for Unit type
	////////////////////////////////////////////
	template <typename U>
	struct IsUnit
	{
		constexpr static bool Value = false;
	};

	template <typename U, int64 SN, int64 SD, typename UBase>
	struct IsUnit<::ScaledUnit<U, SN, SD, UBase>>
	{
		constexpr static bool Value = true;
	};

	template <typename U, int32 PN, int32 PD, typename UBase>
	struct IsUnit<::PoweredUnit<U, PN, PD, UBase>>
	{
		constexpr static bool Value = true;
	};

	template <typename U, typename V>
	struct IsUnit<::ComposedUnit<U, V>>
	{
		constexpr static bool Value = true;
	};

	////////////////////////////////////////////
	// Trait for Unitless type
	////////////////////////////////////////////
	template <typename U>
	struct IsUnitless
	{
		constexpr static bool Value = ::StaticUtilities::IsArithmetic<U>::Value;
	};

	template <typename T>
	struct IsUnitless<::Unitless<T>>
	{
		constexpr static bool Value = true;
	};

	namespace internals
	{
		template <class T, class = void>
		struct IsValueType
		{
			constexpr static bool Value = false;
		};

		template <class T>
		struct IsValueType<T, std::void_t<typename T::ValueType>>
		{
			constexpr static bool Value = true;
		};

		////////////////////////////////////////////
		// Get Base Value type of a Unit
		////////////////////////////////////////////
		template <typename U>
		struct GetValueType
		{
			using Type = StaticUtilities::IfElse<IsValueType<U>::Value, typename U::ValueType, U>;
		};

		template <>
		struct GetValueType<::BaseValueRepresentation>
		{
			using Type = ::BaseValueRepresentation;
		};

		template <typename T>
		struct GetValueType<::Unitless<T>>
		{
			using Type = T;
		};

		template <typename SV, int64 SN, int64 SD, typename SUBase>
		struct GetValueType<::ScaledUnit<SV, SN, SD, SUBase>>
		{
			using Type = typename GetValueType<SV>::Type;
		};

		template <typename PV, int32 PN, int32 PD, typename PUBase>
		struct GetValueType<::PoweredUnit<PV, PN, PD, PUBase>>
		{
			using Type = typename GetValueType<PV>::Type;
		};

		template <typename U1, typename U2>
		struct GetValueType<::ComposedUnit<U1, U2>>
		{
			using Type = typename GetValueType<U1>::Type;
		};
	}

	////////////////////////////////////////////
	// Get Base Value type of a Unit
	////////////////////////////////////////////
	template <typename U>
	using GetValueType = typename internals::GetValueType<U>::Type;
}

////////////////////////////////////////////
// Unitless - Wrapper to a Type
////////////////////////////////////////////
template <typename T>
class Unitless final
{
public:
	using ValueType = T;

	constexpr Unitless(T val = T(0))
		: m_value(val)
	{
	}

	// Special case when Power of 0
	template <typename U, int32 D, typename UBase>
	constexpr Unitless(const PoweredUnit<U, 0, D, UBase> &u)
		: m_value(u.Value())
	{
	}

	///////////////////////////
	// Negation operators
	///////////////////////////
	constexpr Unitless<T> operator-() const
	{
		return Unitless<T>(-m_value);
	}

	///////////////////////////
	// Auto in(de)crement operators
	///////////////////////////
	constexpr Unitless<T> operator++(int)
	{
		Unitless<T> temp(m_value);
		++m_value;
		return temp;
	}

	constexpr Unitless<T> &operator++()
	{
		++m_value;
		return *this;
	}

	constexpr Unitless<T> operator--(int)
	{
		Unitless<T> temp(m_value);
		--m_value;
		return temp;
	}

	constexpr Unitless<T> &operator--()
	{
		--m_value;
		return *this;
	}

	constexpr Unitless<T> &operator+=(Unitless<T> u)
	{
		m_value += u.m_value;
		return *this;
	}

	constexpr Unitless<T> &operator-=(Unitless<T> u)
	{
		m_value -= u.m_value;
		return *this;
	}

	constexpr Unitless<T> &operator*=(Unitless<T> u)
	{
		m_value *= u.m_value;
		return *this;
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr Unitless<T> &operator*=(const F &u)
	{
		m_value *= u;
		return *this;
	}

	constexpr Unitless<T> &operator/=(Unitless<T> u)
	{
		m_value /= u.m_value;
		return *this;
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr Unitless<T> &operator/=(const F &u)
	{
		m_value /= u;
		return *this;
	}

	constexpr Unitless<T> operator+(const Unitless<T> &other) const
	{
		return Unitless<T>(m_value + other.m_value);
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr Unitless<T> operator+(const F &other)
	{
		return Unitless<T>(m_value + other);
	}

	constexpr Unitless<T> operator-(const Unitless<T> &other) const
	{
		return Unitless<T>(m_value - other.m_value);
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr Unitless<T> operator-(const F &other)
	{
		return Unitless<T>(m_value - other);
	}

	constexpr Unitless<T> operator/(const Unitless<T> &other) const
	{
		return Unitless<T>(m_value / other.m_value);
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr Unitless<T> operator/(const F &other)
	{
		return Unitless<T>(m_value / other);
	}

	constexpr Unitless<T> operator*(const Unitless<T> &other) const
	{
		return Unitless<T>(m_value * other.m_value);
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr Unitless<T> operator*(const F &other)
	{
		return Unitless<T>(m_value * other);
	}

	constexpr operator T() const
	{
		return m_value;
	}

	constexpr T Value() const
	{
		return m_value;
	}

	template <typename U>
	constexpr U ToUnit() const
	{
		return U(m_value);
	}

	constexpr Unitless<T> ToUnitless() const { return Unitless<T>(m_value); }

	template <typename S>
	friend S &operator>>(S &in, Unitless<T> &s)
	{
		return (in >> s.m_value);
	}

	template <typename S>
	friend S &operator<<(S &out, const Unitless<T> &s)
	{
		return (out << s.m_value);
	}

protected:
	T m_value;
};

// Arithmetic value add unitless
template <typename F, typename U, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
constexpr Unitless<U> operator+(const F &f, const Unitless<U> &u)
{
	return Unitless<U>(f + u.Value());
}

// Arithmetic value sub unitless
template <typename F, typename U, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
constexpr Unitless<U> operator-(const F &f, const Unitless<U> &u)
{
	return Unitless<U>(f - u.Value());
}

// Arithmetic value multiply unitless
template <typename F, typename U, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
constexpr Unitless<U> operator*(const F &f, const Unitless<U> &u)
{
	return u * f;
}

// Arithmetic value divide unitless
template <typename F, typename U, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
constexpr Unitless<U> operator/(const F &f, const Unitless<U> &u)
{
	return Unitless<U>(f / u.Value());
}

////////////////////////////////////////////
// Real value without units (Unitless<BaseValueRepresentation>)
////////////////////////////////////////////
using Real = Unitless<BaseValueRepresentation>;

////////////////////////////////////////////
// Unit
////////////////////////////////////////////
template <typename U>
class Unit
{
public:
	using ValueType = units::GetValueType<U>;

	constexpr Unit()
		: m_value()
	{
	}

	constexpr explicit Unit(ValueType val)
		: m_value(val)
	{
	}

	constexpr Unit(const Unit &) = default;

	///////////////////////////
	// Assignation operators
	///////////////////////////
	constexpr U &operator=(const Unit<U> &u)
	{
		m_value = static_cast<ValueType>(u.Value());

		return static_cast<U &>(*this);
	}

	///////////////////////////
	// Negation operators
	///////////////////////////
	constexpr U operator-() const
	{
		return U(-m_value);
	}

	///////////////////////////
	// Auto in(de)crement operators
	///////////////////////////
	constexpr U operator++(int)
	{
		U temp(m_value);
		++m_value;
		return temp;
	}

	constexpr U &operator++()
	{
		++m_value;
		return static_cast<U &>(*this);
	}

	constexpr U operator--(int)
	{
		U temp(m_value);
		--m_value;
		return temp;
	}

	constexpr U &operator--()
	{
		--m_value;
		return static_cast<U &>(*this);
	}

	///////////////////////////
	// Add/Sub operators
	///////////////////////////
	constexpr U operator-(const U &u) const
	{
		return U(m_value - u.m_value);
	}

	constexpr U operator+(const U &u) const
	{
		return U(m_value + u.m_value);
	}

	///////////////////////////
	// Multiplication operators
	///////////////////////////
	// Multiply by Scalar -> Unit
	constexpr U operator*(Unitless<ValueType> u) const
	{
		return U(m_value * u.Value());
	}

	// Multiply by an arithmetic value -> Unit
	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr U operator*(const F &u) const
	{
		return U(m_value * u);
	}

	///////////////////////////
	// Division operators
	///////////////////////////
	// Divide by same type -> Scalar
	constexpr Unitless<ValueType> operator/(const U &u) const
	{
		return Unitless<ValueType>(m_value / u.m_value);
	}

	// Divide by Scalar -> Unit
	constexpr U operator/(Unitless<ValueType> u) const
	{
		return U(m_value / u.Value());
	}

	// Divide by an arithmetic value -> Unit
	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr U operator/(const F &u) const
	{
		return U(m_value / u);
	}

	///////////////////////////
	// Add/Sub equal operator
	///////////////////////////
	constexpr U &operator+=(const U &u)
	{
		m_value += u.m_value;
		return static_cast<U &>(*this);
	}

	constexpr U &operator-=(const U &u)
	{
		m_value -= u.m_value;
		return static_cast<U &>(*this);
	}

	///////////////////////////
	// Multiply equal operator
	///////////////////////////
	constexpr U &operator*=(Unitless<ValueType> u)
	{
		m_value *= u.Value();
		return static_cast<U &>(*this);
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr U &operator*=(const F &u)
	{
		m_value *= u;
		return static_cast<U &>(*this);
	}

	///////////////////////////
	// Divide equal operator
	///////////////////////////
	constexpr U &operator/=(Unitless<ValueType> u)
	{
		m_value /= u.Value();
		return static_cast<U &>(*this);
	}

	template <typename F, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
	constexpr U &operator/=(const F &u)
	{
		m_value /= u;
		return static_cast<U &>(*this);
	}

	///////////////////////////
	// Logical operators
	///////////////////////////
	constexpr bool operator==(const U &u) const
	{
		return m_value == u.m_value;
	}

	constexpr bool operator!=(const U &u) const
	{
		return !(*this == u);
	}

	constexpr bool operator<(const U &u) const
	{
		return m_value < u.m_value;
	}

	constexpr bool operator>(const U &u) const
	{
		return m_value > u.m_value;
	}

	constexpr bool operator<=(const U &u) const
	{
		return !(*this > u);
	}

	constexpr bool operator>=(const U &u) const
	{
		return !(*this < u);
	}

	///////////////////////////
	// Other functions
	///////////////////////////
	constexpr ValueType Value() const { return m_value; }

	constexpr explicit operator ValueType() const
	{
		return m_value;
	}

	template <typename V>
	constexpr V ToUnit() const { return V(m_value); }

	constexpr Unitless<ValueType> ToUnitless() const { return Unitless<ValueType>(m_value); }

	template <typename S>
	friend S &operator>>(S &is, Unit<U> &u)
	{
		return (is >> u.m_value);
	}

protected:
	ValueType m_value;
};

// Scalar multiply unit
template <typename U, typename REP>
constexpr U operator*(Unitless<REP> d, const Unit<U> &u)
{
	return u * d;
}

// Arithmetic value multiply unit
template <typename F, typename U, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
constexpr U operator*(const F &f, const Unit<U> &u)
{
	return u * f;
}

////////////////////////////////////////////
// Internal utilities
////////////////////////////////////////////
namespace units
{
	////////////////////////////////////////////
	// Get the Scale of the Unit
	////////////////////////////////////////////
	template <class T>
	struct GetScale
	{
		constexpr static uint64 Num = 1;
		constexpr static uint64 Den = 1;
	};

	template <class U, int64 N, int64 D, typename UBase>
	struct GetScale<::ScaledUnit<U, N, D, UBase>>
	{
		constexpr static uint64 Num = N * GetScale<U>::Num;
		constexpr static uint64 Den = D * GetScale<U>::Den;
	};

	template <class U, int32 N, int32 D, typename UBase>
	struct GetScale<::PoweredUnit<U, N, D, UBase>>
	{
		constexpr static uint64 Num = GetScale<U>::Num;
		constexpr static uint64 Den = GetScale<U>::Den;
	};

	////////////////////////////////////////////
	// Get the Power of the Unit
	////////////////////////////////////////////
	template <class T>
	struct GetUnitPower
	{
		constexpr static int32 Num = 0;
		constexpr static int32 Den = 0;
	};

	template <class U, int32 N, int32 D, typename UBase>
	struct GetUnitPower<::PoweredUnit<U, N, D, UBase>>
	{
		constexpr static int32 Num = N + GetUnitPower<U>::Num;
		constexpr static int32 Den = D + GetUnitPower<U>::Den;
	};

	template <class T>
	struct GetPower
	{
		constexpr static int32 Num = 1;
		constexpr static int32 Den = 1;
	};

	template <class U, int32 N, int32 D, typename UBase>
	struct GetPower<::PoweredUnit<U, N, D, UBase>>
	{
		constexpr static int32 Num = N * GetPower<U>::Num;
		constexpr static int32 Den = D * GetPower<U>::Den;
	};

	namespace internals
	{
		////////////////////////////////////////////
		// Get Base Unit of a Unit
		////////////////////////////////////////////
		template <typename U>
		struct GetBaseUnit
		{
			using BaseUnit = U;
		};

		template <typename U>
		struct GetBaseUnit<::Unitless<U>>
		{
			using BaseUnit = ::Unitless<U>;
		};

		template <typename SV, int64 SN, int64 SD, typename SUBase>
		struct GetBaseUnit<::ScaledUnit<SV, SN, SD, SUBase>>
		{
			using BaseUnit = SUBase;
		};

		template <typename PV, int32 PN, int32 PD, typename PUBase>
		struct GetBaseUnit<::PoweredUnit<PV, PN, PD, PUBase>>
		{
			using BaseUnit = PUBase;
		};

		template <typename U1, typename U2>
		struct GetBaseUnit<::ComposedUnit<U1, U2>>
		{
			using BaseUnit = ::ComposedUnit<U1, U2>;
		};
	}

	template <typename T>
	using GetBaseUnit = typename ::units::internals::GetBaseUnit<T>::BaseUnit;

	namespace internals
	{
		////////////////////////////////////////////
		// Trait for same base unit
		////////////////////////////////////////////
		template <typename U, typename V>
		struct IsSameBaseUnit
		{
			constexpr static bool Value = ::StaticUtilities::IsSameTypes<::units::GetBaseUnit<U>, ::units::GetBaseUnit<V>>::Value;
		};

		////////////////////////////////////////////
		// Get the change in scale between two
		// scales
		////////////////////////////////////////////
		template <int64 SNFrom, int64 SDFrom, int64 SNTarget, int64 SDTarget, typename ValueType>
		struct GetScaleChange
		{
			constexpr static ValueType Get() { return (static_cast<ValueType>(SDTarget) * static_cast<ValueType>(SNFrom)) / (static_cast<ValueType>(SNTarget) * static_cast<ValueType>(SDFrom)); }
		};

		// Particular case when the same scale, do not make the division for nothing
		template <int64 N, int64 D, typename ValueType>
		struct GetScaleChange<N, D, N, D, ValueType>
		{
			constexpr static ValueType Get() { return ValueType(1); }
		};

		////////////////////////////////////////////
		// Get the change factor between two units
		// taking into account the power and the
		// scale
		////////////////////////////////////////////
		template <int64 SNTarget, int64 SDTarget, int64 SNFrom, int64 SDFrom, int32 PN, int32 PD, typename ValueType>
		struct GetChangeFactorBase
		{
			constexpr static ValueType GetFactor() { return std::pow(GetScaleChange<SNFrom, SDFrom, SNTarget, SDTarget, ValueType>::Get(), (PN / static_cast<ValueType>(PD))); }
		};

		// Particular case when Power of 1, do not make the pow function for nothing
		template <int64 SNTarget, int64 SDTarget, int64 SNFrom, int64 SDFrom, int32 F, typename ValueType>
		struct GetChangeFactorBase<SNTarget, SDTarget, SNFrom, SDFrom, F, F, ValueType>
		{
			constexpr static ValueType GetFactor() { return GetScaleChange<SNFrom, SDFrom, SNTarget, SDTarget, ValueType>::Get(); }
		};

		////////////////////////////////////////////
		// Change the value of a unit V to
		// another unit U
		////////////////////////////////////////////
		template <typename U, typename V>
		struct ChangeFactor
		{
			using UValueType = ::units::GetValueType<U>;
			using VValueType = ::units::GetValueType<V>;

			constexpr static UValueType GetFactor()
			{
				return GetChangeFactorBase<::units::GetScale<U>::Num, ::units::GetScale<U>::Den, ::units::GetScale<V>::Num, ::units::GetScale<V>::Den, ::units::GetPower<V>::Num, ::units::GetPower<V>::Den, UValueType>::GetFactor();
			}

			constexpr static UValueType GetValue(VValueType value)
			{
				return value * GetFactor();
			}
		};

		template <typename U>
		struct ChangeFactor<U, U>
		{
			using UValueType = ::units::GetValueType<U>;

			constexpr static UValueType GetFactor()
			{
				return UValueType(1);
			}

			constexpr static UValueType GetValue(UValueType value)
			{
				return value;
			}
		};

		////////////////////////////////////////////
		// Inverse Power result type
		////////////////////////////////////////////
		template <typename U>
		struct InversePower
		{
			using Type = ::PoweredUnit<U, -1, 1, ::units::GetBaseUnit<U>>;
		};

		template <typename T>
		struct InversePower<::Unitless<T>>
		{
			using Type = ::Unitless<T>;
		};

		template <typename V, int64 SN, int64 SD, typename UBase>
		struct InversePower<::ScaledUnit<V, SN, SD, UBase>>
		{
			using Type = ::PoweredUnit<::ScaledUnit<V, SN, SD, UBase>, -1, 1, UBase>;
		};

		template <typename V, int32 PN, int32 PD, typename UBase>
		struct InversePower<::PoweredUnit<V, PN, PD, UBase>>
		{
			using Type = ::PoweredUnit<V, -PN, PD, UBase>;
		};

		template <typename V, int32 PD, typename UBase>
		struct InversePower<::PoweredUnit<V, -1, PD, UBase>>
		{
			using Type = V;
		};

		template <typename U1, typename U2>
		struct InversePower<::ComposedUnit<U1, U2>>
		{
			using Type = ::ComposedUnit<typename InversePower<U1>::Type, typename InversePower<U2>::Type>;
		};

		////////////////////////////////////////////
		// Square root result type
		////////////////////////////////////////////
		template <typename U>
		struct SqrtTransform
		{
			using Type = ::PoweredUnit<U, 1, 2, ::units::GetBaseUnit<U>>;
		};

		template <typename T>
		struct SqrtTransform<::Unitless<T>>
		{
			using Type = ::Unitless<T>;
		};

		template <typename U, int32 N, int32 D, typename UBase>
		struct SqrtTransform<::PoweredUnit<U, N, D, UBase>>
		{
			constexpr static int32 RN = ::StaticUtilities::IfElseValue < N % (D * 2) == 0, N / (D * 2), N > ::Value;
			constexpr static int32 RD = ::StaticUtilities::IfElseValue < N % (D * 2) == 0, 1, D * 2 > ::Value;

			using Type = ::StaticUtilities::IfElse<RN == RD, U, ::PoweredUnit<U, RN, RD, UBase>>;
		};

		template <typename U1, typename U2>
		struct SqrtTransform<::ComposedUnit<U1, U2>>
		{
			using Type = ::ComposedUnit<typename SqrtTransform<U1>::Type, typename SqrtTransform<U2>::Type>;
		};

		////////////////////////////////////////////
		// Power result type
		////////////////////////////////////////////
		template <typename U, int32 NMult, int32 DMult>
		struct PowerTransform
		{
			constexpr static int32 RN = GetPower<U>::Num * NMult;
			constexpr static int32 RD = GetPower<U>::Den * DMult;

			using Type = ::PoweredUnit<U, RN / ::StaticUtilities::PGCD<RN, RD>::Value, RD / ::StaticUtilities::PGCD<RN, RD>::Value, ::units::GetBaseUnit<U>>;
		};

		template <typename T, int32 NMult, int32 DMult>
		struct PowerTransform<::Unitless<T>, NMult, DMult>
		{
			using Type = ::Unitless<T>;
		};

		template <typename U, int32 N, int32 D, typename UBase, int NMult, int DMult>
		struct PowerTransform<::PoweredUnit<U, N, D, UBase>, NMult, DMult>
		{
			constexpr static int32 RN = GetPower<::PoweredUnit<U, N, D, UBase>>::Num * NMult;
			constexpr static int32 RD = GetPower<::PoweredUnit<U, N, D, UBase>>::Den * DMult;

			using Type = ::PoweredUnit<U, RN / ::StaticUtilities::PGCD<RN, RD>::Value, RD / ::StaticUtilities::PGCD<RN, RD>::Value, UBase>;
		};

		template <typename U1, typename U2, int32 NMult, int32 DMult>
		struct PowerTransform<::ComposedUnit<U1, U2>, NMult, DMult>
		{
			using Type = ::ComposedUnit<typename PowerTransform<U1, NMult, DMult>::Type, typename PowerTransform<U2, NMult, DMult>::Type>;
		};

		template <typename U, int64 Num, int64 Den = 1>
		struct ScaleTransform
		{
			using Type = ::ScaledUnit<U, Num, Den, typename GetBaseUnit<U>::BaseUnit>;
		};
	}

	template <typename U, int32 N, int32 D = 1>
	using Pow = typename ::units::internals::PowerTransform<U, N, D>::Type;

	template <typename U>
	using Sqrt = typename ::units::internals::SqrtTransform<U>::Type;

	template <typename U>
	using InvPow = typename ::units::internals::InversePower<U>::Type;

	template <typename U, int64 N, int64 D = 1>
	using Scale = typename ::units::internals::ScaleTransform<U, N, D>::Type;

	namespace internals
	{
		////////////////////////////////////////////
		// Transform two units together
		////////////////////////////////////////////
		template <typename U, typename V>
		struct Transform;

		template <typename T, typename V>
		struct Transform<::Unitless<T>, V>
		{
			using UValueType = T;
			using ReturnTypeMultiply = V;
			using ReturnTypeDivide = typename InversePower<V>::Type;

			constexpr static bool Find = true;

			constexpr static T GetChangeFactor()
			{
				return T(1);
			}
		};

		template <typename U, int64 N, int64 D, typename UBase, typename V>
		struct Transform<::ScaledUnit<U, N, D, UBase>, V>
		{
			using UValueType = ::units::GetValueType<::ScaledUnit<U, N, D, UBase>>;

			// If the operation is valid, return the operator ReturnType,
			// Otherwise, return BaseType itself
			// Used for the construction of the main resulting type
			using ReturnTypeMultiply = typename ::ScaledUnit<U, N, D, UBase>::template OperatorResultType<V>::MultiplyType;
			using ReturnTypeDivide = typename ::ScaledUnit<U, N, D, UBase>::template OperatorResultType<V>::DivideType;

			// If the baseUnit of V is UBase, we find a match
			constexpr static bool VIsScalar = ::units::IsUnitless<V>::Value;
			constexpr static bool Find = VIsScalar || IsSameBaseUnit<V, UBase>::Value;

			constexpr static UValueType GetChangeFactor()
			{
				return ((Find && !VIsScalar) ? ChangeFactor<::ScaledUnit<U, N, D, UBase>, V>::GetFactor() : UValueType(1));
			}
		};

		template <typename U, int32 N, int32 D, typename UBase, typename V>
		struct Transform<::PoweredUnit<U, N, D, UBase>, V>
		{
			using UValueType = ::units::GetValueType<::PoweredUnit<U, N, D, UBase>>;

			// If the operation is valid, return the operator ReturnType,
			// Otherwise, return BaseType itself
			// Used for the construction of the main resulting type
			using ReturnTypeMultiply = typename ::PoweredUnit<U, N, D, UBase>::template OperatorResultType<V>::MultiplyType;
			using ReturnTypeDivide = typename ::PoweredUnit<U, N, D, UBase>::template OperatorResultType<V>::DivideType;

			// We find a match if the two Unit are of the same baseUnit type
			constexpr static bool VIsScalar = ::units::IsUnitless<V>::Value;
			constexpr static bool Find = VIsScalar || IsSameBaseUnit<V, UBase>::Value;

			constexpr static UValueType GetChangeFactor()
			{
				return ((Find && !VIsScalar) ? ChangeFactor<::PoweredUnit<U, N, D, UBase>, V>::GetFactor() : UValueType(1));
			}
		};

		// Will return ComposedUnit<U1, U2> minus V, or ComposedUnit<U1, U2>
		template <typename U1, typename U2, typename V>
		struct Transform<::ComposedUnit<U1, U2>, V>
		{
			using UValueType = ::units::GetValueType<::ComposedUnit<U1, U2>>;

			// Try to transform each unit of the ComposedUnit by the unit V
			using U1MultiplyType = typename Transform<U1, V>::ReturnTypeMultiply;
			using U2MultiplyType = typename Transform<U2, V>::ReturnTypeMultiply;

			using U1DivideType = typename Transform<U1, V>::ReturnTypeDivide;
			using U2DivideType = typename Transform<U2, V>::ReturnTypeDivide;

			constexpr static bool Find = Transform<U1, V>::Find || Transform<U2, V>::Find || ::StaticUtilities::IsSameTypes<::ComposedUnit<U1, U2>, V>::Value;

			// ResultType = ComposedUnit<TransformU1, TransformU2>;
			// If we find a match
			//    If (U1*V==Scalar) ResultType = U2
			//    elseif (U2*V==Scalar) ResultType = U1
			//    else ResultType = ComposedUnit<TransformU1, TransformU2>
			// Otherwise, ResultType = ComposedUnit<TransformU1, TransformU2>
			// If there is a match, the result should be the composition of each transform
			using MultiplyTypeNoScalar = ::ComposedUnit<U1MultiplyType, U2MultiplyType>;
			using MultiplyResultStep1 = ::StaticUtilities::IfElse<IsUnitless<U1MultiplyType>::Value, U2, MultiplyTypeNoScalar>;
			using MultiplyResultStep2 = ::StaticUtilities::IfElse<IsUnitless<U2MultiplyType>::Value, U1, MultiplyResultStep1>;

			using ReturnTypeMultiply = ::StaticUtilities::IfElse<Find, MultiplyResultStep2, ::ComposedUnit<U1, U2>>;

			using DivideTypeNoScalar = ::ComposedUnit<U1DivideType, U2DivideType>;
			using DivideResultStep1 = ::StaticUtilities::IfElse<IsUnitless<U1DivideType>::Value, U2, DivideTypeNoScalar>;
			using DivideResultStep2 = ::StaticUtilities::IfElse<IsUnitless<U2DivideType>::Value, U1, DivideResultStep1>;

			using ReturnTypeDivide = ::StaticUtilities::IfElse<Find, DivideResultStep2, ::ComposedUnit<U1, U2>>;

			constexpr static UValueType GetChangeFactor()
			{
				// GetChangeFactor return 1 if no match found
				return Transform<U1, V>::GetChangeFactor() * Transform<U2, V>::GetChangeFactor();
			}
		};

		template <typename U, typename V>
		struct TransformBase
		{
			using UValueType = ::units::GetValueType<U>;

			constexpr static bool FindMultiply = Transform<U, V>::Find;
			constexpr static bool FindDivide = Transform<U, V>::Find;

			// If we find a match, return the Transform result
			// Otherwise, composed the two units
			using ReturnTypeMultiply = ::StaticUtilities::IfElse<FindMultiply, typename Transform<U, V>::ReturnTypeMultiply, ::ComposedUnit<U, V>>;
			using ReturnTypeDivide = ::StaticUtilities::IfElse<FindDivide, typename Transform<U, V>::ReturnTypeDivide, ::ComposedUnit<U, typename InversePower<V>::Type>>;

			constexpr static UValueType GetChangeFactorMultiply()
			{
				return Transform<U, V>::GetChangeFactor();
			}

			constexpr static UValueType GetChangeFactorDivide()
			{
				return Transform<U, V>::GetChangeFactor();
			}
		};

		template <typename U, typename U1, typename U2>
		struct TransformBase<U, ::ComposedUnit<U1, U2>>
		{
			using UValueType = ::units::GetValueType<U>;

			using MultiplyType = TransformBase<typename TransformBase<U, U2>::ReturnTypeMultiply, U1>;
			using ReturnTypeMultiply = typename MultiplyType::ReturnTypeMultiply;

			using DivideType = TransformBase<typename TransformBase<U, U2>::ReturnTypeDivide, U1>;
			using ReturnTypeDivide = typename DivideType::ReturnTypeDivide;

			constexpr static bool FindMultiply = MultiplyType::FindMultiply;
			constexpr static bool FindDivide = DivideType::FindDivide;

			constexpr static UValueType GetChangeFactorMultiply()
			{
				return MultiplyType::GetChangeFactorMultiply() * TransformBase<U, U2>::GetChangeFactorMultiply();
			}

			constexpr static UValueType GetChangeFactorDivide()
			{
				return DivideType::GetChangeFactorDivide() * TransformBase<U, U2>::GetChangeFactorDivide();
			}
		};
	}

	////////////////////////////////////////////
	// Main utility struct to Transform two
	// units together
	////////////////////////////////////////////
	template <typename U, typename V>
	struct TransformUnit
	{
		using UValueType = ::units::GetValueType<U>;

		using MultiplyType = typename internals::TransformBase<U, V>::ReturnTypeMultiply;
		using DivideType = typename internals::TransformBase<U, V>::ReturnTypeDivide;

		constexpr static MultiplyType Multiply(const U &u, const V &v)
		{
			UValueType factor = internals::TransformBase<U, V>::GetChangeFactorMultiply();
			return MultiplyType(u.Value() * (v.Value() * factor));
		}

		constexpr static DivideType Divide(const U &u, const V &v)
		{
			UValueType factor = internals::TransformBase<U, V>::GetChangeFactorDivide();
			return DivideType(u.Value() / (v.Value() * factor));
		}
	};

	template <typename U>
	struct TransformUnit<U, U>
	{
		using MultiplyType = typename internals::TransformBase<U, U>::ReturnTypeMultiply;
		using DivideType = typename internals::TransformBase<U, U>::ReturnTypeDivide;

		constexpr static MultiplyType Multiply(const U &u, const U &v)
		{
			return MultiplyType(u.Value() * v.Value());
		}

		constexpr static DivideType Divide(const U &u, const U &v)
		{
			return DivideType(u.Value() / v.Value());
		}
	};

	template <typename U, typename V>
	using UnitOperator = TransformUnit<U, V>;

	template <typename U, typename V>
	using MutiplyResultType = typename TransformUnit<U, V>::MultiplyType;

	template <typename U, typename V>
	using DivideResultType = typename TransformUnit<U, V>::DivideType;

	namespace internals
	{
		////////////////////////////////////////////
		// To test if two Units are equivalent
		////////////////////////////////////////////
		template <typename U, typename V>
		struct IsTransformableTo
		{
			constexpr static bool TypeUnitless = false; // IsUnitless<U>::Value && IsUnitless<V>::Value;
			constexpr static bool Result = TypeUnitless || (IsSameBaseUnit<U, V>::Value && GetPower<U>::Num == GetPower<V>::Num && GetPower<U>::Den == GetPower<V>::Den);

			using UValueType = ::units::GetValueType<U>;

			constexpr static UValueType GetChangeFactor()
			{
				return TypeUnitless ? UValueType(1) : Result ? ChangeFactor<U, V>::GetFactor()
															 : UValueType(1);
			}
		};

		template <typename U, typename V>
		struct IsInSet
		{
			using UValueType = typename IsTransformableTo<U, V>::UValueType;

			constexpr static bool Result = IsTransformableTo<U, V>::Result;

			constexpr static UValueType GetChangeFactor()
			{
				return Result ? IsTransformableTo<U, V>::GetChangeFactor() : UValueType(1);
			}
		};

		template <typename U, typename V1, typename V2>
		struct IsInSet<U, ::ComposedUnit<V1, V2>>
		{
			using UValueType = ::units::GetValueType<U>;

			constexpr static bool Result = IsInSet<U, V1>::Result || IsInSet<U, V2>::Result;

			constexpr static UValueType GetChangeFactor()
			{
				return IsInSet<U, V1>::GetChangeFactor() * IsInSet<U, V2>::GetChangeFactor();
			}
		};

		template <typename U1, typename U2, typename V1, typename V2>
		struct IsInSet<::ComposedUnit<U1, U2>, ::ComposedUnit<V1, V2>>
		{
			using UValueType = ::units::GetValueType<::ComposedUnit<U1, U2>>;

			constexpr static bool Result = IsInSet<U1, ::ComposedUnit<V1, V2>>::Result && IsInSet<U2, ::ComposedUnit<V1, V2>>::Result;

			constexpr static UValueType GetChangeFactor()
			{
				return IsInSet<U1, ::ComposedUnit<V1, V2>>::GetChangeFactor() * IsInSet<U2, ::ComposedUnit<V1, V2>>::GetChangeFactor();
			}
		};
	}

	////////////////////////////////////////////
	// To test if two Units are equivalent
	////////////////////////////////////////////
	template <typename U, typename V>
	struct IsEquivalent
	{
		using UValueType = typename internals::IsTransformableTo<U, V>::UValueType;

		constexpr static bool Value = internals::IsTransformableTo<U, V>::Result;

		constexpr static UValueType GetFactor()
		{
			return Value ? internals::IsTransformableTo<U, V>::GetChangeFactor() : UValueType(1);
		}
	};

	template <typename U1, typename U2, typename V1, typename V2>
	struct IsEquivalent<::ComposedUnit<U1, U2>, ::ComposedUnit<V1, V2>>
	{
		using UValueType = ::units::GetValueType<::ComposedUnit<U1, U2>>;

		constexpr static bool Value = internals::IsInSet<::ComposedUnit<U1, U2>, ::ComposedUnit<V1, V2>>::Result && internals::IsInSet<::ComposedUnit<V1, V2>, ::ComposedUnit<U1, U2>>::Result;

		constexpr static UValueType GetFactor()
		{
			return Value ? internals::IsInSet<::ComposedUnit<U1, U2>, ::ComposedUnit<V1, V2>>::GetChangeFactor() : UValueType(1);
		}
	};
}

namespace units
{
	namespace outputs
	{
		////////////////////////////////////////////
		// Write the unit output
		////////////////////////////////////////////
		// General case
		template <typename Unit>
		struct outputUnit
		{
			template <typename S>
			// User addition of __attribute__((unused)) to silence clang warning
			static void Output(S &os __attribute__((unused)))
			{
			}
		};
		template <typename U, int64 Num, int64 Den>
		struct outputUnitScaledBase;

		// Particular cases for ScaledUnit
		template <typename U, int64 Num, int64 Den>
		struct outputUnitScaledBase
		{
			template <typename S>
			static void Output(S &os)
			{
				os << Num;
				if (Den != 1)
					os << '/' << Den;
				os << '*';
				outputUnit<U>::Output(os);
			}
		};

		// TODO : Check to transform these traits for autogenerated with macro when defining ScaleFactor later?
		template <typename U>
		struct outputUnitScaledBase<U, 1, 1>
		{
			template <typename S>
			static void Output(S &os)
			{
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 10>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'd';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 100>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'c';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 1000>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'm';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 1000000000>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'n';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 1000000000000>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'p';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 1000000000000000>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'f';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1, 1000000000000000000>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'a';
				outputUnit<U>::Output(os);
			}
		};

		template <typename U>
		struct outputUnitScaledBase<U, 1000, 1>
		{
			template <typename S>
			static void Output(S &os)
			{
				os << 'k';
				outputUnit<U>::Output(os);
			}
		};

		// General case for ScaledUnit
		template <typename U, int64 SN, int64 SD, typename UBase>
		struct outputUnit<::ScaledUnit<U, SN, SD, UBase>>
		{
			template <typename S>
			static void Output(S &os)
			{
				outputUnitScaledBase<GetBaseUnit<U>, GetScale<::ScaledUnit<U, SN, SD, UBase>>::Num, GetScale<::ScaledUnit<U, SN, SD, UBase>>::Den>::Output(os);
			}
		};

		// General case for PoweredUnit
		template <typename U, int32 PN, int32 PD, typename UBase>
		struct outputUnit<::PoweredUnit<U, PN, PD, UBase>>
		{
			template <typename S>
			static void Output(S &os)
			{
				outputUnit<U>::Output(os);
				if (PN != PD)
				{
					os << '^';
					if (PN % PD)
					{
						os << '(';
						os << PN << '/' << PD;
						os << ')';
					}
					else
					{
						os << PN;
					}
				}
			}
		};

		// General case for ComposedUnit
		template <typename U1, typename U2>
		struct outputUnit<::ComposedUnit<U1, U2>>
		{
			template <typename S>
			static void Output(S &os)
			{
				outputUnit<U1>::Output(os);
				os << '*';
				outputUnit<U2>::Output(os);
			}
		};

	}
}

///////////////////////////
// Scaled Unit
///////////////////////////
template <typename U, int64 N = 1, int64 D = 1, typename UBase = units::GetBaseUnit<U>>
class ScaledUnit final : public Unit<ScaledUnit<U, N, D, UBase>>
{
public:
	using ValueType = typename Unit<ScaledUnit<U, N, D, UBase>>::ValueType;

	///////////////////////////
	// Traits for Operators Result Type
	///////////////////////////
	template <typename T>
	struct OperatorResultType // Otherwise, handle by ComposedUnit
	{
		using MultiplyType = ScaledUnit<U, N, D, UBase>;
		using DivideType = ScaledUnit<U, N, D, UBase>;
	};

	template <typename V, int64 SN, int64 SD>
	struct OperatorResultType<ScaledUnit<V, SN, SD, UBase>> // Operator by a scale of the same base
	{
		using MultiplyType = PoweredUnit<ScaledUnit<U, N, D, UBase>, 2, 1, UBase>;
		using DivideType = Unitless<ValueType>;
	};

	template <typename V, int32 PN, int32 PD>
	struct OperatorResultType<PoweredUnit<V, PN, PD, UBase>> // Operator by Powered of Self
	{
		using MultiplyType = StaticUtilities::IfElse<PN + PD == 0, Unitless<ValueType>, PoweredUnit<ScaledUnit<U, N, D, UBase>, PN + PD, PD, UBase>>;
		using DivideType = StaticUtilities::IfElse<PD - PN == 0, Unitless<ValueType>, PoweredUnit<ScaledUnit<U, N, D, UBase>, PD - PN, PD, UBase>>;
	};

	///////////////////////////
	// Constructors
	///////////////////////////

	// From a double value
	constexpr explicit ScaledUnit(const ValueType &val = 0.0)
		: Unit<ScaledUnit<U, N, D, UBase>>(val)
	{
	}

	// From a ScaledUnit with the same baseUnit, different scale -> Scale it!
	template <typename V, int64 SN, int64 SD>
	constexpr ScaledUnit(const ScaledUnit<V, SN, SD, UBase> &u)
		: Unit<ScaledUnit<U, N, D, UBase>>(::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase>>::GetValue(u.Value()))
	{
	}

	// From a PoweredUnit (1/1) of a ScaledUnit same as this one -> Just the scale unit
	constexpr ScaledUnit(const PoweredUnit<ScaledUnit<U, N, D, UBase>, 1, 1, UBase> &u)
		: Unit<ScaledUnit<U, N, D, UBase>>(u.Value())
	{
	}

	// From a PoweredUnit (1/1) of a ScaledUnit with the same baseUnit, but not the same scale -> Scale it!
	template <typename V, int64 SN, int64 SD>
	constexpr ScaledUnit(const PoweredUnit<ScaledUnit<V, SN, SD, UBase>, 1, 1, UBase> &u)
		: Unit<ScaledUnit<U, N, D, UBase>>(::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase>>::GetValue(u.Value()))
	{
	}

	template <typename V1, typename V2, typename = StaticUtilities::EnableIf<StaticUtilities::IsSameTypes<UBase, ComposedUnit<V1, V2>>>>
	constexpr ScaledUnit(const ComposedUnit<V1, V2> &u)
		: Unit<ScaledUnit<U, N, D, UBase>>(::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ComposedUnit<V1, V2>>::GetValue(u.Value()))
	{
	}

	///////////////////////////
	// Assignation operators
	///////////////////////////
	// Assignation from the same Scaled Unit
	constexpr ScaledUnit<U, N, D, UBase> &operator=(const ScaledUnit<U, N, D, UBase> &u)
	{
		if (this == &u)
			return *this;

		Unit<ScaledUnit<U, N, D, UBase>>::m_value = u.Value();

		return *this;
	}

	// Assignation from a ScaledUnit with the same baseUnit, but not the same scale -> Scale it!
	template <typename V, int64 SN, int64 SD>
	constexpr ScaledUnit<U, N, D, UBase> &operator=(const ScaledUnit<V, SN, SD, UBase> &u)
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase>>;
		Unit<ScaledUnit<U, N, D, UBase>>::m_value = ChangeFactorType::GetValue(u.Value());
		return *this;
	}

	template <typename U1, typename U2, typename = StaticUtilities::EnableIf<StaticUtilities::IsSameTypes<UBase, ComposedUnit<U1, U2>>>>
	constexpr ScaledUnit<U, N, D, UBase> &operator=(const ComposedUnit<U1, U2> &u)
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ComposedUnit<U1, U2>>;
		Unit<ScaledUnit<U, N, D, UBase>>::m_value = ChangeFactorType::GetValue(u.Value());
		return *this;
	}

	///////////////////////////
	// Multiplication operators
	///////////////////////////
	// Make accessible operator* from base class (Unit<U>)
	using Unit<ScaledUnit<U, N, D, UBase>>::operator*;

	// Multiply by self -> PoweredUnit
	constexpr typename OperatorResultType<ScaledUnit<U, N, D, UBase>>::MultiplyType operator*(const ScaledUnit<U, N, D, UBase> &u) const
	{
		using ReturnType = typename OperatorResultType<ScaledUnit<U, N, D, UBase>>::MultiplyType;
		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() * u.Value());
	}

	// Multiply by same baseUnit, Scaled -> PoweredUnit
	template <typename V, int64 SN, int64 SD>
	constexpr typename OperatorResultType<ScaledUnit<V, SN, SD, UBase>>::MultiplyType operator*(const ScaledUnit<V, SN, SD, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase>>;
		using ReturnType = typename OperatorResultType<ScaledUnit<V, SN, SD, UBase>>::MultiplyType;

		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() * ChangeFactorType::GetValue(u.Value()));
	}

	// Multiply by PoweredUnit, same baseUnit, same Scaled -> PoweredUnit'
	template <int32 PN, int32 PD>
	constexpr typename OperatorResultType<PoweredUnit<ScaledUnit<U, N, D, UBase>, PN, PD, UBase>>::MultiplyType operator*(const PoweredUnit<ScaledUnit<U, N, D, UBase>, PN, PD, UBase> &u) const
	{
		using ReturnType = typename OperatorResultType<PoweredUnit<ScaledUnit<U, N, D, UBase>, PN, PD, UBase>>::MultiplyType;

		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() * u.Value());
	}

	// Multiply by PoweredUnit, same baseUnit, different Scaled -> PoweredUnit'
	template <typename V, int64 SN, int64 SD, int32 PN, int32 PD>
	constexpr typename OperatorResultType<PoweredUnit<ScaledUnit<V, SN, SD, UBase>, PN, PD, UBase>>::MultiplyType operator*(const PoweredUnit<ScaledUnit<V, SN, SD, UBase>, PN, PD, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, PoweredUnit<ScaledUnit<V, SN, SD, UBase>, PN, PD, UBase>>;
		using ReturnType = typename OperatorResultType<PoweredUnit<ScaledUnit<V, SN, SD, UBase>, PN, PD, UBase>>::MultiplyType;

		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() * ChangeFactorType::GetValue(u.Value()));
	}

	// Multiply by ScaledUnit, different baseUnit -> ComposedUnit
	template <typename V, int64 N2, int64 D2, typename UBase2>
	constexpr ::units::MutiplyResultType<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, N2, D2, UBase2>> operator*(const ScaledUnit<V, N2, D2, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, N2, D2, UBase2>>;
		return Op::Multiply(*this, u);
	}

	// Multiply by PoweredUnit, different unit -> ComposedUnit
	template <typename V, int32 N2, int32 D2, typename UBase2>
	constexpr ::units::MutiplyResultType<ScaledUnit<U, N, D, UBase>, PoweredUnit<V, N2, D2, UBase2>> operator*(const PoweredUnit<V, N2, D2, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<ScaledUnit<U, N, D, UBase>, PoweredUnit<V, N2, D2, UBase2>>;
		return Op::Multiply(*this, u);
	}

	// Multiply by ComposedUnit
	template <typename U1, typename U2>
	constexpr ::units::MutiplyResultType<ComposedUnit<U1, U2>, ScaledUnit<U, N, D, UBase>> operator*(const ComposedUnit<U1, U2> &u) const
	{
		using Op = ::units::UnitOperator<ComposedUnit<U1, U2>, ScaledUnit<U, N, D, UBase>>;
		return Op::Multiply(u, *this);
	}

	///////////////////////////
	// Division operators
	///////////////////////////
	// Make accessible operator/ from base class (Unit<U>)
	using Unit<ScaledUnit<U, N, D, UBase>>::operator/;

	// Divide by ScaledUnit, same baseUnit, different scale -> double
	template <typename V, int64 SN, int64 SD>
	constexpr typename OperatorResultType<ScaledUnit<V, SN, SD, UBase>>::DivideType operator/(const ScaledUnit<V, SN, SD, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase>>;
		using ReturnType = typename OperatorResultType<ScaledUnit<V, SN, SD, UBase>>::DivideType;

		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() / ChangeFactorType::GetValue(u.Value()));
	}

	// Divide by PoweredUnit, same baseUnit, different scale -> Powered ScaledUnit
	template <typename V, int32 PN, int32 PD>
	constexpr typename OperatorResultType<PoweredUnit<V, PN, PD, UBase>>::DivideType operator/(const PoweredUnit<V, PN, PD, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<ScaledUnit<U, N, D, UBase>, PoweredUnit<V, PN, PD, UBase>>;
		using ReturnType = typename OperatorResultType<PoweredUnit<V, PN, PD, UBase>>::DivideType;

		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() / ChangeFactorType::GetValue(u.Value()));
	}

	// Divide by PoweredUnit, same baseUnit, same scale -> Powered ScaledUnit
	template <int32 PN, int32 PD>
	constexpr typename OperatorResultType<PoweredUnit<ScaledUnit<U, N, D, UBase>, PN, PD, UBase>>::DivideType operator/(const PoweredUnit<ScaledUnit<U, N, D, UBase>, PN, PD, UBase> &u) const
	{
		using ReturnType = typename OperatorResultType<PoweredUnit<ScaledUnit<U, N, D, UBase>, PN, PD, UBase>>::DivideType;

		return ReturnType(Unit<ScaledUnit<U, N, D, UBase>>::Value() / u.Value());
	}

	// Divide by ScaledUnit, different baseUnit -> ComposedUnit and Powered -1 for other ScaledUnit
	template <typename V, int64 SN, int64 SD, typename UBase2>
	constexpr ::units::DivideResultType<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase2>> operator/(const ScaledUnit<V, SN, SD, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<ScaledUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase2>>;
		return Op::Divide(*this, u);
	}

	// Divide by ScaledUnit, different baseUnit -> ComposedUnit and Powered -1 for other ScaledUnit
	template <typename V, int32 PN, int32 PD, typename UBase2>
	constexpr ::units::DivideResultType<ScaledUnit<U, N, D, UBase>, PoweredUnit<V, PN, PD, UBase2>> operator/(const PoweredUnit<V, PN, PD, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<ScaledUnit<U, N, D, UBase>, PoweredUnit<V, PN, PD, UBase2>>;
		return Op::Divide(*this, u);
	}

	// Divide by a ComposedUnit -> ComposedUnit
	template <typename U1, typename U2>
	constexpr ::units::DivideResultType<ScaledUnit<U, N, D, UBase>, ComposedUnit<U1, U2>> operator/(const ComposedUnit<U1, U2> &u) const
	{
		using Op = ::units::UnitOperator<ScaledUnit<U, N, D, UBase>, ComposedUnit<U1, U2>>;
		return Op::Divide(*this, u);
	}
};

//////////////////////////////////
// Powered Unit
//////////////////////////////////
template <typename U, int32 N, int32 D = 1, typename UBase = units::GetBaseUnit<U>>
class PoweredUnit final : public Unit<PoweredUnit<U, N, D, UBase>>
{
public:
	using ValueType = typename Unit<PoweredUnit<U, N, D, UBase>>::ValueType;

	///////////////////////////
	// Traits for Operators Result Type
	///////////////////////////
	template <typename T>
	struct OperatorResultType // Otherwise, handle by the ComposedUnit
	{
		// Because we can't explicitly specialize the template into another template class, we must test if the T is the same as the U typename
		using MultiplyType = StaticUtilities::IfElse<StaticUtilities::IsSameTypes<T, U>::Value, StaticUtilities::IfElse<N + D == 0, Unitless<ValueType>, PoweredUnit<U, N + D, D, UBase>>, PoweredUnit<U, N, D, UBase>>;
		using DivideType = StaticUtilities::IfElse<StaticUtilities::IsSameTypes<T, U>::Value, StaticUtilities::IfElse<N - D == 1, U, StaticUtilities::IfElse<N - D == 0, Unitless<ValueType>, PoweredUnit<U, N - D, D, UBase>>>, PoweredUnit<U, N, D, UBase>>;
	};

	template <typename V, int64 SN, int64 SD>
	struct OperatorResultType<ScaledUnit<V, SN, SD, UBase>> // Operator by ScaledUnit
	{
		using MultiplyType = StaticUtilities::IfElse<N + D == 0, Unitless<ValueType>, PoweredUnit<U, N + D, D, UBase>>;
		using DivideType = StaticUtilities::IfElse<N - D == 1, U, StaticUtilities::IfElse<N - D == 0, Unitless<ValueType>, PoweredUnit<U, N - D, D, UBase>>>;
	};

	template <typename V, int32 PN, int32 PD>
	struct OperatorResultType<PoweredUnit<V, PN, PD, UBase>> // Operator by Powered of the same unit
	{
		constexpr static int32 PGCDMultiply = StaticUtilities::PGCD<(N * PD + D * PN), D * PD>::Value;
		constexpr static int32 PGCDDivide = StaticUtilities::PGCD<(N * PD - D * PN), D * PD>::Value;

		using MultiplyType = StaticUtilities::IfElse<(N * PD + D * PN) % (D * PD) == 0, StaticUtilities::IfElse<(N * PD) + (D * PN) == 0, Unitless<ValueType>, StaticUtilities::IfElse<(N * PD) + (PN * D) == (D * PD), U, PoweredUnit<U, ((N * PD) + (D * PN)) / (D * PD), 1, UBase>>>, PoweredUnit<U, ((N * PD) + (PN * D)) / PGCDMultiply, (D * PD) / PGCDMultiply, UBase>>;
		using DivideType = StaticUtilities::IfElse<(N * PD - D * PN) % (D * PD) == 0, StaticUtilities::IfElse<(N * PD) - (D * PN) == 0, Unitless<ValueType>, StaticUtilities::IfElse<(N * PD) - (PN * D) == (D * PD), U, PoweredUnit<U, ((N * PD) - (D * PN)) / (D * PD), 1, UBase>>>, PoweredUnit<U, ((N * PD) - (PN * D)) / PGCDMultiply, (D * PD) / PGCDMultiply, UBase>>;
	};

	///////////////////////////
	// Constructors
	///////////////////////////
	constexpr explicit PoweredUnit(const ValueType &val = 0.0)
		: Unit<PoweredUnit<U, N, D, UBase>>(val)
	{
	}

	constexpr PoweredUnit(const PoweredUnit<U, N, D, UBase> &u)
		: Unit<PoweredUnit<U, N, D, UBase>>(u.Value())
	{
	}

	template <typename V>
	constexpr PoweredUnit(const PoweredUnit<V, N, D, UBase> &u)
		: Unit<PoweredUnit<U, N, D, UBase>>(::units::internals::ChangeFactor<PoweredUnit<U, N, D, UBase>, PoweredUnit<V, N, D, UBase>>::GetValue(u.Value()))
	{
	}

	///////////////////////////
	// Multiplication operators
	///////////////////////////
	// Make accessible operator* from base class (Unit<U>)
	using Unit<PoweredUnit<U, N, D, UBase>>::operator*;

	// Multiply by the inverse power -> double
	constexpr typename OperatorResultType<PoweredUnit<U, -N, D, UBase>>::MultiplyType operator*(const PoweredUnit<U, -N, D, UBase> &u) const
	{
		using ReturnType = typename OperatorResultType<PoweredUnit<U, -N, D, UBase>>::MultiplyType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() * u.Value());
	}

	// Multiply by the unit itself (same scale, power = 1)
	constexpr typename OperatorResultType<U>::MultiplyType operator*(const U &u) const
	{
		using ReturnType = typename OperatorResultType<U>::MultiplyType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() * u.Value());
	}

	// Multiply by a ScaledUnit, with same baseUnit, different scale
	template <typename V, int64 SN, int64 SD>
	constexpr typename OperatorResultType<ScaledUnit<V, SN, SD, UBase>>::MultiplyType operator*(const ScaledUnit<V, SN, SD, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<U, ScaledUnit<V, SN, SD, UBase>>;
		using ReturnType = typename OperatorResultType<ScaledUnit<V, SN, SD, UBase>>::MultiplyType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() * ChangeFactorType::GetValue(u.Value()));
	}

	// Multiply by another PoweredUnit, with the same baseUnit, but different scale and different power
	template <typename V, int PN2, int PD2>
	constexpr typename OperatorResultType<PoweredUnit<V, PN2, PD2, UBase>>::MultiplyType operator*(const PoweredUnit<V, PN2, PD2, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<U, PoweredUnit<V, PN2, PD2, UBase>>;
		using ReturnType = typename OperatorResultType<PoweredUnit<V, PN2, PD2, UBase>>::MultiplyType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() * ChangeFactorType::GetValue(u.Value()));
	}

	// Multiply by Another PoweredUnit -> ComposedUnit
	template <typename V, int32 PN2, int32 PD2, typename UBase2>
	constexpr ::units::MutiplyResultType<PoweredUnit<U, N, D, UBase>, PoweredUnit<V, PN2, PD2, UBase2>> operator*(const PoweredUnit<V, PN2, PD2, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<PoweredUnit<U, N, D, UBase>, PoweredUnit<V, PN2, PD2, UBase2>>;
		return Op::Multiply(*this, u);
	}

	// Multiply by Another ScaledUnit -> ComposedUnit
	template <typename V, int64 SN, int64 SD, typename UBase2>
	constexpr ::units::MutiplyResultType<PoweredUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase2>> operator*(const ScaledUnit<V, SN, SD, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<PoweredUnit<U, N, D, UBase>, ScaledUnit<V, SN, SD, UBase2>>;
		return Op::Multiply(*this, u);
	}

	// Multiply by Another PoweredUnit -> ComposedUnit
	template <typename U1, typename U2>
	constexpr ::units::MutiplyResultType<ComposedUnit<U1, U2>, PoweredUnit<U, N, D, UBase>> operator*(const ComposedUnit<U1, U2> &u) const
	{
		using Op = ::units::UnitOperator<ComposedUnit<U1, U2>, PoweredUnit<U, N, D, UBase>>;
		return Op::Multiply(u, *this);
	}

	///////////////////////////
	// Division Operators
	///////////////////////////
	// Make accessible operator/ from base class (Unit<U>)
	using Unit<PoweredUnit<U, N, D, UBase>>::operator/;

	// Divide by a transformed unit of the same power
	template <typename V>
	constexpr typename OperatorResultType<PoweredUnit<V, N, D, UBase>>::DivideType operator/(const PoweredUnit<V, N, D, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<U, PoweredUnit<V, N, D, UBase>>;
		using ReturnType = typename OperatorResultType<PoweredUnit<V, N, D, UBase>>::DivideType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() / ChangeFactorType::GetValue(u.Value()));
	}

	// Divide by a different power of the same unit
	template <int32 N2, int32 D2>
	constexpr typename OperatorResultType<PoweredUnit<U, N2, D2, UBase>>::DivideType operator/(const PoweredUnit<U, N2, D2, UBase> &u) const
	{
		using ReturnType = typename OperatorResultType<PoweredUnit<U, N2, D2, UBase>>::DivideType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() / u.Value());
	}

	// Divide by a transformed unit of different power
	template <typename V, int32 N2, int32 D2>
	constexpr typename OperatorResultType<PoweredUnit<V, N2, D2, UBase>>::DivideType operator/(const PoweredUnit<V, N2, D2, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<U, PoweredUnit<V, N2, D2, UBase>>;
		using ReturnType = typename OperatorResultType<PoweredUnit<V, N2, D2, UBase>>::DivideType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() / ChangeFactorType::GetValue(u.Value()));
	}

	// Divide by a ScaledUnit of the same baseUnit, different scale -> Scale it, and divide
	template <typename V, int64 N2, int64 D2>
	constexpr typename OperatorResultType<ScaledUnit<V, N2, D2, UBase>>::DivideType operator/(const ScaledUnit<V, N2, D2, UBase> &u) const
	{
		using ChangeFactorType = ::units::internals::ChangeFactor<U, ScaledUnit<V, N2, D2, UBase>>;
		using ReturnType = typename OperatorResultType<ScaledUnit<V, N2, D2, UBase>>::DivideType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() / ChangeFactorType::GetValue(u.Value()));
	}

	// Divide by a ScaledUnit of the same baseUnit -> Scale it, and divide
	constexpr typename OperatorResultType<U>::DivideType operator/(const U &u) const
	{
		using ReturnType = typename OperatorResultType<U>::DivideType;

		return ReturnType(Unit<PoweredUnit<U, N, D, UBase>>::Value() / u.Value());
	}

	// Divide by a PoweredUnit of another baseUnit -> Compose it
	template <typename V, int32 N2, int32 D2, typename UBase2>
	constexpr ::units::DivideResultType<PoweredUnit<U, N, D, UBase>, PoweredUnit<V, N2, D2, UBase2>> operator/(const PoweredUnit<V, N2, D2, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<PoweredUnit<U, N, D, UBase>, PoweredUnit<V, N2, D2, UBase2>>;
		return Op::Divide(*this, u);
	}

	// Divide by a ScaledUnit of another baseUnit -> Compose it
	template <typename V, int64 N2, int64 D2, typename UBase2>
	constexpr ::units::DivideResultType<PoweredUnit<U, N, D, UBase>, ScaledUnit<V, N2, D2, UBase2>> operator/(const ScaledUnit<V, N2, D2, UBase2> &u) const
	{
		using Op = ::units::UnitOperator<PoweredUnit<U, N, D, UBase>, ScaledUnit<V, N2, D2, UBase2>>;
		return Op::Divide(*this, u);
	}

	// Divide by a ComposedUnit -> ComposedUnit
	template <typename U1, typename U2>
	constexpr ::units::DivideResultType<PoweredUnit<U, N, D, UBase>, ComposedUnit<U1, U2>> operator/(const ComposedUnit<U1, U2> &u) const
	{
		using Op = ::units::UnitOperator<PoweredUnit<U, N, D, UBase>, ComposedUnit<U1, U2>>;
		return Op::Divide(*this, u);
	}
};

////////////////////////////////////////////
// Composed Unit
////////////////////////////////////////////
template <typename U1, typename U2>
class ComposedUnit final : public Unit<ComposedUnit<U1, U2>>
{
public:
	using ValueType = typename Unit<ComposedUnit<U1, U2>>::ValueType;

	///////////////////////////
	// Constructors
	///////////////////////////
	constexpr explicit ComposedUnit(const ValueType &val = 0.0)
		: Unit<ComposedUnit<U1, U2>>(val)
	{
	}

	constexpr ComposedUnit(const ComposedUnit<U1, U2> &u)
		: Unit<ComposedUnit<U1, U2>>(u.Value())
	{
	}

	template <typename V1, typename V2, typename = StaticUtilities::EnableIf<units::IsEquivalent<ComposedUnit<U1, U2>, ComposedUnit<V1, V2>>>>
	constexpr ComposedUnit(const ComposedUnit<V1, V2> &u)
		: Unit<ComposedUnit<U1, U2>>(u.Value() * ::units::IsEquivalent<ComposedUnit<U1, U2>, ComposedUnit<V1, V2>>::GetFactor())
	{
	}

	// From a PoweredUnit (1/1) of a ComposedUnit same as this one -> Just the ComposedUnit
	constexpr ComposedUnit(const PoweredUnit<ComposedUnit<U1, U2>, 1, 1, ComposedUnit<U1, U2>> &u)
		: Unit<ComposedUnit<U1, U2>>(u.Value())
	{
	}

	template <typename U, int64 N, int64 D>
	constexpr ComposedUnit(const ScaledUnit<U, N, D, ComposedUnit<U1, U2>> &u)
		: Unit<ComposedUnit<U1, U2>>(::units::internals::ChangeFactor<ComposedUnit<U1, U2>, ScaledUnit<U, N, D, ComposedUnit<U1, U2>>>::GetValue(u.Value()))
	{
	}

	///////////////////////////
	// Assignation operators
	///////////////////////////
	using Unit<ComposedUnit<U1, U2>>::operator=;

	template <typename V1, typename V2, typename = StaticUtilities::EnableIf<units::IsEquivalent<ComposedUnit<U1, U2>, ComposedUnit<V1, V2>>>>
	constexpr ComposedUnit<U1, U2> &operator=(const ComposedUnit<V1, V2> &v)
	{
		ValueType changeFactor = ::units::IsEquivalent<ComposedUnit<U1, U2>, ComposedUnit<V1, V2>>::GetFactor();
		Unit<ComposedUnit<U1, U2>>::m_value = v.Value() * changeFactor;
		return *this;
	}

	///////////////////////////
	// Multiplication operators
	///////////////////////////
	// Make accessible operator* from base class (Unit<U>)
	using Unit<ComposedUnit<U1, U2>>::operator*;

	// Multiply by Another Unit -> ComposedUnit
	template <typename V, typename = StaticUtilities::EnableIf<units::IsUnit<V>>>
	constexpr units::MutiplyResultType<ComposedUnit<U1, U2>, V> operator*(const V &u) const
	{
		using Op = ::units::UnitOperator<ComposedUnit<U1, U2>, V>;
		return Op::Multiply(*this, u);
	}

	// Multiply by self -> PoweredUnit
	constexpr ::units::MutiplyResultType<ComposedUnit<U1, U2>, ComposedUnit<U1, U2>> operator*(const ComposedUnit<U1, U2> &u) const
	{
		using ReturnType = ::units::MutiplyResultType<ComposedUnit<U1, U2>, ComposedUnit<U1, U2>>;
		return ReturnType(Unit<ComposedUnit<U1, U2>>::Value() * u.Value());
	}

	///////////////////////////
	// Division operators
	///////////////////////////
	// Make accessible operator/ from base class (Unit<U>)
	using Unit<ComposedUnit<U1, U2>>::operator/;

	// Divide by Another Unit -> ComposedUnit
	template <typename V, typename = StaticUtilities::EnableIf<units::IsUnit<V>>>
	constexpr units::DivideResultType<ComposedUnit<U1, U2>, V> operator/(const V &u) const
	{
		using Op = ::units::UnitOperator<ComposedUnit<U1, U2>, V>;
		return Op::Divide(*this, u);
	}
};

////////////////////////////////////////////
// Global Unit operators functions
////////////////////////////////////////////
template <typename U, typename REP>
constexpr units::InvPow<U> operator/(const Unitless<REP> &d, const Unit<U> &u)
{
	return units::InvPow<U>(d.Value() / u.Value());
}

template <typename F, typename U, typename = StaticUtilities::EnableIf<StaticUtilities::IsArithmetic<F>>>
constexpr units::InvPow<U> operator/(const F &f, const Unit<U> &u)
{
	return units::InvPow<U>(f / u.Value());
}

template <typename U, typename = StaticUtilities::EnableIf<units::IsUnit<U>>>
constexpr units::Sqrt<U> sqrt(const Unit<U> &u)
{
	return units::Sqrt<U>(std::sqrt(u.Value()));
}

template <int32 PowNum, int32 PowDen = 1, typename U, typename = StaticUtilities::EnableIf<units::IsUnit<U>>>
constexpr units::Pow<U, PowNum, PowDen> pow(const Unit<U> &u)
{
	return units::Pow<U, PowNum, PowDen>(std::pow(u.Value(), static_cast<typename Unit<U>::ValueType>(PowNum) / static_cast<typename Unit<U>::ValueType>(PowDen)));
}

////////////////////////////////////////////
// Transform to absolute value (abs functions)
////////////////////////////////////////////
template <typename U, typename = StaticUtilities::EnableIf<units::IsUnit<U>>>
constexpr U abs(const U &u)
{
	return U(std::abs((typename U::ValueType)u));
}

////////////////////////////////////////////
// Write to stream
////////////////////////////////////////////
template <typename S, typename U>
S &operator<<(S &os, const Unit<U> &u)
{
	os << u.Value() << ' ';
	::units::outputs::outputUnit<U>::Output(os);
	return os;
}

template <typename U>
std::string to_string(const Unit<U> &unit)
{
	std::ostringstream oss;
	oss << unit;
	return oss.str();
}

#define UNIT_DISPLAY_NAME(unit, s)                          \
	namespace units                                         \
	{                                                       \
		namespace outputs                                   \
		{                                                   \
			template <>                                     \
			struct outputUnit<::unit>                       \
			{                                               \
				template <typename Stream>                  \
				static void Output(Stream &os) { os << s; } \
			};                                              \
		}                                                   \
	}

// Used to create a type multiplying an arbitrary number of types
template <typename U, typename... Values>
struct Multiply
{
	using Type = typename units::TransformUnit<U, typename Multiply<Values...>::Type>::MultiplyType;
};

template <typename U>
struct Multiply<U>
{
	using Type = U;
};

// Used to create a type that divide two resulting types (can be used with Multiply)
template <typename U, typename V>
struct Divide
{
	using Type = typename units::TransformUnit<U, V>::DivideType;
};

////////////////////////////////////////////
// SI Prefix definitions
////////////////////////////////////////////
template <typename Unit>
struct standard
{
	typedef ScaledUnit<Unit> Type;
};

template <typename Unit>
struct deca
{
	typedef ScaledUnit<Unit, 10> Type;
};
template <typename Unit>
struct hecto
{
	typedef ScaledUnit<Unit, 100> Type;
};
template <typename Unit>
struct kilo
{
	typedef ScaledUnit<Unit, 1000> Type;
};
template <typename Unit>
struct mega
{
	typedef ScaledUnit<typename ::kilo<Unit>::Type, 1000> Type;
};
template <typename Unit>
struct giga
{
	typedef ScaledUnit<typename ::mega<Unit>::Type, 1000> Type;
};
template <typename Unit>
struct tera
{
	typedef ScaledUnit<typename ::giga<Unit>::Type, 1000> Type;
};
template <typename Unit>
struct peta
{
	typedef ScaledUnit<typename ::tera<Unit>::Type, 1000> Type;
};
template <typename Unit>
struct exa
{
	typedef ScaledUnit<typename ::peta<Unit>::Type, 1000> Type;
};
template <typename Unit>
struct zetta
{
	typedef ScaledUnit<typename ::exa<Unit>::Type, 1000> Type;
};
template <typename Unit>
struct yotta
{
	typedef ScaledUnit<typename ::zetta<Unit>::Type, 1000> Type;
};

template <typename Unit>
struct deci
{
	typedef ScaledUnit<Unit, 1, 10> Type;
};
template <typename Unit>
struct centi
{
	typedef ScaledUnit<Unit, 1, 100> Type;
};
template <typename Unit>
struct milli
{
	typedef ScaledUnit<Unit, 1, 1000> Type;
};
template <typename Unit>
struct micro
{
	typedef ScaledUnit<typename ::milli<Unit>::Type, 1, 1000> Type;
};
template <typename Unit>
struct nano
{
	typedef ScaledUnit<typename ::micro<Unit>::Type, 1, 1000> Type;
};
template <typename Unit>
struct pico
{
	typedef ScaledUnit<typename ::nano<Unit>::Type, 1, 1000> Type;
};
template <typename Unit>
struct femto
{
	typedef ScaledUnit<typename ::pico<Unit>::Type, 1, 1000> Type;
};
template <typename Unit>
struct atto
{
	typedef ScaledUnit<typename ::femto<Unit>::Type, 1, 1000> Type;
};
template <typename Unit>
struct zepto
{
	typedef ScaledUnit<typename ::atto<Unit>::Type, 1, 1000> Type;
};
template <typename Unit>
struct yocto
{
	typedef ScaledUnit<typename ::zepto<Unit>::Type, 1, 1000> Type;
};

////////////////////////////////////////////
// SI Base Unit - DO NOT USE DIRECTLY
////////////////////////////////////////////
template <typename R = BaseValueRepresentation>
struct metreBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct secondBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct gBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct moleBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct kelvinBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct ampereBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct candelaBase
{
	using ValueType = R;
};

template <typename R = BaseValueRepresentation>
struct angleBase
{
	using ValueType = R;
};

// Generate display
UNIT_DISPLAY_NAME(metreBase<>, "m")
UNIT_DISPLAY_NAME(secondBase<>, "s")
UNIT_DISPLAY_NAME(moleBase<>, "mol")
UNIT_DISPLAY_NAME(kelvinBase<>, "K")
UNIT_DISPLAY_NAME(gBase<>, "g")
UNIT_DISPLAY_NAME(ampereBase<>, "A")
UNIT_DISPLAY_NAME(candelaBase<>, "cd")

////////////////////////////////////////////
// SI Unit - The one to use
////////////////////////////////////////////

// Distance
using Metre = standard<metreBase<>>::Type;
// Time
using Second = standard<secondBase<>>::Type;
// Amount of substance
using Mole = standard<moleBase<>>::Type;
// Temperature
using Kelvin = standard<kelvinBase<>>::Type;
// Mass
using Gram = standard<gBase<>>::Type;
// Electric current
using Ampere = standard<ampereBase<>>::Type;
// Luminous intensity
using Candela = standard<candelaBase<>>::Type;
// Angle
using Radian = standard<angleBase<>>::Type;

////////////////////////////////////////////
// Custom Units
////////////////////////////////////////////

struct powerBase
{
	using ValueType = double;
};

struct tickBase
{
	using ValueType = int;
};

// Power
using Power = standard<powerBase>::Type;
// Tick
using Tick = standard<tickBase>::Type;

UNIT_DISPLAY_NAME(Power, "Power")
UNIT_DISPLAY_NAME(Tick, "Tick")

////////////////////////////////////////////
// Scaled SI Unit
////////////////////////////////////////////

// Length units

using Kilometre = ::kilo<Metre>::Type;
using Decimetre = ::deci<Metre>::Type;
using Centimetre = ::centi<Metre>::Type;
using Millimetre = ::milli<Metre>::Type;
using Micrometre = ::micro<Metre>::Type;
using Nanometre = ::nano<Metre>::Type;
using Picometre = ::pico<Metre>::Type;

using Inch = ScaledUnit<Centimetre, 254, 100>;
using Foot = ScaledUnit<Inch, 12, 1>;
using Yard = ScaledUnit<Foot, 3, 1>;
using Mile = ScaledUnit<Yard, 1760, 1>;

// Mass units

using Kilogram = ::kilo<Gram>::Type;
using Centigram = ::centi<Gram>::Type;
using Milligram = ::milli<Gram>::Type;

// Time units

using Attosecond = ::atto<Second>::Type;
using Femtosecond = ::femto<Second>::Type;
using Picosecond = ::pico<Second>::Type;
using Nanosecond = ::nano<Second>::Type;
using Microsecond = ::micro<Second>::Type;
using Millisecond = ::milli<Second>::Type;
using Minute = ScaledUnit<Second, 60, 1>;
using Hour = ScaledUnit<Minute, 60, 1>;
using Day = ScaledUnit<Hour, 24, 1>;

// Electric current

using Kiloampere = ::kilo<Ampere>::Type;
using Milliampere = ::milli<Ampere>::Type;

// Temperature

using Kilokelvin = ::kilo<Kelvin>::Type;
using Millikelvin = ::milli<Kelvin>::Type;

// Amount of substance

using Kilomole = ::kilo<Mole>::Type;
using Millimole = ::milli<Mole>::Type;

// Luminous intensity

using Millicandela = ::milli<Candela>::Type;

// Angle unit
using Degree = ScaledUnit<Radian, 31415926535897932, 1800000000000000000>;
using Revolution = ScaledUnit<Radian, 31415926535897932, 5000000000000000>;

UNIT_DISPLAY_NAME(Radian, "Radian")
UNIT_DISPLAY_NAME(Degree, "Degree")
UNIT_DISPLAY_NAME(Revolution, "Revolution")

////////////////////////////////////////////
// Powered SI Unit
////////////////////////////////////////////

using Centimetre2 = ::units::Pow<Centimetre, 2>;
using Centimetre3 = ::units::Pow<Centimetre, 3>;
using Centimetre4 = ::units::Pow<Centimetre, 4>;

using CentimetreM1 = ::units::Pow<Centimetre, -1>;
using CentimetreM2 = ::units::Pow<Centimetre, -2>;
using CentimetreM3 = ::units::Pow<Centimetre, -3>;
using CentimetreM4 = ::units::Pow<Centimetre, -4>;

using Decimetre2 = ::units::Pow<Decimetre, 2>;
using Decimetre3 = ::units::Pow<Decimetre, 3>;
using Decimetre4 = ::units::Pow<Decimetre, 4>;

using DecimetreM1 = ::units::Pow<Decimetre, -1>;
using DecimetreM2 = ::units::Pow<Decimetre, -2>;
using DecimetreM3 = ::units::Pow<Decimetre, -3>;
using DecimetreM4 = ::units::Pow<Decimetre, -4>;

using Metre2 = ::units::Pow<Metre, 2>;
using Metre3 = ::units::Pow<Metre, 3>;
using Metre4 = ::units::Pow<Metre, 4>;

using MetreM1 = ::units::Pow<Metre, -1>;
using MetreM2 = ::units::Pow<Metre, -2>;
using MetreM3 = ::units::Pow<Metre, -3>;
using MetreM4 = ::units::Pow<Metre, -4>;

using Kilometre2 = ::units::Pow<Kilometre, 2>;
using Kilometre3 = ::units::Pow<Kilometre, 3>;
using Kilometre4 = ::units::Pow<Kilometre, 4>;

using KilometreM1 = ::units::Pow<Kilometre, -1>;
using KilometreM2 = ::units::Pow<Kilometre, -2>;
using KilometreM3 = ::units::Pow<Kilometre, -3>;
using KilometreM4 = ::units::Pow<Kilometre, -4>;

using Millimetre2 = ::units::Pow<Millimetre, 2>;
using Millimetre3 = ::units::Pow<Millimetre, 3>;
using Millimetre4 = ::units::Pow<Millimetre, 4>;

using MillimetreM1 = ::units::Pow<Millimetre, -1>;
using MillimetreM2 = ::units::Pow<Millimetre, -2>;
using MillimetreM3 = ::units::Pow<Millimetre, -3>;
using MillimetreM4 = ::units::Pow<Millimetre, -4>;

using Second2 = ::units::Pow<Second, 2>;
using Second3 = ::units::Pow<Second, 3>;
using Second4 = ::units::Pow<Second, 4>;

using SecondM1 = ::units::Pow<Second, -1>;
using SecondM2 = ::units::Pow<Second, -2>;
using SecondM3 = ::units::Pow<Second, -3>;
using SecondM4 = ::units::Pow<Second, -4>;

using Millisecond2 = ::units::Pow<Millisecond, 2>;
using Millisecond3 = ::units::Pow<Millisecond, 3>;

using MillisecondM1 = ::units::Pow<Millisecond, -1>;
using MillisecondM2 = ::units::Pow<Millisecond, -2>;

using Ampere2 = ::units::Pow<Ampere, 2>;
using Ampere3 = ::units::Pow<Ampere, 3>;
using Ampere4 = ::units::Pow<Ampere, 4>;

using AmpereM1 = ::units::Pow<Ampere, -1>;
using AmpereM2 = ::units::Pow<Ampere, -2>;
using AmpereM3 = ::units::Pow<Ampere, -3>;

////////////////////////////////////////////
// Named Derived SI Unit
////////////////////////////////////////////
using Newton = Multiply<Kilogram, Metre, SecondM2>::Type;	// Force
using Pascal = Multiply<Kilogram, MetreM1, SecondM2>::Type; // Pressure, stress

using Joule = Multiply<Kilogram, Metre2, SecondM2>::Type;  // Energy, work, heat
using Torque = Multiply<Kilogram, Metre2, SecondM2>::Type; // Torque, moment of force
using Watt = Multiply<Kilogram, Metre2, SecondM3>::Type;   // Power, radiant flux

using Coulomb = Multiply<Ampere, Second>::Type;													  // Electric charge or quantity of electricity
using Volt = Divide<Multiply<Kilogram, Metre2>::Type, Multiply<Second3, Ampere>::Type>::Type;	  // Voltage (Electrical potential difference), Electromotive force (Joule/Coulomb)
using Farad = Divide<Multiply<Ampere2, Second4>::Type, Multiply<Kilogram, Metre2>::Type>::Type;	  // Electric capacitance (Coulomb/Volt)
using Ohm = Divide<Multiply<Kilogram, Metre2>::Type, Multiply<Second3, Ampere2>::Type>::Type;	  // Electric resistance, impedance, reactance (Volt/Ampere)
using Siemens = Divide<Multiply<Ampere2, Second3>::Type, Multiply<Kilogram, Metre2>::Type>::Type; // Electrical conductance (Ampere/Volt)

using Weber = Divide<Kilogram, Multiply<Metre2, Second2, Ampere>::Type>::Type;					// Magnetic flux
using Tesla = Divide<Kilogram, Multiply<Second2, Ampere>::Type>::Type;							// Magnetic field strength
using Henry = Divide<Multiply<Kilogram, Metre2>::Type, Multiply<Second2, Ampere2>::Type>::Type; // Inductance

using Lumen = Candela;					   // Luminous flux
using Lux = Divide<Candela, Metre2>::Type; // Illuminance

using Becquerel = SecondM1;					   // Radioactivity
using Gray = Divide<Metre2, Second2>::Type;	   // Absorbed dose (of ionizing radiation)
using Sievert = Divide<Metre2, Second2>::Type; // Equivalent dose (of ionizing radiation)

using Katal = Divide<Mole, Second>::Type; // Catalytic Activity
using Hertz = SecondM1;					  // Frequency

UNIT_DISPLAY_NAME(Newton, "N")
UNIT_DISPLAY_NAME(Pascal, "Pa")
UNIT_DISPLAY_NAME(Joule, "J")
UNIT_DISPLAY_NAME(Watt, "W")
UNIT_DISPLAY_NAME(Coulomb, "C")
UNIT_DISPLAY_NAME(Volt, "V")
UNIT_DISPLAY_NAME(Farad, "F")
UNIT_DISPLAY_NAME(Ohm, "Ohm")
UNIT_DISPLAY_NAME(Siemens, "S")
UNIT_DISPLAY_NAME(Weber, "Wb")
UNIT_DISPLAY_NAME(Tesla, "T")
UNIT_DISPLAY_NAME(Henry, "H")
UNIT_DISPLAY_NAME(Lux, "lx")
UNIT_DISPLAY_NAME(Gray, "Gy")
UNIT_DISPLAY_NAME(Katal, "kat")

UNIT_DISPLAY_NAME(Hour, "H")

// UNIT_DISPLAY_NAME(Hertz, "Hz")
// UNIT_DISPLAY_NAME(Becquerel, "Bq")
// UNIT_DISPLAY_NAME(Sievert, "Sv")

////////////////////////////////////////////
// Other Composed Unit
////////////////////////////////////////////
using Speed = Divide<Metre, Second>::Type;
using AngularSpeed = Divide<Degree, Second>::Type;
using Acceleration = Divide<Metre, Second2>::Type;
using AngularAcceleration = Divide<Degree, Second2>::Type;
using Momentum = Divide<Multiply<Kilogram, Metre>::Type, Second>::Type;
using Density = Divide<Kilogram, Metre3>::Type;

using Angstrom = ScaledUnit<Metre, 1, 10000000000>;

////////////////////////////////////////////
// Literals
////////////////////////////////////////////

// Length literals
constexpr Metre operator"" _m(long double val)
{
	return Metre(static_cast<Metre::ValueType>(val));
}

constexpr Metre operator"" _m(unsigned long long val)
{
	return Metre(static_cast<Metre::ValueType>(val));
}

constexpr Kilometre operator"" _km(long double val)
{
	return Kilometre(static_cast<Kilometre::ValueType>(val));
}

constexpr Kilometre operator"" _km(unsigned long long val)
{
	return Kilometre(static_cast<Kilometre::ValueType>(val));
}

constexpr Decimetre operator"" _dm(long double val)
{
	return Decimetre(static_cast<Decimetre::ValueType>(val));
}

constexpr Decimetre operator"" _dm(unsigned long long val)
{
	return Decimetre(static_cast<Decimetre::ValueType>(val));
}

constexpr Centimetre operator"" _cm(long double val)
{
	return Centimetre(static_cast<Centimetre::ValueType>(val));
}

constexpr Centimetre operator"" _cm(unsigned long long val)
{
	return Centimetre(static_cast<Centimetre::ValueType>(val));
}

constexpr Millimetre operator"" _mm(long double val)
{
	return Millimetre(static_cast<Millimetre::ValueType>(val));
}

constexpr Millimetre operator"" _mm(unsigned long long val)
{
	return Millimetre(static_cast<Millimetre::ValueType>(val));
}

constexpr Micrometre operator"" _um(long double val)
{
	return Micrometre(static_cast<Micrometre::ValueType>(val));
}

constexpr Micrometre operator"" _um(unsigned long long val)
{
	return Micrometre(static_cast<Micrometre::ValueType>(val));
}

constexpr Nanometre operator"" _nm(long double val)
{
	return Nanometre(static_cast<Nanometre::ValueType>(val));
}

constexpr Nanometre operator"" _nm(unsigned long long val)
{
	return Nanometre(static_cast<Nanometre::ValueType>(val));
}

constexpr Picometre operator"" _pm(long double val)
{
	return Picometre(static_cast<Picometre::ValueType>(val));
}

constexpr Picometre operator"" _pm(unsigned long long val)
{
	return Picometre(static_cast<Picometre::ValueType>(val));
}

// Fredom literals
constexpr Inch operator"" _in(long double val)
{
	return Inch(static_cast<Inch::ValueType>(val));
}

constexpr Inch operator"" _in(unsigned long long val)
{
	return Inch(static_cast<Inch::ValueType>(val));
}

constexpr Foot operator"" _ft(long double val)
{
	return Foot(static_cast<Foot::ValueType>(val));
}

constexpr Foot operator"" _ft(unsigned long long val)
{
	return Foot(static_cast<Foot::ValueType>(val));
}

constexpr Yard operator"" _yd(long double val)
{
	return Yard(static_cast<Yard::ValueType>(val));
}

constexpr Yard operator"" _yd(unsigned long long val)
{
	return Yard(static_cast<Yard::ValueType>(val));
}

constexpr Mile operator"" _mi(long double val)
{
	return Mile(static_cast<Mile::ValueType>(val));
}

constexpr Mile operator"" _mi(unsigned long long val)
{
	return Mile(static_cast<Mile::ValueType>(val));
}

// Mass literals
constexpr Kilogram operator"" _kg(long double val)
{
	return Kilogram(static_cast<Kilogram::ValueType>(val));
}

constexpr Kilogram operator"" _kg(unsigned long long val)
{
	return Kilogram(static_cast<Kilogram::ValueType>(val));
}

constexpr Gram operator"" _g(long double val)
{
	return Gram(static_cast<Gram::ValueType>(val));
}

constexpr Gram operator"" _g(unsigned long long val)
{
	return Gram(static_cast<Gram::ValueType>(val));
}

constexpr Milligram operator"" _mg(long double val)
{
	return Milligram(static_cast<Milligram::ValueType>(val));
}

constexpr Milligram operator"" _mg(unsigned long long val)
{
	return Milligram(static_cast<Milligram::ValueType>(val));
}

constexpr Centigram operator"" _cg(long double val)
{
	return Centigram(static_cast<Centigram::ValueType>(val));
}

constexpr Centigram operator"" _cg(unsigned long long val)
{
	return Centigram(static_cast<Centigram::ValueType>(val));
}

// Time literals
constexpr Second operator"" _s(long double val)
{
	return Second(static_cast<Second::ValueType>(val));
}

constexpr Second operator"" _s(unsigned long long val)
{
	return Second(static_cast<Second::ValueType>(val));
}

constexpr Millisecond operator"" _ms(long double val)
{
	return Millisecond(static_cast<Millisecond::ValueType>(val));
}

constexpr Millisecond operator"" _ms(unsigned long long val)
{
	return Millisecond(static_cast<Millisecond::ValueType>(val));
}

constexpr Minute operator"" _min(long double val)
{
	return Minute(static_cast<Minute::ValueType>(val));
}

constexpr Minute operator"" _min(unsigned long long val)
{
	return Minute(static_cast<Minute::ValueType>(val));
}

constexpr Hour operator"" _h(long double val)
{
	return Hour(static_cast<Hour::ValueType>(val));
}

constexpr Hour operator"" _h(unsigned long long val)
{
	return Hour(static_cast<Hour::ValueType>(val));
}

constexpr Day operator"" _d(long double val)
{
	return Day(static_cast<Day::ValueType>(val));
}

constexpr Day operator"" _d(unsigned long long val)
{
	return Day(static_cast<Day::ValueType>(val));
}

// Electric current literals
constexpr Ampere operator"" _A(long double val)
{
	return Ampere(static_cast<Ampere::ValueType>(val));
}

constexpr Ampere operator"" _A(unsigned long long val)
{
	return Ampere(static_cast<Ampere::ValueType>(val));
}

constexpr Milliampere operator"" _mA(long double val)
{
	return Milliampere(static_cast<Milliampere::ValueType>(val));
}

constexpr Milliampere operator"" _mA(unsigned long long val)
{
	return Milliampere(static_cast<Milliampere::ValueType>(val));
}

constexpr Kiloampere operator"" _kA(long double val)
{
	return Kiloampere(static_cast<Kiloampere::ValueType>(val));
}

constexpr Kiloampere operator"" _kA(unsigned long long val)
{
	return Kiloampere(static_cast<Kiloampere::ValueType>(val));
}

// Temperature literals
constexpr Kelvin operator"" _K(long double val)
{
	return Kelvin(static_cast<Kelvin::ValueType>(val));
}

constexpr Kelvin operator"" _K(unsigned long long val)
{
	return Kelvin(static_cast<Kelvin::ValueType>(val));
}

constexpr Millikelvin operator"" _mK(long double val)
{
	return Millikelvin(static_cast<Millikelvin::ValueType>(val));
}

constexpr Millikelvin operator"" _mK(unsigned long long val)
{
	return Millikelvin(static_cast<Millikelvin::ValueType>(val));
}

constexpr Kilokelvin operator"" _kK(long double val)
{
	return Kilokelvin(static_cast<Kilokelvin::ValueType>(val));
}

constexpr Kilokelvin operator"" _kK(unsigned long long val)
{
	return Kilokelvin(static_cast<Kilokelvin::ValueType>(val));
}

// Amount of substance literals
constexpr Mole operator"" _mol(long double val)
{
	return Mole(static_cast<Mole::ValueType>(val));
}

constexpr Mole operator"" _mol(unsigned long long val)
{
	return Mole(static_cast<Mole::ValueType>(val));
}

constexpr Kilomole operator"" _kmol(long double val)
{
	return Kilomole(static_cast<Kilomole::ValueType>(val));
}

constexpr Kilomole operator"" _kmol(unsigned long long val)
{
	return Kilomole(static_cast<Kilomole::ValueType>(val));
}

// Luminous intensity literals
constexpr Candela operator"" _cd(long double val)
{
	return Candela(static_cast<Candela::ValueType>(val));
}

constexpr Candela operator"" _cd(unsigned long long val)
{
	return Candela(static_cast<Candela::ValueType>(val));
}

constexpr Millicandela operator"" _mcd(long double val)
{
	return Millicandela(static_cast<Millicandela::ValueType>(val));
}

constexpr Millicandela operator"" _mcd(unsigned long long val)
{
	return Millicandela(static_cast<Millicandela::ValueType>(val));
}

// Angle literals
constexpr Radian operator"" _rad(long double val)
{
	return Radian(static_cast<Radian::ValueType>(val));
}

constexpr Radian operator"" _rad(unsigned long long val)
{
	return Radian(static_cast<Radian::ValueType>(val));
}

constexpr Degree operator"" _deg(long double val)
{
	return Degree(static_cast<Degree::ValueType>(val));
}

constexpr Degree operator"" _deg(unsigned long long val)
{
	return Degree(static_cast<Degree::ValueType>(val));
}

constexpr Revolution operator"" _rev(long double val)
{
	return Revolution(static_cast<Revolution::ValueType>(val));
}

constexpr Revolution operator"" _rev(unsigned long long val)
{
	return Revolution(static_cast<Revolution::ValueType>(val));
}

// Derived unit literals
constexpr Newton operator"" _N(long double val)
{
	return Newton(static_cast<Newton::ValueType>(val));
}

constexpr Newton operator"" _N(unsigned long long val)
{
	return Newton(static_cast<Newton::ValueType>(val));
}

constexpr Pascal operator"" _Pa(long double val)
{
	return Pascal(static_cast<Pascal::ValueType>(val));
}

constexpr Pascal operator"" _Pa(unsigned long long val)
{
	return Pascal(static_cast<Pascal::ValueType>(val));
}

constexpr Joule operator"" _J(long double val)
{
	return Joule(static_cast<Joule::ValueType>(val));
}

constexpr Joule operator"" _J(unsigned long long val)
{
	return Joule(static_cast<Joule::ValueType>(val));
}

constexpr Torque operator"" _Nm(long double val)
{
	return Torque(static_cast<Torque::ValueType>(val));
}

constexpr Torque operator"" _Nm(unsigned long long val)
{
	return Torque(static_cast<Torque::ValueType>(val));
}

constexpr Watt operator"" _W(long double val)
{
	return Watt(static_cast<Watt::ValueType>(val));
}

constexpr Watt operator"" _W(unsigned long long val)
{
	return Watt(static_cast<Watt::ValueType>(val));
}

constexpr Coulomb operator"" _C(long double val)
{
	return Coulomb(static_cast<Coulomb::ValueType>(val));
}

constexpr Coulomb operator"" _C(unsigned long long val)
{
	return Coulomb(static_cast<Coulomb::ValueType>(val));
}

constexpr Volt operator"" _V(long double val)
{
	return Volt(static_cast<Volt::ValueType>(val));
}

constexpr Volt operator"" _V(unsigned long long val)
{
	return Volt(static_cast<Volt::ValueType>(val));
}

constexpr Farad operator"" _F(long double val)
{
	return Farad(static_cast<Farad::ValueType>(val));
}

constexpr Farad operator"" _F(unsigned long long val)
{
	return Farad(static_cast<Farad::ValueType>(val));
}

constexpr Ohm operator"" _Ohm(long double val)
{
	return Ohm(static_cast<Ohm::ValueType>(val));
}

constexpr Ohm operator"" _Ohm(unsigned long long val)
{
	return Ohm(static_cast<Ohm::ValueType>(val));
}

constexpr Siemens operator"" _S(long double val)
{
	return Siemens(static_cast<Siemens::ValueType>(val));
}

constexpr Siemens operator"" _S(unsigned long long val)
{
	return Siemens(static_cast<Siemens::ValueType>(val));
}

constexpr Weber operator"" _Wb(long double val)
{
	return Weber(static_cast<Weber::ValueType>(val));
}

constexpr Weber operator"" _Wb(unsigned long long val)
{
	return Weber(static_cast<Weber::ValueType>(val));
}

constexpr Tesla operator"" _T(long double val)
{
	return Tesla(static_cast<Tesla::ValueType>(val));
}

constexpr Tesla operator"" _T(unsigned long long val)
{
	return Tesla(static_cast<Tesla::ValueType>(val));
}

constexpr Henry operator"" _H(long double val)
{
	return Henry(static_cast<Henry::ValueType>(val));
}

constexpr Henry operator"" _H(unsigned long long val)
{
	return Henry(static_cast<Henry::ValueType>(val));
}

constexpr Lumen operator"" _lm(long double val)
{
	return Lumen(static_cast<Lumen::ValueType>(val));
}

constexpr Lumen operator"" _lm(unsigned long long val)
{
	return Lumen(static_cast<Lumen::ValueType>(val));
}

constexpr Lux operator"" _lx(long double val)
{
	return Lux(static_cast<Lux::ValueType>(val));
}

constexpr Lux operator"" _lx(unsigned long long val)
{
	return Lux(static_cast<Lux::ValueType>(val));
}

constexpr Gray operator"" _Gy(long double val)
{
	return Gray(static_cast<Gray::ValueType>(val));
}

constexpr Gray operator"" _Gy(unsigned long long val)
{
	return Gray(static_cast<Gray::ValueType>(val));
}

constexpr Katal operator"" _kat(long double val)
{
	return Katal(static_cast<Katal::ValueType>(val));
}

constexpr Katal operator"" _kat(unsigned long long val)
{
	return Katal(static_cast<Katal::ValueType>(val));
}

constexpr Hertz operator"" _Hz(long double val)
{
	return Hertz(static_cast<Hertz::ValueType>(val));
}

constexpr Hertz operator"" _Hz(unsigned long long val)
{
	return Hertz(static_cast<Hertz::ValueType>(val));
}

constexpr Becquerel operator"" _Bq(long double val)
{
	return Becquerel(static_cast<Becquerel::ValueType>(val));
}

constexpr Becquerel operator"" _Bq(unsigned long long val)
{
	return Becquerel(static_cast<Becquerel::ValueType>(val));
}

constexpr Sievert operator"" _Sv(long double val)
{
	return Sievert(static_cast<Sievert::ValueType>(val));
}

constexpr Sievert operator"" _Sv(unsigned long long val)
{
	return Sievert(static_cast<Sievert::ValueType>(val));
}

// Other literals
constexpr Angstrom operator"" _Angstrom(long double val)
{
	return Angstrom(static_cast<Angstrom::ValueType>(val));
}

constexpr Angstrom operator"" _Angstrom(unsigned long long val)
{
	return Angstrom(static_cast<Angstrom::ValueType>(val));
}

// Power literals
constexpr Power operator"" _power(long double val)
{
	return Power(static_cast<Power::ValueType>(val));
}

constexpr Power operator"" _power(unsigned long long val)
{
	return Power(static_cast<Power::ValueType>(val));
}

// Tick literals
constexpr Tick operator"" _tick(unsigned long long val)
{
	return Tick(static_cast<Tick::ValueType>(val));
}

#endif //_UNITS_H_
#pragma clang diagnostic pop