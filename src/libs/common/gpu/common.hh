/***********************************************************************
 ** common.hh
 ***********************************************************************
 ** Copyright (c) SEAGNAL SAS
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2.1 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 ** MA 02110-1301 USA
 **
 ** -------------------------------------------------------------------
 **
 ** As a special exception to the GNU Lesser General Public License, you
 ** may link, statically or dynamically, a "work that uses this Library"
 ** with a publicly distributed version of this Library to produce an
 ** executable file containing portions of this Library, and distribute
 ** that executable file under terms of your choice, without any of the
 ** additional requirements listed in clause 6 of the GNU Lesser General
 ** Public License.  By "a publicly distributed version of this Library",
 ** we mean either the unmodified Library as distributed by the copyright
 ** holder, or a modified version of this Library that is distributed under
 ** the conditions defined in clause 3 of the GNU Lesser General Public
 ** License.  This exception does not however invalidate any other reasons
 ** why the executable file might be covered by the GNU Lesser General
 ** Public License.
 **
 ***********************************************************************/

/* define against mutual inclusion */
#ifndef GPU_COMMON_HH_
#define GPU_COMMON_HH_

/**
 * @file gpu_common.hh
 * GPU Common types definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <cuda.h>
#include <cuda_runtime.h>
#include <iostream>


#include <float.h>

/***********************************************************************
 * Defines
 ***********************************************************************/
#ifndef __CUDACC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#ifdef ERROR_ECLIPSE
struct {
	int x;
	int y;
	int z;
} blockIdx, blockDim, gridDim, threadIdx;
#endif
#pragma GCC diagnostic pop
#define __syncthreads()
#define __mul24(a,b) (a*b)
#define atomicAdd(a,b) (a+b)
#include <math.h>
#endif

/***********************************************************************
 * Types
 ***********************************************************************/
inline __host__ __device__ float2 conj(float2 const& a) {
	return make_float2(a.x, -a.y);
}

inline __host__ __device__ double2 conj(double2 const& a) {
	return make_double2(a.x, -a.y);
}

inline __host__ __device__ float dotf(float2 const& a, float2 const& b) {
	return a.x * b.x + a.y * b.y;
}

