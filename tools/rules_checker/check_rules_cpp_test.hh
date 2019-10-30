namespace NS_1 {
class ST_CLASS_TEST1_BAD {
  void m_func_test1_good(void);
  uint32_t m_func_test2_good();
  uint32_t & m_func_test3_good(void);
  uint32_t m_func_test4_good(uint32_t const in_i_var_func1_test4_good);
  uint32_t m_func_test5_good(const uint32_t in_i_var_func1_test5_good, uint32_t in_i_var_func2_test5_good);
  uint32_t m_func_test6_good(uint32_t * in_i_var_func1_test6_bad, uint32_t & in_i_var_func2_test6_bad);
  uint32_t & m_func_test7_good();
  uint32_t * m_func_test8_good();
  const uint32_t m_func_test9_good();
  uint32_t const m_func_test10_good();
  uint32_t * const m_func_test11_good();
  uint32_t const * m_func_test12_good();
  const uint32_t * m_func_test13_good();
  uint32_t ** m_func_test14_good();
  uint32_t * const * m_func_test15_good();
  uint32_t m_func_test16_good(uint32_t const ** in_ppi_var_func1_test6_good);
  uint32_t m_func_test17_good(std::vector<ST_TRACK> const &  in_rvs_var_func1_test17_good);
};
}
