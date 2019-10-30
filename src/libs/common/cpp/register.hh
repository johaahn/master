/***********************************************************************
 ** register.hh
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

#ifndef REGISTER_HH_
#define REGISTER_HH_

/**
 * @file register.hh
 * Register management template class.
 *
 * @author SEAGNAL (johann.baudy@seagnal.fr)
 * @date 2015
 *
 * @version 1.0 Original version
 */

/***********************************************************************
 * Includes
 ***********************************************************************/

#include <c/common.h>
#include <cpp/debug.hh>
#include <cpp/string.hh>

#include <vector>
#include <list>
#include <iomanip>
#include <string.h>
#include <sstream>
//#define LF_REGISTER_DEBUG_SET
template<typename T>
struct ST_REGISTER_OPERATOR {
	/* Offset inside word */
	uint32_t i_offset_bit;
	uint32_t i_offset_word;

	/* Mask of word */
	T i_mask;
	T i_mask_shifted;

	uint32_t i_size_op;
};

template<typename T>
struct ST_REGISTER_SEGMENT {
	/* Info of segment */
	std::string str_description;
	std::string str_name;

	uint32_t i_id;

	/* Size of word */
	size_t sz_bit;
	size_t sz_word;

	/* Operator list */
	std::list<ST_REGISTER_OPERATOR<T>> l_single_ops;
};

