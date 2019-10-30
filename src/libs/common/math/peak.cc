/***********************************************************************
 ** fftw.c
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

#include <math/peak.hh>
#include <cpp/debug.hh>


static int f_peak_find_width(
		ST_PEAK & out_s_peak,
		float const * const in_pf_data,
		size_t const in_sz_data,
		int const in_i_index,
		float const in_f_value,
		float const in_f_snr_from_max_time,
		int const in_width_analysis,
		unsigned int const in_i_width_modulation) {

		//_DBG << _V(in_pf_data)  << _V(in_pf_data) <<_V(in_pf_data)<<_V(in_i_index) <<_V(in_f_value) <<_V(in_width_analysis);
		/* Looking for pulse inside pulse */
		float f_snr_pf = in_f_snr_from_max_time;
		/** Looking for new detections */
		out_s_peak.i_right_margin = 0;
		out_s_peak.i_left_margin = 0;
		out_s_peak.f_left_acc = 0.0;
		out_s_peak.i_left_acc = 0;
		out_s_peak.i_index = in_i_index;
		out_s_peak.f_value = in_f_value;
		uint32_t i_left_margin_cnt = 0;
		uint32_t i_right_margin_cnt = 0;
		out_s_peak.f_right_acc = 0.0;
		out_s_peak.i_right_acc = 0;

		/* Parse left */
		for (int i_parse = 1; i_parse < in_width_analysis; i_parse++) {
			int i_new_index = M_MAX(out_s_peak.i_index - i_parse, 0);
			if (i_new_index < 0) {
				break;
			}
			float f_new = in_pf_data[i_new_index];
		  //_DBG  << "L " << _V(i_parse)  << _V(i_new_index) <<_V(f_new) << _V(in_f_value) << _V(in_f_value - f_new) <<_V(f_snr_pf) << _V(in_i_width_modulation);

			if (i_left_margin_cnt < in_i_width_modulation ) {
				/* Discard maximum if border is not found yet */
				if (f_new > out_s_peak.f_value) {
					break;
				}
				/* Looking for border */
				if (out_s_peak.f_value - f_new > f_snr_pf) {
					out_s_peak.i_left_margin = i_parse;
					i_left_margin_cnt ++;

				} else {
					i_left_margin_cnt = 0;
          out_s_peak.f_left_acc = 0;
          out_s_peak.i_left_acc = 0;
				}
			} else {
				out_s_peak.f_left_acc += (f_new);
				out_s_peak.i_left_acc++;
			}
			if(i_parse == 1) {
				out_s_peak.f_left_acc = 0;
			}
		}
		if(i_left_margin_cnt != in_i_width_modulation) {
			out_s_peak.i_left_margin = 0;
		}
		if (out_s_peak.i_left_margin) {
			/* Parse right */
			for (int i_parse = 1; i_parse < in_width_analysis; i_parse++) {
				int i_new_index = out_s_peak.i_index + i_parse;
				if (i_new_index >= int(in_sz_data)) {
					break;
				}
				float f_new = in_pf_data[i_new_index];
				//_DBG  << "R " << _V(i_parse)  << _V(i_new_index) <<_V(f_new) <<_V(in_f_value) <<_V(in_f_value - f_new) <<_V(f_snr_pf);

				if (i_right_margin_cnt < in_i_width_modulation ) {
					/* Discard maximum if border is not found yet */
					if (f_new > out_s_peak.f_value) {
						break;
					}
					/* Looking for border */
					if (out_s_peak.f_value - f_new > f_snr_pf) {
						out_s_peak.i_right_margin = i_parse;
						i_right_margin_cnt ++;
					} else {
						i_right_margin_cnt = 0;
            out_s_peak.f_right_acc = 0;
            out_s_peak.i_right_acc = 0;
					}
				} else {
					out_s_peak.f_right_acc += (f_new);
					out_s_peak.i_right_acc++;
				}
			}
		}
		if(i_right_margin_cnt != in_i_width_modulation) {
			out_s_peak.i_right_margin = 0;
		}

	return EC_SUCCESS;
}

int f_peak_find(std::list<ST_PEAK> & out_l_peak,
		Eigen::VectorXf const & in_c_vect,
		float const in_f_snr_from_max_time,
		float const in_f_level_limit,
		int const in_width_analysis,
		int const in_i_width_modulation) {

	float const *pf_u = &in_c_vect(0);
	Eigen::Map<Eigen::VectorXf const> c_mat_map(pf_u, in_c_vect.size());

	return f_peak_find(out_l_peak, pf_u, in_c_vect.size(), in_f_snr_from_max_time, in_f_level_limit, in_width_analysis, in_i_width_modulation);
}

