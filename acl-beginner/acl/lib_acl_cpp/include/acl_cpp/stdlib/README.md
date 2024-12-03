# FIX acl code (for C++23) : __VA_ARGS__를 처리할 때 ##__VA_ARGS__를 사용하여 가변 인자가 없는 경우에도 매크로가 제대로 작동하도록 수정
## lib_acl_cpp/include/acl_cpp/stdlib/log.hpp

```
#  if _MSC_VER >= 1500
#   define logger(fmt, ...)  \
        acl::log::msg4(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#   define logger_warn(fmt, ...)  \
        acl::log::warn4(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#   define logger_error(fmt, ...)  \
        acl::log::error4(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#   define logger_fatal(fmt, ...)  \
        acl::log::fatal4(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#   define logger_debug(section, level, fmt, ...)  \
        acl::log::msg6(section, level, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#  else
#   define logger       acl::log::msg1
#   define logger_warn  acl::log::warn1
	@@ -55,7 +55,7 @@
#  endif

# endif
#endif // ACL_LOGGER_MACRO_OFF

namespace acl {
```
