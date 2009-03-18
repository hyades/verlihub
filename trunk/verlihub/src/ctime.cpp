/***************************************************************************
                          ctime.cpp  -  description
                             -------------------
    begin                : Sun Aug 31 2003
    copyright            : (C) 2003 by Daniel Muller
    email                : dan at verliba dot cz
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ctime.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#include <sstream>

#if defined _WIN32
#include <windows.h>

/* /////////////////////////////////////////////////////////////////////////////
 * API functions
 */

void gettimeofday(struct timeval *tv, struct timezone *tz)
{
	SYSTEMTIME              st;
	union
	{
		FILETIME            ft;
		unsigned __int64    ui64;
	} u;

	((void)tz);

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &u.ft);

	u.ui64 -= (unsigned __int64)(24*3600)*((1970-1601)*365 + 92)*10000000ul;
	tv->tv_sec  =   (long)(u.ui64 / 10000000);
	tv->tv_usec =   (long)(u.ui64 % 10000000);
}

#endif

using namespace std;

namespace nUtils {

cTime::~cTime(){
}

string cTime::AsString() const{
	ostringstream os;
	os << (*this);
	return os.str();
}

std::ostream & operator<< (std::ostream &os, const cTime &t)
{
	#if !defined _WIN32
	static char buf[26];
	#else
	char * buf;
	#endif

	long n, rest, i;

	switch (t.mPrintType)
	{
	case 1:
		#if !defined _WIN32
		ctime_r((time_t*)&t.tv_sec,buf);
		#else
		buf = ctime( (const time_t*)&(t.tv_sec) );
		#endif
		buf[strlen(buf)-1]=0;
		os << buf;
		break;
	case 2:
		rest = t.tv_sec;
		i = 0;

		n = rest / (24*3600*7);
		rest %= (24*3600*7);
		if(n && ++i <= 2) os << n << "weeks ";

		n = rest / (24*3600);
		rest %= (24*3600);
		if(n && ++i <= 2) os << n << "days ";

		n = rest / (3600);
		rest %= (3600);
		if(n && ++i <= 2) os << n << "hours ";

		n = rest / (60);
		rest %= (60);
		if(n && ++i <= 2) os << n << "min ";

		n = rest;
		rest = 0;
		if(++i <= 2) os << n << "sec ";

		if(++i <= 2) os << t.tv_usec/1000 << "ms ";
		if(++i <= 2) os << t.tv_usec%1000 << "�s ";
		break;
	default :
		os << t.tv_sec << "s " << t.tv_usec << "�s";
		break;
	}
	return os;
};

};
