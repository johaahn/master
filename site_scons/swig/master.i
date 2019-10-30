%include <stdint.i>
#ifdef SWIGPYTHON
%include <exception.i>
#endif

#ifdef SWIGOCTAVE
%define MASTER_STRUCT( name)
%extend name {
  name(octave_value in) {
    uint8NDArray tmp = in.uint8_array_value();
    size_t i_size = tmp.byte_size();
    if(i_size != sizeof(name)) {
      error("Error: size of name is invalid (%d != %d)\n",i_size, sizeof(name));
    }
    void const * pv_tmp = tmp.data();
    name *result = new name(*static_cast<const name*>(pv_tmp));
    return result;
  }
}
%enddef
%define MASTER_STRUCT_NS(namespace, name)
%extend namespace ## name {
  name(octave_value in) {
    uint8NDArray tmp = in.uint8_array_value();
    size_t i_size = tmp.byte_size();
    if(i_size != sizeof(name)) {
      error("Error: size of name is invalid (%d != %d)\n",i_size, sizeof(name));
    }
    void const * pv_tmp = tmp.data();
    name *result = new name(*static_cast<const name*>(pv_tmp));
    return result;
  }
}
%enddef
%typemap(memberin) float [ANY] {
  printf("TODO octave_value => FLOAT[] \n");
}
%typemap(out) float [ANY] {
  Matrix tmp( $1_dim0, 1 );
  for( unsigned int i = 0; i < $1_dim0; i++ )
  {
      tmp(i, 0) = $1[i];
  }
  $result = tmp;
}
%typemap(memberin) uint64_t [ANY] {
  printf("TODO octave_value => uint64_t[] \n");
}
%typemap(out) uint64_t [ANY] {
  uint64NDArray tmp( dim_vector ($1_dim0, 1) );
  for( unsigned int i = 0; i < $1_dim0; i++ )
  {
    tmp(i) = $1[i];
  }
  $result = tmp;
}
%typemap(memberin) uint32_t [ANY] {
  printf("TODO octave_value => uint32_t[] \n");
}
%typemap(out) uint32_t {
  $result = octave_uint32($1);
}
%typemap(out) uint32_t [ANY] {
  uint32NDArray tmp( dim_vector ($1_dim0, 1) );
  for( unsigned int i = 0; i < $1_dim0; i++ )
  {
    tmp(i) = $1[i];
  }
  $result = octave_value(tmp);
}
#endif

#ifdef SWIGPYTHON
%define MASTER_STRUCT(name)
%extend name {
  name(PyObject *in) {
    assert(PyObject_CheckBuffer(in));
    Py_buffer view;
    const int ret = PyObject_GetBuffer(in, &view, PyBUF_SIMPLE);
    assert(0==ret);

    /* Check size */
    if(view.len != sizeof(name)) {
      char errbuffer [2048];
      snprintf(errbuffer, sizeof(errbuffer), "Error: size of name is invalid (%d != %d)\n", (int)view.len, (int)sizeof(name));
      printf("%s\n", errbuffer);
      PyErr_SetString(PyExc_IndexError, errbuffer);
      throw std::string(errbuffer);
      return NULL;
    }

    /* Allocate memory */
    name *result = new name(*static_cast<const name*>(view.buf));
    PyBuffer_Release(&view); // Note you could/should retain view.obj for the life of this object to prevent use after free
    return result;
  }
}
%enddef
%define MASTER_STRUCT_NS(namespace, name)
%extend namespace ## name {
  name(PyObject *in) {
    assert(PyObject_CheckBuffer(in));
    Py_buffer view;
    const int ret = PyObject_GetBuffer(in, &view, PyBUF_SIMPLE);
    assert(0==ret);

    /* Check size */
    if(view.len != sizeof(name)) {
      char errbuffer [2048];
      snprintf(errbuffer, sizeof(errbuffer), "Error: size of name is invalid (%d != %d)\n", (int)view.len, (int)sizeof(name));
      printf("%s\n", errbuffer);
      PyErr_SetString(PyExc_IndexError, errbuffer);
      throw std::string(errbuffer);
      return NULL;
    }

    /* Allocate memory */
    name *result = new name(*static_cast<const name*>(view.buf));
    PyBuffer_Release(&view); // Note you could/should retain view.obj for the life of this object to prevent use after free
    return result;
  }
}
%enddef
%exception {
  try {
      $action
  } catch (std::string & msg) {
    SWIG_exception(SWIG_RuntimeError, msg.c_str());
  }
}

%typemap(memberin) float [ANY] {
    int i;
    for (i = 0; i < $1_dim0; i++) {
        $1[i] = $input[i];
    }
}

%typemap(out) float [ANY] {
  int i;
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PyFloat_FromDouble((double) $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%typemap(memberin) uint64_t [ANY] {
    int i;
    for (i = 0; i < $1_dim0; i++) {
        $1[i] = $input[i];
    }
}

%typemap(out) uint64_t [ANY] {
  int i;
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PyLong_FromLong( $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%typemap(memberin) uint32_t [ANY] {
    int i;
    for (i = 0; i < $1_dim0; i++) {
        $1[i] = $input[i];
    }
}

%typemap(out) uint32_t [ANY] {
  int i;
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PyLong_FromLong( $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%typemap(memberin) int64_t [ANY] {
    int i;
    for (i = 0; i < $1_dim0; i++) {
        $1[i] = $input[i];
    }
}

%typemap(out) int64_t [ANY] {
  int i;
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PyLong_FromLong( $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%typemap(memberin) int32_t [ANY] {
    int i;
    for (i = 0; i < $1_dim0; i++) {
        $1[i] = $input[i];
    }
}

%typemap(out) int32_t [ANY] {
  int i;
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PyLong_FromLong( $1[i]);
    PyList_SetItem($result,i,o);
  }
}
#endif