inline __host__ __device__ float dotf(float3 const& a, float3 const& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline __host__ __device__ double dot(double2 const& a, double2 const& b) {
	return a.x * b.x + a.y * b.y;
}

inline __host__ __device__ double dot(double3 const& a, double3 const& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline __device__ float2 make_complex(float const &argument) {
	float2 s_tmp;
	sincosf(argument, &s_tmp.y, &s_tmp.x);
	return s_tmp;
}

inline __device__ double2 make_complex(double const &argument) {
	double2 s_tmp;
	sincos(argument, &s_tmp.y, &s_tmp.x);
	return s_tmp;
}

template<typename T>
inline __host__ __device__ float normf(T const &a) {
	return sqrtf(dotf(a, a));
}

template<typename T>
inline __host__ __device__ double norm(T const &a) {
	return sqrt(dot(a, a));
}

#if 0
inline __host__ __device__ double2 operator =(float2  const &a) {
	return make_double2(a.x, a.y);
}

inline __host__ __device__ float2 operator =(double2  const &a) {
	return make_float2(a.x, a.y);
}
#endif

inline __host__ __device__ float2 operator +(float2  const &a, float2  const &b) {
	return make_float2(a.x+b.x, a.y+b.y);
}

inline __host__ __device__ void operator +=(float2 &a, float2 const &b) {
	a.x += b.x;
	a.y += b.y;
}

inline __host__ __device__ float3 operator +(float3  const &a, float3  const &b) {
	return make_float3(a.x+b.x, a.y+b.y, a.z+b.z);
}

inline __host__ __device__ void operator +=(float3 &a, float3 const &b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

inline __host__   __device__ float2 operator /(float2 const &a, float  const &b) {
	return make_float2(a.x / b, a.y / b);
}
inline __host__ __device__ void operator /=(float2 &a, float const &b) {
	a.x /= b;
	a.y /= b;
}

inline __host__   __device__ float2 operator /(float2 const &a, float2  const &b) {
	float f_norm = b.x * b.x + b.y * b.y;
	return make_float2((a.x*b.x + a.y*b.y) / f_norm, (-a.x*b.y + a.y*b.x) / f_norm);
}
inline __host__ __device__ void operator /=(float2 &a, float2 const &b) {
	float f_norm = b.x * b.x + b.y * b.y;
	float f_ax = a.x;
	a.x = (a.x*b.x + a.y*b.y) / f_norm;
	a.y = (-f_ax*b.y + a.y*b.x) / f_norm;
}

inline __host__   __device__ float2 operator *(float2 const &a, float const &b) {
	return make_float2(a.x*b, a.y*b);
}
inline __host__   __device__ float2 operator *(float2 const &a, float2 const &b) {
	return make_float2(a.x * b.x - a.y * b.y,  a.y * b.x + a.x * b.y);
}

inline __host__   __device__ void operator *=(float2 &a, float2 const &b) {
	float f_ax = a.x;
	a.x = (a.x * b.x - a.y * b.y);
	a.y = (a.y * b.x + f_ax * b.y);
}

inline __host__ __device__ void operator *=(float2 &a, float const &b) {
	a.x *= b;
	a.y *= b;
}

inline __host__   __device__ float3 operator-(float3 const & a,
		float3 const & b) {
	return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline __host__ __device__ void operator -=(float3 &a, float3 const &b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}
inline __host__   __device__ float2 operator-(float2 const & a,
		float2 const & b) {
	return make_float2(a.x - b.x, a.y - b.y);
}
inline __host__ __device__ void operator -=(float2 &a, float2 const &b) {
	a.x -= b.x;
	a.y -= b.y;
}


inline __host__   __device__ double3 operator-(double3 const & a,
		double3 const & b) {
	return make_double3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline __host__ __device__ void operator -=(double3 &a, double3 const &b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}

template<typename T>
inline __device__   __host__ T clamp(T f, T a, T b) {
	return max(a, min(f, b));
}

inline __host__ __device__ void mult_complex(float2 &out, float2 &a,
		float2 &b) {
	out.x = a.x * b.x - a.y * b.y;
	out.y = a.y * b.x + a.x * b.y;
}

template<typename T>
inline __device__ T __max(T&a, T&b) {
	return M_MAX(a, b);
}
template<>
inline __device__ float2 __max(float2&a, float2&b) {
	return make_float2(M_MAX(a.x, b.x), M_MAX(a.y, b.y));
}

template<typename T>
inline __device__ T __zero() {
	return 0;
}
template<>
inline __device__ float2 __zero() {
	return make_float2(0, 0);
}

template<typename T>
inline __device__ T __max() {
	return FLT_MAX;
}
template<>
inline __device__ float2 __max() {
	return make_float2(FLT_MAX, FLT_MAX);
}

template<typename T>
inline __device__ T __min() {
	return -FLT_MAX;
}
template<>
inline __device__ float2 __min() {
	return make_float2(-FLT_MAX, -FLT_MAX);
}

// non-specialized class template
template<class T>
class SharedMem {
public:
	// Ensure that we won't compile any un-specialized types
	__device__ T* getPointer() {
		abort();
	}
	;
};

// specialization for int
template<>
class SharedMem<int> {
public:
	static __device__ int* getPointer() {
		extern __shared__ int s_int[];
		return s_int;
	}
};

// specialization for float
template<>
class SharedMem<float> {
public:
	static __device__ float* getPointer() {
		extern __shared__ float s_float[];
		return s_float;
	}
};

// specialization for float2
template<>
class SharedMem<float2> {
public:
	static __device__ float2* getPointer() {
		extern __shared__ float2 s_float2[];
		return s_float2;
	}
};
#endif
