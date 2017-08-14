#pragma once

#ifndef __TSTRING
#define __TSTRING

#include <tchar.h>
#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

typedef unsigned char  byte;

typedef std::basic_string<TCHAR>		 tstring;

typedef std::basic_istream<TCHAR>        tistream;
typedef std::basic_ostream<TCHAR>        tostream;
typedef std::basic_iostream<TCHAR>       tiostream;

typedef std::basic_ifstream<TCHAR>       tifstream;
typedef std::basic_ofstream<TCHAR>       tofstream;
typedef std::basic_fstream<TCHAR>        tfstream;

typedef std::basic_istringstream<TCHAR>  tistringstream;
typedef std::basic_ostringstream<TCHAR>  tostringstream;
typedef std::basic_stringstream<TCHAR>   tstringstream;

#define tstrlen   _tcslen
#define tstrcmp   _tcscmp
#define tstrftime _tcsftime 

#if defined (UNICODE) || defined(_UNICODE)
	#define to_tstring std::to_wstring   
	#define tcin  std::wcin
	#define tcout std::wcout
	#define tcerr std::wcerr
	#define tclog std::wclog
#else
	#define to_tstring std::to_string	  
	#define tcin  std::cin	  
	#define tcout std::cout
	#define tcerr std::cerr
	#define tclog std::clog 
#endif	
					
//		---- char*  To wchar* ----
const inline wchar_t *GetWC(const char *c) {
	const size_t inSize = strlen(c) + 1;
	size_t outSize;
	wchar_t* wc = new wchar_t[inSize];
#ifdef _WIN32 || _WIN64
	mbstowcs_s(&outSize, wc, inSize, c, inSize - 1);
#else
	outSize = mbstowcs(wc, c, inSize - 1);
#endif // _WIN32 || _WIN64
	return wc;
}
//		---- wchar* To wchar*  (для совместимости) ----
const inline wchar_t *GetWC(const wchar_t *wc) { return wc; }
//		---- wchar* To char* ----
const inline char *GetC(const wchar_t *wc) {
	const size_t inSize = wcslen(wc) - 1;
	size_t outSize;
	char* c = new char[inSize];
#ifdef _WIN32 || _WIN64
	wcstombs_s(&outSize, c, inSize, wc, inSize - 1);
#else
	outSize = wcstombs(c, wc, inSize - 1);
#endif
	return c;
}
//		---- char*  To char*   (для совсместимости) ----
const inline char *GetC(const char *c) { return c; }
//		---- char*  To TCHAR* ----
const inline TCHAR* GetString(const char * c) { 
	return 
#ifdef UNICODE || _UNICODE
	GetWC
#endif
	(c);
}
//		---- wchar* To TCHAR* ----
const inline TCHAR* GetString(const wchar_t * wc){
	return 
#ifndef UNICODE || _UNICODE
	GetC
#endif // UNICODE || _UNICODE
	(wc);
}

//littleEndian To BegEndian and BegEndian To LittleEndian int16_t
const inline int16_t reversInt16(int16_t val) {
	TCHAR tmp[2];
	tstringstream ss;
	ss << val;
	ss >> tmp;
	return static_cast<int32_t>((tmp[0]) | (tmp[1] << 8));
}
//littleEndian To BegEndian and BegEndian To LittleEndian int32_t
const inline int32_t reversInt32(int32_t val) {
	TCHAR tmp[4];
	tstringstream ss;
	ss << val;
	ss >> tmp;
	return static_cast<int32_t>((tmp[0]) | (tmp[1] << 8) || (tmp[2] << 16) || (tmp[3] << 24));
}

#endif //__TSTRING
