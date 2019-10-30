/***********************************************************************
** kalman.hh
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
#ifndef KALMAN_HH_
#define KALMAN_HH_

/**
 * @file kalman.hh
 * Kalmam math class definition.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2014
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/
#include <Eigen/Core>
#include <Eigen/Dense>
/***********************************************************************
 * Namespaces
 ***********************************************************************/

/***********************************************************************
 * Defines
 ***********************************************************************/

/***********************************************************************
 * Types
 ***********************************************************************/
template<typename T_TYPE, int SIZE>
class CT_EKF {

	Eigen::Matrix<T_TYPE, SIZE, 1> X;
	Eigen::Matrix<T_TYPE, SIZE, SIZE> P;
	Eigen::Matrix<T_TYPE, SIZE, SIZE> I;

public:
	virtual ~CT_EKF() {
	}

	void f_init(const Eigen::Matrix<T_TYPE, SIZE, 1>& in_c_X, const Eigen::Matrix<T_TYPE, SIZE, SIZE>& in_c_P) {
		X = in_c_X;
		P = in_c_P;
		I = in_c_P;
		I.setIdentity();
	}

	virtual void f_predict(const Eigen::Matrix<T_TYPE, SIZE, 1>& in_c_dX, const Eigen::Matrix<T_TYPE, SIZE, SIZE>& in_c_F, const Eigen::Matrix<T_TYPE, SIZE, SIZE>& in_c_Q) {
		P = in_c_F*P*in_c_F.transpose() + in_c_Q;
		X = X + in_c_dX;
	}

	template <int SIZE_MEAS>
	void f_update(const Eigen::Matrix<T_TYPE, SIZE_MEAS, 1>& in_c_Z, const Eigen::Matrix<T_TYPE, 1, SIZE>& in_c_H, const Eigen::Matrix<T_TYPE, SIZE_MEAS, SIZE_MEAS>& in_c_R) {
		Eigen::Matrix<T_TYPE, SIZE_MEAS, 1> Y = in_c_Z - in_c_H*X;
		Eigen::Matrix<T_TYPE, SIZE, 1> Ht = in_c_H.transpose();

		/* Generate S */
		Eigen::Matrix<T_TYPE, SIZE_MEAS, SIZE_MEAS> S = (in_c_H*P*Ht+in_c_R);
		Eigen::Matrix<T_TYPE, SIZE_MEAS, SIZE_MEAS> Si = S.inverse();
		/* Generate Gain K */
		Eigen::Matrix<T_TYPE, SIZE, 1> K = P*Ht*Si;

		/* Update Cov matrix */
		P = (I - K*in_c_H)*P;

		/* Update STATE */
		X = X + K*Y;
	}


	Eigen::Matrix<T_TYPE, SIZE, 1>& f_get_state () {
		return X;
	}
};

#endif
