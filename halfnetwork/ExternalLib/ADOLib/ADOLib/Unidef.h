#pragma once

namespace std
{
	#ifdef	_UNICODE
		typedef wstring tstring;
		static wostream& tcout = wcout;
		static wistream& tcin  = wcin;
	#else
		typedef string tstring;
		static ostream& tcout =  cout;
		static istream& tcin  =  cin;
	#endif
}
