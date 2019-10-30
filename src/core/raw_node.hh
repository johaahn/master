

class CT_PORT_NODE_RAW: public node<T, CT_GUARD> {


}


template<typename T, template<class > class G>
int raw_node<T, G>::from_parser(node_parser<G> & in_c_parser) {
	clear();
  bool b_loop = true;

  node_resource_segment<G> c_tmp;
  c_tmp->resize(1024);
  uint32_t i_offset = 0;

	while (b_loop) {
    uint8_t i_data;
		/* Read flags */
		ec = in_c_parser.parse_data(&i_data, 1);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}

    ai_buffer[i_offset] = i_data;
    i_offset++;

    if(i_offset == c_tmp.size()) {
      b_loop = false;
    }

    if(i_data == _i_sep) {
      b_loop = false;
    }
	}

  /* Update data */
  set_segment(c_tmp, 0, i_offset);

	on_read();

	return EC_BML_SUCCESS;
}


template<typename T, template<class > class G>
int raw_node<T, G>::to_writer(node_writer<G> & in_c_writer, int in_i_align) {
  size_t sz_buffer = _s_segment._pc_resource ? _s_segment._sz_data : 0;

	on_write();

	M_ASSERT(sz_buffer == _s_segment.size());
	ec = in_c_writer.write_resource(_s_segment);
	if (ec != EC_BML_SUCCESS) {
		DBG_LINE();
		return ec;
	}

	return EC_BML_SUCCESS;
}
