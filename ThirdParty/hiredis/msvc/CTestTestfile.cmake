# CMake generated Testfile for 
# Source directory: C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis
# Build directory: C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/msvc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(hiredis-test "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/test.sh")
  set_tests_properties(hiredis-test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;158;ADD_TEST;C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(hiredis-test "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/test.sh")
  set_tests_properties(hiredis-test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;158;ADD_TEST;C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(hiredis-test "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/test.sh")
  set_tests_properties(hiredis-test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;158;ADD_TEST;C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(hiredis-test "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/test.sh")
  set_tests_properties(hiredis-test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;158;ADD_TEST;C:/Users/eehluhy/Desktop/c++/edu_com2us_cpp_server_Training/ThirdParty/hiredis/CMakeLists.txt;0;")
else()
  add_test(hiredis-test NOT_AVAILABLE)
endif()