#ifndef __halfnetworkcommontype_h__
#define __halfnetworkcommontype_h__

#pragma once
#include <ace/ace_wchar.h>
namespace HalfNetwork
{

	typedef	unsigned char		uint8;
	typedef	signed char			int8;
	typedef	unsigned short		uint16;
	typedef	signed short		int16;
	typedef	unsigned int		uint32;
	typedef	signed int			int32;
	typedef	float				f32;
	typedef	double				f64;
	typedef double				real;

	const uint32	Invalid_ID	=	0xFFFFFFFF;

	#ifndef SAFE_DELETE 
	#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } } 
	#endif // SAFE_DELETE 

	#ifndef WIN32
	#define TRUE	1
	#define FALSE	0
	#endif

	typedef std::basic_string<ACE_TCHAR> tstring;

} // namespace HalfNetwork

#endif // __halfnetworkcommontype_h__