/**************************************************************************
*   Original Author: Daniel Muller (dan at verliba dot cz) 2003-05        *
*                                                                         *
*   Copyright (C) 2006-2011 by Verlihub Project                           *
*   devs at verlihub-project dot org                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "cserverdc.h"
#include "creglist.h"
#include "cuser.h"
#include "creguserinfo.h"
#include "stringutils.h"
#include "i18n.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <unistd.h>

namespace nVerliHub {
	using namespace nConfig;
	using namespace nSocket;
	using namespace nMySQL;
	using namespace nUtils;
	namespace nTables {


cRegList::cRegList(cMySQL &mysql, cServerDC *server): cConfMySQL(mysql)
	,mCache(mysql,"reglist", "nick", "reg_date"), mS (server)
{
	//cTime now;
	SetClassName("nDC::cRegList");
	mMySQLTable.mName="reglist";
	ostringstream nickDomain;
	nickDomain << "varchar(";
	nickDomain << mS->mC.max_nick;
	nickDomain << ")";
	AddCol("nick", nickDomain.str().c_str(), "", false, mModel.mNick);
	AddPrimaryKey("nick");
	AddCol("class", "int(2)", "1", true, mModel.mClass);
	AddCol("class_protect", "int(2)", "0", true, mModel.mClassProtect);
	AddCol("class_hidekick", "int(2)", "0", true, mModel.mClassHideKick);
	AddCol("hide_kick", "tinyint(1)", "0", true, mModel.mHideKick);
	AddCol("hide_keys", "tinyint(1)", "0", true, mModel.mHideKeys);
	AddCol("hide_share", "tinyint(1)", "0", true, mModel.mHideShare);
	AddCol("reg_date", "int(11)", "", true, mModel.mRegDate);
	AddCol("reg_op", "varchar(64)", "", true, mModel.mRegOp);
	AddCol("pwd_change", "tinyint(1)", "1", true, mModel.mPwdChange);
	AddCol("pwd_crypt", "tinyint(1)", "1", true, mModel.mPWCrypt);
	AddCol("login_pwd", "varchar(60)", "", true, mModel.mPasswd);
	AddCol("login_last", "int(11)", "0", true, mModel.mLoginLast);
	AddCol("logout_last", "int(11)", "0", true, mModel.mLogoutLast);
	AddCol("login_cnt", "int(11)", "0", true, mModel.mLoginCount);
	AddCol("login_ip", "varchar(16)", "", true, mModel.mLoginIP);
	AddCol("error_last", "int(11)", "", true, mModel.mErrorLast);
	AddCol("error_cnt", "int(11)", "0", true, mModel.mErrorCount);
	AddCol("error_ip", "varchar(16)", "", true, mModel.mErrorIP);
	AddCol("enabled", "tinyint(1)", "1", true, mModel.mEnabled);
	AddCol("email", "varchar(60)", "", true, mModel.mEmail);
	AddCol("note_op", "text", "", true, mModel.mNoteOp);
	AddCol("note_usr", "text", "", true, mModel.mNoteUsr);
	AddCol("auth_ip", "varchar(15)", "", true, mModel.mAuthIP);
	AddCol("alternate_ip", "varchar(16)", "", true, mModel.mAlternateIP);
	mMySQLTable.mExtra = "PRIMARY KEY(nick), ";
	mMySQLTable.mExtra+= "INDEX login_index (login_last), ";
	mMySQLTable.mExtra+= "INDEX logout_index (logout_last)";
	SetBaseTo(&mModel);
}

cRegList::~cRegList(){
}

/** find nick in reglist
if not foud return 0
else return 1 and fill in the reuserinfo parameter */
bool cRegList::FindRegInfo(cRegUserInfo &ui, const string &nick)
{
	if(mCache.IsLoaded() && !mCache.Find(nick))
		return false;//@todo nick2dbkey
	SetBaseTo(&ui);
	ui.mNick = nick;//@todo nick2dbkey
	return LoadPK();
}

int cRegList::ShowUsers(cConnDC *op, ostringstream &os, int cls)
{
	 if (op && op->mpUser) {
		ostringstream oss;
		oss << "SELECT `nick` FROM " << mMySQLTable.mName << " WHERE `class` = " << cls << " ORDER BY `nick` ASC";
		mQuery.OStream() << oss.str();
		if (mQuery.Query() <= 0) return 0;
		int n = mQuery.StoreResult();
		cMySQLColumn col;
		MYSQL_ROW row;
		cUser *usr = NULL;

		for (int i = 0; i < n; i++) {
			row = mQuery.Row();
			usr = mS->mUserList.GetUserByNick(row[0]);
			os << " " << row[0];
			if (usr && usr->mxConn) os << " [" << toUpper(_("On")) << "]";
			os << "\r\n";
		}

		mQuery.Clear();
		return n;
	}

	return 0;
}