template<typename T, typename T_ENUM = uint32_t> class CT_REGISTER {
	std::string _str_name;

	uint32_t _i_offset_bit;
	uint32_t _i_offset_word;
	uint32_t _i_id_next;

	std::vector<ST_REGISTER_SEGMENT<T>> _v_segment;
public:

	CT_REGISTER() :
			_str_name("UNKNOWN") {

		_i_id_next = 0;
		_i_offset_bit = 0;
		_i_offset_word = 0;

	}
	void f_set_name(std::string const & in_str_name) {
		_str_name = in_str_name;
	}

	void f_push(T_ENUM in_i_id, size_t in_sz_bit, std::string in_str_name = "",
			std::string in_str_description = "") {

		if (_i_id_next != in_i_id) {
			throw _FATAL<< "Id does not match previous id" <<_V(in_i_id)<<_V(_i_id_next) <<_V(in_str_name);
		}
		if(in_sz_bit == 0) {
			throw _FATAL << "Size not filled" <<_V(in_sz_bit) <<_V(in_str_name);
		}

		ST_REGISTER_SEGMENT<T> s_tmp;

		s_tmp.str_description = in_str_description;
		s_tmp.str_name = in_str_name;
		s_tmp.sz_bit = 0;
		s_tmp.sz_word = 0;
		s_tmp.i_id = in_i_id;

		/* reset write offset */

		size_t i_size = in_sz_bit;
		while(i_size) {
			ST_REGISTER_OPERATOR<T> s_op;

			size_t i_size_op = M_MIN(i_size, sizeof(T)*8-_i_offset_bit);
			if(i_size_op == sizeof(T)*8) {
				s_op.i_mask = T(-1);
			} else {
				s_op.i_mask =  (uint64_t(1) << i_size_op)-1;
			}
			s_op.i_offset_bit = _i_offset_bit;
			s_op.i_offset_word = _i_offset_word;
			s_op.i_mask_shifted = s_op.i_mask << _i_offset_bit;
			s_op.i_size_op = i_size_op;

			_i_offset_bit += i_size_op;
			M_ASSERT(_i_offset_bit <= sizeof(T)*8);

			if(_i_offset_bit == sizeof(T)*8) {
				_i_offset_bit = 0;
				_i_offset_word++;
			}

			i_size -= i_size_op;
			s_tmp.l_single_ops.push_back(s_op);
		}

		s_tmp.sz_bit += in_sz_bit;
		s_tmp.sz_word += ((in_sz_bit+(sizeof(T)*8-1))/(sizeof(T)*8));

		_v_segment.push_back(s_tmp);
		_i_id_next++;

	}

	void f_set(T_ENUM const in_i_id, T * in_pi_mem, size_t const in_sz_mem,
			T * in_pi_value, size_t const in_sz_value) {

		/* Get segment according to ID */
		//_DBG << _V(_v_segment.size()) << _V(in_i_id);
		M_ASSERT(_v_segment.size() > in_i_id);
		ST_REGISTER_SEGMENT<T>& s_segment = _v_segment[in_i_id];
		if(s_segment.sz_word > in_sz_value) {
			_CRIT << "???" << _V(s_segment.sz_word) << _V(in_sz_value);
			_FATAL << "segment "<< std::dec <<_V(s_segment.str_name) <<_V((int)s_segment.sz_bit) <<_V((int)s_segment.sz_word)<< _V((int)s_segment.i_id);
		}
		M_ASSERT(s_segment.sz_word <= in_sz_value);

		/* Initialize tmp integer */
		T i_tmp = 0;
		uint32_t i_tmp_size_bit = 0;
		size_t i_value_offset = 0;

#ifdef LF_REGISTER_DEBUG_SET
		std::cout << "segment "<< std::dec <<_V(s_segment.str_name) <<_V((int)s_segment.sz_bit) <<_V((int)s_segment.sz_word)<< _V((int)s_segment.i_id) << std::endl;
#endif

		/* Parse all ops */
		for (auto pc_it = s_segment.l_single_ops.begin(); pc_it != s_segment.l_single_ops.end();
				pc_it++) {

#ifdef LF_REGISTER_DEBUG_SET
			std::cout << "-ops: " << std::dec <<_V((int)pc_it->i_offset_word) << _V((int)pc_it->i_offset_bit)
			<< _V((int)pc_it->i_offset_word) <<_V((int)pc_it->i_size_op) << std::hex <<_V((int)pc_it->i_mask_shifted)
			<<_V((int)pc_it->i_mask) << std::endl;
#endif

			T i_mem_base = in_pi_mem[pc_it->i_offset_word];
			T i_mem_base_cleared = i_mem_base&(~pc_it->i_mask_shifted);

			uint32_t i_ops_size_bit = pc_it->i_size_op;
			uint32_t i_offset_bit = pc_it->i_offset_bit;

			/* Loop while ops not closed */
			while(i_ops_size_bit) {

				/* Load tmp integer */
				if(i_tmp_size_bit == 0) {
					i_tmp = in_pi_value[i_value_offset];
					i_tmp_size_bit = sizeof(i_tmp)*8;
#ifdef LF_REGISTER_DEBUG_SET
					std::cout << " l"<<_V(i_value_offset) << _V(in_sz_value) <<_V(i_tmp) <<  std::endl;
#endif
					i_value_offset++;

					M_ASSERT(i_value_offset <= in_sz_value);
				}


				/* Common size between ops and tmp integer */
				size_t i_sz_common_bit = M_MIN(i_ops_size_bit, i_tmp_size_bit);
				T i_mask_common = ((1<<i_sz_common_bit)-1);
				if(i_sz_common_bit == sizeof(T)*8) {
					i_mask_common = T(-1);
				}
				//T i_mask_common = ((uint32_t(1)<<uint32_t(i_sz_common_bit)));
				//i_mask_common -= 1;

				T i_value_common = i_tmp&i_mask_common;
#ifdef LF_REGISTER_DEBUG_SET
				std::cout << std::hex << " ~"<< _V((uint)i_tmp) << _V((uint)i_value_common)<< _V((uint)i_ops_size_bit)<< _V((uint)i_tmp_size_bit) << _V((uint)i_mask_common) << std::endl;
#endif
				/* Shift value with current offset */
				i_value_common <<= i_offset_bit;
				i_offset_bit += i_sz_common_bit;

				/* Add value to membase */
				i_mem_base_cleared |= i_value_common;

				/* Reduce tmp integer */
				i_tmp >>= i_sz_common_bit;
				i_tmp_size_bit -= i_sz_common_bit;

				/* Reduce current ops */
				i_ops_size_bit -= i_sz_common_bit;
			}

#ifdef LF_REGISTER_DEBUG_SET
			_DBG << "Write" << std::hex << _V((uint)i_mem_base_cleared) << _V((uint)pc_it->i_offset_word);
#endif

			/* Update memory */
			in_pi_mem[pc_it->i_offset_word] = i_mem_base_cleared;
		}

	}

	size_t f_get(T_ENUM in_i_id, T * in_pi_value,
			size_t const in_sz_value, T const * const in_pi_mem, size_t const in_sz_mem) {

		/* Get segment according to ID */
		M_ASSERT(_v_segment.size() > in_i_id);
		ST_REGISTER_SEGMENT<T>& s_segment = _v_segment[in_i_id];
		M_ASSERT(s_segment.sz_word <= in_sz_value);

		/* Initialize tmp integer */
		T i_tmp = 0;
		T i_tmp_size_bit = sizeof(T)*8;
		T i_offset_bit = 0;
		size_t i_value_offset = 0;
#if 0
		std::cout << "segment "<<_V(s_segment.str_name) <<_V((int)s_segment.sz_bit) <<_V((int)s_segment.sz_word)<< _V((int)s_segment.i_id) << std::endl;
#endif
		/* Parse all ops */
		for (auto pc_it = s_segment.l_single_ops.begin(); pc_it != s_segment.l_single_ops.end();
				pc_it++) {
#if 0
			std::cout << "-ops: " <<_V((int)pc_it->i_offset_word) << _V((int)pc_it->i_offset_bit)
			<< _V((int)pc_it->i_offset_word) <<_V((int)pc_it->i_size_op) << std::hex <<_V((int)pc_it->i_mask_shifted)
			<<_V((int)pc_it->i_mask) << std::endl;
#endif

			T i_mem_base = in_pi_mem[pc_it->i_offset_word];

			T i_ops_value = i_mem_base&(pc_it->i_mask_shifted);
			i_ops_value >>= pc_it->i_offset_bit;
			T i_ops_size_bit = pc_it->i_size_op;
			//std::cout << std::hex << _V((int)pc_it->i_offset_word) << _V((int)i_mem_base) << std::endl;

			/* Loop while ops not closed */
			while(i_ops_size_bit) {
				//std::cout << _V((uint)i_tmp_size_bit) <<_V((uint)i_value_offset) << std::hex <<_V((uint)i_tmp)<<_V((uint)in_sz_value) << std::endl;
				//std::cout<< _V((uint)i_ops_size_bit)<< std::endl;
				/* Store tmp integer */
				if(i_tmp_size_bit == 0) {
					if(i_value_offset < in_sz_value) {
#if 0
						_DBG << "Write" << std::hex << _V((uint)i_tmp)<< _V(i_value_offset);
#endif
						in_pi_value[i_value_offset] = i_tmp;
						i_value_offset++;
					} else {
						_CRIT << "Received buffer is too small";
					}
					i_tmp_size_bit = sizeof(T)*8;
					i_tmp = 0;
					i_offset_bit = 0;
					//M_ASSERT(i_value_offset <= in_sz_value);
				}

				/* Common size between ops and tmp integer */
				size_t i_sz_common_bit = M_MIN(i_ops_size_bit, i_tmp_size_bit);
				T i_mask_common = ((1<<i_sz_common_bit)-1);
				if(i_sz_common_bit == sizeof(T)*8) {
					i_mask_common = T(-1);
				}
				T i_value_common = i_ops_value&i_mask_common;

				/* Shift value with current offset */
				i_value_common <<= i_offset_bit;
				i_offset_bit += i_sz_common_bit;
				//std::cout << _V((uint)i_offset_bit)<< std::endl;
				//std::cout << std::hex << _V((uint)i_value_common)<< std::endl;
				/* Add value to membase */
				i_tmp |= i_value_common;

				/* Reduce ops integer */
				i_tmp_size_bit -= i_sz_common_bit;

				/* Reduce current ops */
				i_ops_size_bit -= i_sz_common_bit;
				i_ops_value >>= i_sz_common_bit;
			}

		}
#if 0
		std::cout  << _V((uint)i_tmp_size_bit) <<_V((uint)i_value_offset) << std::hex <<_V((uint)i_tmp)<<_V((uint)in_sz_value) <<std::endl;
#endif
		/* Store tmp integer */
		if(i_tmp_size_bit != sizeof(T)*8) {
			if(i_value_offset < in_sz_value) {
#if 0
				_DBG << "Write"<< std::hex  << _V((uint)i_tmp) << _V(i_value_offset);
#endif
				in_pi_value[i_value_offset] = i_tmp;
				i_value_offset++;
			} else {
				_CRIT << "Received buffer is too small";
			}
			//_DBG << _V(i_value_offset)<< _V(in_sz_value);
			//M_ASSERT(i_value_offset == in_sz_value);
		}
		//_DBG << i_value_offset;
		return i_value_offset;
	}
	void f_print(T const * const in_pi_mem, size_t const in_sz_mem) {
		T ai_tmp[32];
		int ec;

		std::cout << "Register:" << _str_name << std::endl;
		_DBG << std::hex << in_pi_mem[0] << " " << in_pi_mem[1] << " "<< in_pi_mem[2] << _V(in_sz_mem);
		for (auto pc_it = _v_segment.begin(); pc_it != _v_segment.end();
				pc_it++) {
			memset(ai_tmp, 0, sizeof(ai_tmp));
			ec = f_get((T_ENUM) pc_it->i_id, ai_tmp, sizeof(ai_tmp)/sizeof(T), in_pi_mem,
					in_sz_mem);
			//_DBG << ec;
			std::cout << std::setw(50) << std::right << pc_it->str_name
					<< " : "<< std::setw(30) << std::left <<f_string_from_buffer(ai_tmp, ec)
					<< "  " << std::setw(10) << std::left << int(*ai_tmp)
					<< " -- " << pc_it->str_description<< std::endl;
		}
	}

	std::string f_dump_csv(T const * const in_pi_mem, size_t const in_sz_mem) {
		std::stringstream ss;
		T ai_tmp[32];
		int ec;

		for (auto pc_it = _v_segment.begin(); pc_it != _v_segment.end();
				pc_it++) {
			memset(ai_tmp, 0, sizeof(ai_tmp));
			ec = f_get(pc_it->i_id, ai_tmp, sizeof(ai_tmp)/sizeof(T), in_pi_mem,
					in_sz_mem);
			//_DBG << ec;
			ss << _str_name << "," << pc_it->i_id << ","<< pc_it->str_name << "," << f_string_from_buffer(ai_tmp, ec) << ","<< int(*ai_tmp) << std::endl;
		}

		return ss.str();
	}


	template< typename T2>
	void f_set(T_ENUM in_i_id, T * const in_pi_mem,
			T2 in_i_value) {
		//_DBG << _V(sizeof(T))<< _V(sizeof(T2));
		f_set(in_i_id, in_pi_mem, 1, (T*)&in_i_value,
				sizeof(T2)/sizeof(T));
	}

	template< typename T2>
	T2 f_get(T_ENUM in_i_id, T * const in_pi_mem, size_t const in_sz_mem) {
		T2 tmp = 0;
		f_get(in_i_id, (T*)&tmp, sizeof(T2)/sizeof(T), in_pi_mem,
				in_sz_mem);
		return tmp;
	}

	template< typename T2>
	std::vector<T2> f_get_vector(T_ENUM in_i_id, T * const in_pi_mem, size_t const in_sz_mem) {
		M_ASSERT(_v_segment.size() > in_i_id);
		ST_REGISTER_SEGMENT<T>& s_segment = _v_segment[in_i_id];
		//_DBG << s_segment.sz_word;
		std::vector<T2> tmp((s_segment.sz_word*sizeof(T) + (sizeof(T2)-1))/sizeof(T2));
		f_get(in_i_id, (T*)tmp.data(), s_segment.sz_word, in_pi_mem,
				in_sz_mem);
		return tmp;
	}


#if 0
	CT_REGISTER(std::string const &);

	void f_push(uint32_t in_i_id, size_t in_sz_bit,
			std::string in_str_name, std::string in_str_description);

	int f_print(uint64_t * in_pi_value, size_t in_sz_value);

	void f_set64(uint32_t in_i_id,
			uint64_t * in_pi_mem,
			uint64_t in_i_value
	);

	uint64_t f_get64(uint32_t in_i_id,
			uint64_t * in_pi_value,
			uint64_t * in_pi_mem
	);
#endif
};

#endif /* REGISTER_HH_ */
