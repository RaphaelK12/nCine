#ifndef CLASS_NCINE_VECTOR2
#define CLASS_NCINE_VECTOR2

#define _USE_MATH_DEFINES // for M_PI on MSVC
#include <cmath>

namespace ncine {

/// A two component vector based on templates
template <class T>
class Vector2
{
  public:
	T x, y;

	Vector2() { }
	explicit Vector2(T s) : x(s), y(s) { }
	Vector2(T xx, T yy) : x(xx), y(yy) { }

	void set(T xx, T yy);

	T* data();
	const T* data() const;

	T& operator[](unsigned int index);
	const T& operator[](unsigned int index) const;

	bool operator==(const Vector2& v) const;
	Vector2 operator-() const;

	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(const Vector2& v);
	Vector2& operator/=(const Vector2& v);

	Vector2& operator+=(T s);
	Vector2& operator-=(T s);
	Vector2& operator*=(T s);
	Vector2& operator/=(T s);

	Vector2 operator+(const Vector2& v) const;
	Vector2 operator-(const Vector2& v) const;
	Vector2 operator*(const Vector2& v) const;
	Vector2 operator/(const Vector2& v) const;

	Vector2 operator+(T s) const;
	Vector2 operator-(T s) const;
	Vector2 operator*(T s) const;
	Vector2 operator/(T s) const;

	T length() const;
	T sqrLength() const;
	Vector2 normalized() const;
	Vector2& normalize();

	template <class S> friend S dot(const Vector2<S>& v1, const Vector2<S>& v2);

	static const Vector2 Zero;
	static const Vector2 XAxis;
	static const Vector2 YAxis;
};

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;

template <class T>
inline void Vector2<T>::set(T xx, T yy)
{
	x = xx;
	y = yy;
}

template <class T>
T* Vector2<T>::data()
{
	return &x;
}

template <class T>
const T* Vector2<T>::data() const
{
	return &x;
}

template <class T>
inline T& Vector2<T>::operator[](unsigned int index)
{
	return (&x)[index];
}

template <class T>
inline const T& Vector2<T>::operator[](unsigned int index) const
{
	return (&x)[index];
}

template <class T>
inline bool Vector2<T>::operator==(const Vector2& v) const
{
	return (x == v.x && y == v.y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator-() const
{
	return Vector2(-x, -y);
}

template <class T>
inline Vector2<T>& Vector2<T>::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator*=(const Vector2& v)
{
	x *= v.x;
	y *= v.y;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator/=(const Vector2& v)
{
	x /= v.x;
	y /= v.y;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator+=(T s)
{
	x += s;
	y += s;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator-=(T s)
{
	x -= s;
	y -= s;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator*=(T s)
{
	x *= s;
	y *= s;

	return *this;
}

template <class T>
inline Vector2<T>& Vector2<T>::operator/=(T s)
{
	x /= s;
	y /= s;

	return *this;
}

template <class T>
inline Vector2<T> Vector2<T>::operator+(const Vector2& v) const
{
	return Vector2(x + v.x,
				   y + v.y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator-(const Vector2& v) const
{
	return Vector2(x - v.x,
				   y - v.y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator*(const Vector2& v) const
{
	return Vector2(x * v.x,
				   y * v.y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator/(const Vector2& v) const
{
	return Vector2(x / v.x,
				   y / v.y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator+(T s) const
{
	return Vector2(x + s,
				   y + s);
}

template <class T>
inline Vector2<T> Vector2<T>::operator-(T s) const
{
	return Vector2(x - s,
				   y - s);
}

template <class T>
inline Vector2<T> Vector2<T>::operator*(T s) const
{
	return Vector2(x * s,
				   y * s);
}

template <class T>
inline Vector2<T> Vector2<T>::operator/(T s) const
{
	return Vector2(x / s,
				   y / s);
}

template <class T>
inline T Vector2<T>::length() const
{
	return sqrt(x*x + y*y);
}

template <class T>
inline T Vector2<T>::sqrLength() const
{
	return x*x + y*y;
}

template <class T>
inline Vector2<T> Vector2<T>::normalized() const
{
	T len = length();
	return Vector2(x / len, y / len);
}

template <class T>
inline Vector2<T>& Vector2<T>::normalize()
{
	T len = length();

	x /= len;
	y /= len;

	return *this;
}

template <class S>
inline S dot(const Vector2<S>& v1, const Vector2<S>& v2)
{
	return S(v1.x * v2.x +
			 v1.y * v2.y);
}

template <class T> const Vector2<T> Vector2<T>::Zero(0, 0);
template <class T> const Vector2<T> Vector2<T>::XAxis(1, 0);
template <class T> const Vector2<T> Vector2<T>::YAxis(0, 1);

}

#endif