/** add registered user */
bool cRegList::AddRegUser(const string &nick, cConnDC *op, int cl, const char *password)
{
	cRegUserInfo ui;

	if(FindRegInfo(ui, nick))
		return false;
	// Do not register opchat or hub security
	if(toLower(nick) == toLower(mS->mC.opchat_name) || toLower(nick) == toLower(mS->mC.hub_security))
		return false;

	ui.mNick = nick;//@todo nick2dbkey
	if ((cl>=1 && cl<=5) || cl==10 || cl==-1)
		ui.mClass = cl;
	else
		ui.mClass = 1;
	ui.mRegDate = cTime().Sec();
	ui.mRegOp = (op && op->mpUser) ? op->mpUser->mNick : string("hub-security");

	if(password)
		ui.SetPass(string(password),mS->mC.default_password_encryption);
	else
		ui.SetPass(string(),mS->mC.default_password_encryption);

	if(cl < 0)
		ui.mPwdChange = false;
	if(mCache.IsLoaded())
		mCache.Add(nick);//@todo nick2dbkey

	SetBaseTo(&ui);
	return SavePK();
}


/** No descriptions */
bool cRegList::ChangePwd(const string &nick, const string &pwd, int crypt)
{
	if(!FindRegInfo(mModel, nick))
		return false;
	mModel.SetPass(pwd,mS->mC.default_password_encryption);
	return UpdatePK();
}

/** No descriptions */
bool cRegList::SetVar(const string &nick, string &field, string &value)
{
	SetBaseTo(&mModel);
	mModel.mNick = nick; //@todo nick2dbkey
	return UpdatePKVar(field.c_str(), value);
}

/** log that user logged in */
bool cRegList::Login(cConnDC *conn, const string &nick)
{
	cRegUserInfo ui;
	if(!FindRegInfo(ui, nick)) return false;
	ui.mLoginLast = cTime().Sec();
	ui.mLoginIP   = conn->AddrIP();
	ui.mLoginCount ++;
	return UpdatePK();
}

/** log that user logged in */
bool cRegList::Logout(const string &nick)
{
	if(!FindRegInfo(mModel, nick)) return false;
	mModel.mLogoutLast = cTime().Sec()-1; // this is a patch for users that connect twice
	return UpdatePKVar("logout_last");
}

/** log that user logged in with error*/
bool cRegList::LoginError(cConnDC *conn, const string &nick)
{
	if(!FindRegInfo(mModel, nick)) return false;
	mModel.mErrorLast = cTime().Sec();
	mModel.mErrorIP = conn->AddrIP();
	return UpdatePK();
}


/*!
    \fn ::nTables::cRegList::DelReg(const string &nick)
 */
bool cRegList::DelReg(const string &nick)
{
	if(!FindRegInfo(mModel, nick)) return false;
	DeletePK();
	return true;
}

/**

How registering works:
----------------------

1/ user <Nick> asks to be registered
2/ if operator is ok with it, he uses command !regnewuser <Nick> [<class>=1] to prepare an account for the user..
	* this checks if such user is registered (if so error occurs)
	* creates default reguser data into the reglist, which is marked as pwd_change
	* then it sends pm to the user asking him por passwrd and some other settings
3/ user provides password with +passwd <new_pass> [<crypt>=0], which is enabled only if pwd_change is marked
	* if user doesn't provide any password he won't be registered
	* if password is provided and is ok, user finishes his registration
4/ to change password user asks any op again, op uses command !regpasswd <Nick> to enable password changing
5/ to change user's class op uses commands !regclass <Nick> <new_class> ; new_class must be less then op's class
6/ to ungegiser use op command !regdeluser <Nick> - this disables the registration by setting a flag
7/ to purge a disabled record, admin can do !regpurge <Nick>

Note: registered users aren't banned by ip only, but by their nick too

1. method
---------
crypted=crypt("heslo"); // mysql: encrypt; php, perl, C : crypt
if(crypt("heslo2",crypted) == crypted) OK

md5
crypt
*/

	}; // namespace nTables
}; // namespace nVerliHub


