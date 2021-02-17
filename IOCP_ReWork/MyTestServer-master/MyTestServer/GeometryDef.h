#pragma once
#include "stdafx.h"
#include <assert.h>
#include <math.h>

#ifdef _MSC_VER
#pragma pack(push,8)
#endif

const float M_PI = (float)3.14159265358979323846;   // pi
//#define M_PI       3.14159265358979323846   // pi

//#define LOWORD(l)	((WORD)((DWORD)(l) & 0xffff))
//#define HIWORD(l)	((WORD)((DWORD)(l) >> 16))

namespace TypeVector {
	template <typename FloatType>
	struct T_Vector2 {
		FloatType x;
		FloatType y;
	public:
		inline T_Vector2() {};
		inline T_Vector2(const FloatType * from) { x = from[0]; y = from[1]; }
		inline T_Vector2(FloatType x, FloatType y) { this->x = x; this->y = y; }
		inline T_Vector2(const T_Vector2& init) { this->x = init.x; this->y = init.y; }

		inline T_Vector2& operator += (const T_Vector2& src) {
			x += src.x;
			y += src.y;
			return *this;
		}

		inline T_Vector2& operator -= (const T_Vector2& src) {
			x -= src.x;
			y -= src.y;
			return *this;
		}

		inline void Offset(int xOffset, int yOffset) {
			x += xOffset; y += yOffset;
		}
	};

	typedef T_Vector2<float> Vec2;
	typedef Vec2 fPoint;
	typedef Vec2 fSize;
	typedef Vec2 fPos;

