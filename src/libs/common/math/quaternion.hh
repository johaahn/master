/***********************************************************************
 ** quaternion.hh
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
#ifndef COMMON_QUATERNION_HH_
#define COMMON_QUATERNION_HH_

/**
 * @file quaternion.hh
 * Quaternion class definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2014
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

/***********************************************************************
 * Defines
 ***********************************************************************/

/* Math stuff */
#include <float.h>
#include <Eigen/Core>

class Quaterniond: public Eigen::Vector4d {
public:
	Quaterniond() : Eigen::Vector4d (1,0,0,0) {

	}

	void f_set_from_rpy(Eigen::Vector3d const & in_s_rpy) {

		(*this)[0] = ::cos(in_s_rpy[2] / 2.0) * ::cos(in_s_rpy[0] / 2.0)
				* ::cos(in_s_rpy[1] / 2.0)
				+ ::sin(in_s_rpy[2] / 2.0) * ::sin(in_s_rpy[0] / 2.0)
						* ::sin(in_s_rpy[1] / 2.0);
		(*this)[1] = ::cos(in_s_rpy[2] / 2.0) * ::sin(in_s_rpy[0] / 2.0)
				* ::cos(in_s_rpy[1] / 2.0)
				- ::sin(in_s_rpy[2] / 2.0) * ::cos(in_s_rpy[0] / 2.0)
						* ::sin(in_s_rpy[1] / 2.0);
		(*this)[2] = ::cos(in_s_rpy[2] / 2.0) * ::cos(in_s_rpy[0] / 2.0)
				* ::sin(in_s_rpy[1] / 2.0)
				+ ::sin(in_s_rpy[2] / 2.0) * ::sin(in_s_rpy[0] / 2.0)
						* ::cos(in_s_rpy[1] / 2.0);
		(*this)[3] = ::sin(in_s_rpy[2] / 2.0) * ::cos(in_s_rpy[0] / 2.0)
				* ::cos(in_s_rpy[1] / 2.0)
				- ::cos(in_s_rpy[2] / 2.0) * ::sin(in_s_rpy[0] / 2.0)
						* ::sin(in_s_rpy[1] / 2.0);
	}

	Eigen::Vector3d f_trans(Eigen::Vector3d const & in_s) const {
		Eigen::Vector3d out_s;

		const double & a0 = (*this)[0];
		const double & ax = (*this)[1];
		const double & ay = (*this)[2];
		const double & az = (*this)[3];

		const double & v0 = in_s[0];
		const double & vx = in_s[1];
		const double & vy = in_s[2];
		//const float & vz = 0;

		out_s[0] = -ax * (-az * vy - ay * vx - ax * v0)
				+ a0 * (+ay * vy - az * vx + a0 * v0)
				- az * (-ax * vy + a0 * vx + az * v0)
				- ay * (-a0 * vy - ax * vx + ay * v0);

		out_s[1] = -ay * (-az * vy - ay * vx - ax * v0)
				- az * (-ay * vy + az * vx - a0 * v0)
				+ a0 * (-ax * vy + a0 * vx + az * v0)
				- ax * (+a0 * vy + ax * vx - ay * v0);

		out_s[2] = -az * (-az * vy - ay * vx - ax * v0)
				- ay * (+ay * vy - az * vx + a0 * v0)
				- ax * (+ax * vy - a0 * vx - az * v0)
				+ a0 * (+a0 * vy + ax * vx - ay * v0);

		return out_s;
	}

	Quaterniond f_conj(void) {
		Quaterniond c_tmp;
		c_tmp[0] = (*this)[0];
		c_tmp[1] = -(*this)[1];
		c_tmp[2] = -(*this)[2];
		c_tmp[3] = -(*this)[3];
		return c_tmp;
	}
};

#endif
