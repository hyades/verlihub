/***************************************************************************
                          cdcconsole.h  -  description
                             -------------------
    begin                : Wed Jul 2 2003
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

#ifndef CDCCONSOLE_H
#define CDCCONSOLE_H

#include <string>
#include "ctriggers.h"
#include "cobj.h"
#include "cban.h"
#include "ccmdr.h"
#include "cdccommand.h"
#include "cinfoserver.h"
#include "tlistconsole.h"
#include "cconntypes.h"
#include "ctriggers.h"

using namespace std;
using namespace nCmdr;

namespace nDirectConnect {

namespace nTables {
	class cTriggers;
	class cTriggerConsole;
}
using namespace nTables;
using namespace nInfo;

class cConnDC;
class cServerDC;

/**
  * \brief Command interpreter for operators and normal users
  * @author Daniel Muller
  * Contains oll function that are execued for !commands or +commands (except the triggers)
  */

class cDCConsole : public cDCConsoleBase
{
public:
	cDCConsole(cServerDC *, cMySQL &);
	virtual ~cDCConsole();
	/** act on op's command */
	virtual int OpCommand(const string &, cConnDC*);
	/** act on usr's command */
	virtual int UsrCommand(const string & , cConnDC * );
	/** get user's ip */
	int  CmdGetip(istringstream &, cConnDC *);
	/** get user's host */
	int CmdGethost(istringstream & , cConnDC * );
	/** get user's host and ip */
	int CmdGetinfo(istringstream &cmd_line , cConnDC *conn );
	/** unban ip or host */
	//int CmdUnban(sitringstream & cmd_line, cConnDC * conn, int unbantype = cBan::eBF_NICKIP);
	/** ban ip or host */
	//int CmdBan(istringstream & cmd_line, cConnDC * conn, bool, int bantype=cBan::eBF_NICKIP);
	/** quit program */
	int CmdQuit(istringstream &, cConnDC * conn,int code);
	/** show alll variables along with their values */
	int CmdGetconfig(istringstream & cmd_line, cConnDC * conn);
	/** send help message corresponding to connection */
	int CmdHelp(istringstream & cmd_line, cConnDC * conn);
	int CmdCCBroadcast(istringstream & cmd_line, cConnDC * conn,int cl_min, int cl_max);
	int CmdRegMyPasswd(istringstream & cmd_line, cConnDC * conn);
	int CmdMyInfo(istringstream & cmd_line, cConnDC * conn);
	int CmdMyIp(istringstream & cmd_line, cConnDC * conn);
	int CmdMe(istringstream & cmd_line, cConnDC * conn);
	int CmdReport(istringstream & cmd_line, cConnDC * conn);
	int CmdRegMe(istringstream & cmd_line, cConnDC * conn);
	int CmdKick (istringstream & cmd_line, cConnDC * conn);
	int CmdChat (istringstream & cmd_line, cConnDC * conn, bool swith);
	/** banlist */
	int CmdBanList(istringstream & cmd_line, cConnDC * conn, int bantype, bool filter=false);
	/** make a secret user/op */
	int CmdHideMe(istringstream & cmd_line, cConnDC * conn);
	/** progressively change user limit */
	int CmdUserLimit(istringstream & cmd_line, cConnDC * conn);
	int CmdUnHideKick(istringstream &cmd_line, cConnDC *conn);
	int CmdHideKick(istringstream &cmd_line, cConnDC *conn);
	int CmdClass(istringstream &cmd_line, cConnDC *conn);
	int CmdUnGag(istringstream &cmd_line, cConnDC *conn);
	int CmdGag(istringstream &cmd_line, cConnDC *conn);
	int CmdProtect(istringstream &cmd_line, cConnDC *conn);
	int CmdReload (istringstream &cmd_line, cConnDC *conn);
	int CmdCmds (istringstream &cmd_line, cConnDC *conn);
	 /** sets a hub topic **/
	 int CmdTopic(istringstream & cmd_line, cConnDC * conn); 

	static cPCRE mIPRangeRex;
	static bool GetIPRange(const string &range, unsigned long &from, unsigned long &to);

	typedef cDCCommand::sDCCmdFunc cfDCCmdBase;
	typedef cDCCommand cDCCmdBase;

	enum{ eCM_CMD, eCM_BAN, eCM_GAG, eCM_TRIGGER, eCM_SET, eCM_REG, eCM_INFO, eCM_RAW, eCM_WHO, eCM_KICK, eCM_PLUG, eCM_REPORT, eCM_BROADCAST, eCM_CONNTYPE, eCM_TRIGGERS, eCM_GETCONFIG};
	
	cServerDC *mServer;

	cTriggers *mTriggers;
private:
	cCmdr mCmdr;
	cCmdr mUserCmdr;
	struct cfBan : cfDCCmdBase { virtual bool operator()(); } mFunBan;
	cDCCmdBase mCmdBan;
	struct cfGag : cfDCCmdBase { virtual bool operator()(); } mFunGag;
	cDCCmdBase mCmdGag;
	struct cfTrigger : cfDCCmdBase { virtual bool operator()(); } mFunTrigger;
	cDCCmdBase mCmdTrigger;
	struct cfSetVar : cfDCCmdBase { virtual bool operator()(); } mFunSetVar;
	cDCCmdBase mCmdSetVar;
	struct cfRegUsr : cfDCCmdBase { virtual bool operator()(); } mFunRegUsr;
	cDCCmdBase mCmdRegUsr;
	struct cfRaw : cfDCCmdBase { virtual bool operator()(); } mFunRaw;
	cDCCmdBase mCmdRaw;
	struct cfCmd : cfDCCmdBase { virtual bool operator()(); } mFunCmd;
	cDCCmdBase mCmdCmd;
	struct cfWho : cfDCCmdBase { virtual bool operator()(); } mFunWho;
	cDCCmdBase mCmdWho;
	struct cfKick : cfDCCmdBase { virtual bool operator()(); } mFunKick;
	cDCCmdBase mCmdKick;
	struct cfInfo : cfDCCmdBase {
		virtual bool operator()();
		cInfoServer mInfoServer;
	} mFunInfo;
	cDCCmdBase mCmdInfo;
	cDCCmdBase mCmdPlug;
	struct cfPlug : cfDCCmdBase { virtual bool operator()(); } mFunPlug;
	struct cfReport : cfDCCmdBase { virtual bool operator()(); } mFunReport;
	cDCCmdBase mCmdReport;
	struct cfBc : cfDCCmdBase { virtual bool operator()(); } mFunBc;
	cDCCmdBase mCmdBc;
	struct cfGetConfig : cfDCCmdBase { virtual bool operator()(); } mFunGetConfig;
	cDCCmdBase mCmdGetConfig;
	
	
	// redirection consoles to other console
	struct cfRedirToConsole : cfDCCmdBase {
		virtual bool operator()();
		tConsoleBase<cDCConsole> *mConsole;
	} mFunRedirConnType, mFunRedirTrigger;
	cDCCmdBase mCmdRedirConnType;
	cDCCmdBase mCmdRedirTrigger;
	cConnTypeConsole mConnTypeConsole;
	cTriggerConsole *mTriggerConsole;
};

};

#endif