	inline void dtVcopy(float* dest_, const float* src_) {
		static size_t length = sizeof(float) * 3;
		::memcpy_s(dest_, length, src_, length);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename FloatType>
	inline FloatType ScalarLerp(const FloatType v1, const FloatType v2, const FloatType ratio)
	{
		return v1 + (v2 - v1) * ratio;
	}

	template <typename FloatType>
	struct T_Vector3
	{
		/// x
		FloatType x;
		/// y
		FloatType y;
		/// z
		FloatType z;
	public:
		inline T_Vector3() {};

		inline T_Vector3(const FloatType * from)
		{
			x = from[0];
			y = from[1];
			z = from[2];
		}

		inline T_Vector3(FloatType x, FloatType y, FloatType z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		// casting
		inline operator FloatType* () {
			return &x;
		}
		inline operator const FloatType* () const {
			return &x;
		}


		inline T_Vector3 ConvertPos() {
			return T_Vector3(x, z, y);
		}


		//#ifdef _WIN32
		//		template<typename D3DXVECTOR3>
		//		__forceinline T_Vector3(const D3DXVECTOR3 &from)
		//		{
		//			x = from.x;
		//			y = from.y;
		//			z = from.z;
		//		}
		//
		//		template<typename D3DXVECTOR3>
		//		__forceinline operator D3DXVECTOR3 const ()
		//		{
		//			D3DXVECTOR3 ret;
		//			ret.x = x;
		//			ret.y = y;
		//			ret.z = z;
		//			return ret;
		//		}
		//#endif // _WIN32

		// assignment operators
		inline T_Vector3& operator += (const T_Vector3& src)
		{
			x += src.x;
			y += src.y;
			z += src.z;
			return *this;
		}

		inline T_Vector3& operator -= (const T_Vector3& src)
		{
			x -= src.x;
			y -= src.y;
			z -= src.z;
			return *this;
		}

		inline T_Vector3& operator *= (FloatType scale)
		{
			x *= scale;
			y *= scale;
			z *= scale;
			return *this;
		}

		inline T_Vector3& operator /= (FloatType scale)
		{
			x /= scale;
			y /= scale;
			z /= scale;
			return *this;
		}

		// unary operators
		inline T_Vector3 operator + () const
		{
			return *this;
		}

		inline T_Vector3 operator - () const
		{
			return T_Vector3(-x, -y, -z);
		}

		// binary operators
		inline T_Vector3 operator + (const T_Vector3& src) const
		{
			T_Vector3 ret;
			ret.x = x + src.x;
			ret.y = y + src.y;
			ret.z = z + src.z;

			return ret;
		}

		inline T_Vector3 operator - (const T_Vector3& src) const
		{
			T_Vector3 ret;
			ret.x = x - src.x;
			ret.y = y - src.y;
			ret.z = z - src.z;

			return ret;
		}

		inline T_Vector3 operator * (FloatType scale) const
		{
			T_Vector3 ret;
			ret.x = x * scale;
			ret.y = y * scale;
			ret.z = z * scale;

			return ret;
		}

		inline T_Vector3 operator / (FloatType scale) const
		{
			T_Vector3 ret;
			ret.x = x / scale;
			ret.y = y / scale;
			ret.z = z / scale;

			return ret;
		}

		inline bool operator == (const T_Vector3& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
		inline bool operator != (const T_Vector3& rhs) const
		{
			return x != rhs.x || y != rhs.y || z != rhs.z;
		}

#if defined(_WIN32)        
		__declspec(property(get = GetLengthSq)) FloatType LengthSq;
		__declspec(property(get = GetLength)) FloatType Length;
#endif

		inline T_Vector3 GetNormal() const
		{
			FloatType length = GetLength();
			return (length == 0) ? T_Vector3(0, 0, 0) : (*this) / length;
		}

		inline FloatType GetLengthSq() const
		{
			return x*x + y*y + z*z;
		}

		inline FloatType GetLength() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		static inline T_Vector3 Lerp(const T_Vector3 &a, const T_Vector3 &b, FloatType ratio)
		{
			T_Vector3 ret;
			ret.x = ScalarLerp(a.x, b.x, ratio);
			ret.y = ScalarLerp(a.y, b.y, ratio);
			ret.z = ScalarLerp(a.z, b.z, ratio);

			return ret;
		}

		static inline FloatType Dot(const T_Vector3 &a, const T_Vector3 &b)
		{
			return a.x*b.x + a.y*b.y + a.z*b.z;
		}

		inline FloatType Distance(const T_Vector3 &a) const
		{
			T_Vector3 d(a.x - x, a.y - y, a.z - z);
			return d.GetLength();
		}

		inline FloatType DistanceSq(const T_Vector3 &a) const	//Square Á¦°ö
		{
			T_Vector3 d(a.x - x, a.y - y, a.z - z);
			return d.GetLengthSq();
		}

		inline void SetLength(FloatType length)
		{
			T_Vector3 n = GetNormal();
			n *= length;
			*this = n;
		}

		//inline T_Vector3 YawToDir(float Yaw) const
		//{
		//	//const T_Vector3 Forward(0.0f, 0.0f, -1.0f);
		//	T_Vector3 Forward(0.0f, 0.0f, -1.0f);
		//	T_Vector3 dir(0.0f, 0.0f, 0.0f);

		//	Yaw = Yaw / 180.0f *  M_PI;
		//	
		//	dir.z = Forward.z * cos(Yaw) + Forward.x * sin(Yaw);
		//	dir.x = Forward.x * cos(Yaw) + Forward.z * sin(Yaw);
		//	return dir;
		//}

	};

	template <typename FloatType>
	inline T_Vector3<FloatType> operator * (FloatType scale, const struct T_Vector3<FloatType>& src)
	{
		T_Vector3<FloatType> ret;
		ret.x = scale * src.x;
		ret.y = scale * src.y;
		ret.z = scale * src.z;
		return ret;
	}

	//typedef Vector3T<double> Vector3;
	//typedef Vector3T<float> FloatVector3;
	typedef T_Vector3<float> Vec3;

	inline Vec3 YawToDir(float Yaw)
	{
		const Vec3 Forward(0.0f, 0.0f, 1.0f);
		Vec3 dir(0.0f, 0.0f, 0.0f);

		Yaw = (float)(Yaw / 180.0f * M_PI);

		dir.z = (float)(Forward.z * cos(Yaw) + Forward.x * sin(Yaw));
		dir.x = (float)(Forward.x * cos(Yaw) + Forward.z * sin(Yaw));
		return dir;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <typename FloatType>
	struct T_Rect
	{
		FloatType left;
		FloatType top;
		FloatType right;
		FloatType bottom;
	public:
		inline T_Rect() {};
		inline T_Rect(const FloatType * from) { left = from[0]; top = from[1]; right = from[2]; bottom = from[3]; }
		inline T_Rect(FloatType left, FloatType top, FloatType right, FloatType bottom) {
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}

		inline FloatType Width() const { return right - left; }
		inline FloatType Height() const { return top - bottom; }
		inline fSize Size() const { return fSize(right - left, bottom - top); }
		inline fPoint Center() const { return fPoint((left + right) / 2, (top + bottom) / 2); }
		inline bool IsRectNull() const { return (left == 0 && right == 0 && top == 0 && bottom == 0); }
		inline bool PtInRect(fPoint pt) const { return (pt.x > left && pt.x < right && pt.y > top && pt.y < bottom); }
	};

	typedef T_Rect<float> fRect;
}


#ifdef _MSC_VER
#pragma pack(pop)
#endif