/***********************************************************************
 ** misc
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
#ifndef MISC_HH_
#define MISC_HH_

/**
 * @file misc.hh
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date Feb 10, 2013
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <string>
#include <vector>
#include <limits>

/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/

/***********************************************************************
 * Functions
 ***********************************************************************/

/* Files */
bool f_file_exits (const std::string& in_str_name);
time_t f_file_touch(const std::string& in_str_pathname);

/* Base 64 */
std::string f_base64_encode(char const* in_pc_buf, size_t in_sz_buf);
std::vector<char> f_base64_decode(std::string const& in_str);

/* Math overlap */
template<typename T>
inline T f_get_overlap(T const & left_1, T const & right_1, T const & left_2,
		T const & right_2) {
	M_ASSERT(right_1 >= left_1);
	M_ASSERT(right_2 >= left_2);

	if (left_1 > right_2) {
		return right_2 - left_1;
	}
	if (left_2 > right_1) {
		return right_1 - left_2;
	}

	T left_tmp = M_MAX(left_1,left_2);
	T right_tmp = M_MIN(right_1,right_2);

	return right_tmp - left_tmp;
}

template<typename T>
inline T f_get_overlap_mod(T const & left_1, T const & right_1, T const & left_2,
		T const & right_2, T const mod) {
	T tmp = -std::numeric_limits<float>::max();
	for(int i=-1; i<=1; i++)
	//int i = 0;
	{
		M_ASSERT((right_1+T(i)*mod) >= (left_1+T(i)*mod));
		M_ASSERT(right_2 >= left_2);
		T dist;

		if ((left_1+T(i)*mod) > right_2) {
			dist = right_2 - (left_1+T(i)*mod);
		}
		else if (left_2 > (right_1+T(i)*mod)) {
			dist = (right_1+T(i)*mod) - left_2;
		} else {
			T left_tmp = M_MAX((left_1+T(i)*mod),left_2);
			T right_tmp = M_MIN((right_1+T(i)*mod),right_2);

			dist = right_tmp - left_tmp;
		}
		//_DBG << _V(dist);
		tmp = M_MAX(tmp, dist);
	}

	return tmp;
}

#endif /* MISC_HH_ */