int f_peak_find(std::list<ST_PEAK> & out_l_peak,
		float const * const in_pf_data,
		size_t const in_sz_data,
		float const in_f_snr_from_max_time,
		float const in_f_level_limit,
		int const in_width_analysis,
		int const in_i_width_modulation) {

	/* Looking for pulse inside pulse */
	float f_unuse_limit = in_f_level_limit;
	float f_snr_pf = in_f_snr_from_max_time;
	for (int i = 1; i < (int)in_sz_data; i++) {
		float f_current = in_pf_data[i];
		if (f_current < f_unuse_limit) {
			continue;
		}


		/** Looking for new detections */
		ST_PEAK s_peak;

		/* Parse left */
		f_peak_find_width(s_peak, in_pf_data, in_sz_data, i, f_current, f_snr_pf, in_width_analysis, in_i_width_modulation);

		/* If valid push it */
		if (s_peak.i_right_margin && s_peak.i_left_margin) {
      /* Use left only for noise calculation ~ Reverb ... */
      s_peak.f_noise = s_peak.f_left_acc/s_peak.i_left_acc;
#if 0
      std::cout << "NEW PIC !!!!" << _V(s_peak.f_value) << std::dec << _V(s_peak.i_index)
					<< _V(s_peak.i_left_margin) << _V(s_peak.i_right_margin) << _V(s_peak.f_left_acc)
					<< _V(s_peak.i_left_acc) << _V(s_peak.f_left_acc / s_peak.i_left_acc) << _V(s_peak.f_noise)
					<< std::endl;
#endif
			out_l_peak.push_back(s_peak);

		}
		//_DBG<< _V(i) << _V(af_pattern_centered[i]) << _V(af_pattern[i]) <<_V(i_left_margin) << _V(i_right_margin);
#if 0
		if(i_res_bottom == 0) {
			if (af_pattern_dopp[i] > -_s_cdetect_config.f_snr_from_max_time) {
				i_res_bottom = i;
				break;
			}
		}
#endif
	}

	return EC_SUCCESS;
}

bool f_peak_level_sorter(ST_PEAK const& lhs, ST_PEAK const& rhs)
{
	 return lhs.f_value > rhs.f_value;
}


int f_peak_interpolate_max(ST_PEAK_INTERPOLATED & out_s_peak_interpolated, ST_PEAK const & in_s_peak, Eigen::VectorXf const & in_c_vect) {
	if(in_s_peak.i_index == 0) {
		out_s_peak_interpolated.f_index = in_s_peak.i_index;
		out_s_peak_interpolated.f_value = in_s_peak.f_value;
	} else if(in_s_peak.i_index == in_c_vect.size()-1) {
		out_s_peak_interpolated.f_index = in_s_peak.i_index;
		out_s_peak_interpolated.f_value = in_s_peak.f_value;
	} else {
		auto f_value_right = in_c_vect[in_s_peak.i_index+1];
		auto f_value_mid = in_s_peak.f_value;
		auto f_value_left = in_c_vect[in_s_peak.i_index-1];
    float f_dx;

		/* interpolation of maximum position */
		f_peak_parabolic_interpolation(out_s_peak_interpolated.f_value, f_dx, f_value_left, f_value_mid, f_value_right);
    out_s_peak_interpolated.f_index = float(in_s_peak.i_index) + f_dx;
    // _DBG << _V(in_s_peak.i_index) << _V(out_s_peak_interpolated.f_index);
	}
  out_s_peak_interpolated.f_noise = in_s_peak.f_noise;
  return EC_SUCCESS;
}

void f_peak_parabolic_interpolation(
  float & out_f_ymax,
  float & out_f_dx,
  float in_f_yleft,
  float in_f_ymid,
  float in_f_yright
) {

  float dy = 0.5*(in_f_yright-in_f_yleft);
  float d2y = 2.0*in_f_ymid - in_f_yright - in_f_yleft;
  float dx = dy/d2y;

  out_f_ymax = in_f_ymid + 0.5*(dy*dy)/d2y;
  out_f_dx = dx;
  //_DBG << _V(out_f_ymax) << _V(out_f_dx) << _V(in_f_yleft) << _V(in_f_ymid) << _V(in_f_yright);
}
