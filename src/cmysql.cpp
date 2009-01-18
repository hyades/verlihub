/***************************************************************************
 *   Copyright (C) 2003 by Dan Muller                                      *
 *   dan at verliba dot cz                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cmysql.h"

namespace nMySQL {

cMySQL::cMySQL() : cObj("cMySQL")
{
	Init();
}

/*!
    \fn nMySQL::cMySQL::cMySQL(string&host,string&user,string&pass,string&data)
 */
cMySQL::cMySQL(string&host,string&user,string&pass,string&data) : cObj("cMySQL")
{
    Init();
    if(!Connect(host,user,pass,data))
    {
    	throw "Mysql connection error.";
    }
}

cMySQL::~cMySQL()
{
	mysql_close(mDBHandle);
}

void cMySQL::Init()
{
	mDBHandle = NULL;
	mDBHandle = mysql_init(mDBHandle);
	if(!mDBHandle) Error(0, string("Can't init mysql structure :(.: "));	
}

bool cMySQL::Connect(string &host, string &user, string &pass, string &data)
{
	if(Log(1)) LogStream() << "Connecting to mysql server: "
			<< user << "@" << host << "/" << data << endl;
	
	mysql_options(mDBHandle,MYSQL_OPT_COMPRESS,0);
	
	if(!mysql_real_connect(
		mDBHandle,
		host.c_str(),
		user.c_str(),
		pass.c_str(),
		data.c_str(),0,0,0
		)
	){

		Error(1, string("Connection to mysql server failed: "));
		return false;
	}
	mysql_query(mDBHandle, "SET NAMES 'utf8'");
	//SET CHARACTER SET utf8
	return true;
}

/*!
    \fn cMySQL::Error()
 */
void cMySQL::Error(int level, string text)
{
	if(ErrLog(level)) LogStream() << text << mysql_error(mDBHandle) << endl;
}

};
