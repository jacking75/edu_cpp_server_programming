// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


//#include <iostream>
//#include <tchar.h>

// TODO: 在此处引用程序要求的附加头文件

#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"
#include "fiber/libfiber.hpp"

#ifdef	ACL_USE_CPP11
#include "fiber/go_fiber.hpp"
#endif

#ifdef	WIN32
#define	snprintf _snprintf
#endif

