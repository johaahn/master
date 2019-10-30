/***********************************************************************
 ** fir.cc
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

/**
 * @file fir.cc
 * Filter functions
 *
 * @author SEAGNAL (romain.pignatari@seagnal.fr)
 * @date 2015
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <math.h>
#include "fir.hh"


template<typename T>

T sinc(T x) {

	if (x) {
		T y = M_PI*x;
		return sin(y) / (y);
	}

	return 1;

}

template<typename T>
T my_hamming(int N, int ii) {

	T arg = 2*M_PI*((T)ii)/((T)N);
	T res = 0.54 - 0.46 * cos(arg);

	return res;

}

template<typename T>
T sum(T *l, int len) {

	int i;
	double s = 0;
	for (i=0;i<len;i++) {
		s += l[i];
	}

	return s;

}

template<typename T>
void fir1(T *out, int N, T cutoff) {
  //M_ASSERT(cutoff <= 1);
	int i;
	for (i=0;i<N+1;i++){
		out[i] = sinc<T>(cutoff * (((T)i)-((T)(N))/2.0)) * my_hamming<T>(N, i);
	}

	T s = sum(out, N+1);

	for (i=0;i<N+1;i++){
		out[i] /= s;
	}
}


template void fir1<float>(float *out, int N, float cutoff);
template void fir1<double>(double *out, int N, double cutoff);

template float sinc<float>(float x);
template double sinc<double>(double x);
