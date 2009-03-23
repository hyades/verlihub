/***************************************************************************
 *   Copyright (C) 2004 by Daniel Muller                                   *
 *   dan at verliba dot cz                                                 *
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
#ifndef NCONFIGLISTCONSOLE_H
#define NCONFIGLISTCONSOLE_H

#include "cdccommand.h"
#include "ccmdr.h"

using namespace nCmdr;

namespace nDirectConnect{ class cConnDC;}
using namespace nDirectConnect;

namespace nConfig
{

	using namespace ::nCmdr;
/**
a console that parses commands for Lists based on tMySQLMemoryList

@author Daniel Muller
*/
template <class DATA_TYPE, class LIST_TYPE, class OWNER_TYPE>
class tListConsole : public tConsoleBase<OWNER_TYPE>
{
protected:
	class cfBase;

public:
	tListConsole(void *owner):
		tConsoleBase<OWNER_TYPE>((OWNER_TYPE *)owner),
		//mOwner((OWNER_TYPE *)owner),
		mCmdr(this)
	{}

	virtual ~tListConsole(){};

	enum { eLC_ADD, eLC_DEL, eLC_MOD, eLC_LST, eLC_HELP, eLC_FREE};

	virtual void AddCommands()
	{
		mCmdAdd.Init(int(eLC_ADD), CmdId(eLC_ADD), GetParamsRegex(eLC_ADD), &mcfAdd);
		mCmdDel.Init(eLC_DEL, CmdId(eLC_DEL), GetParamsRegex(eLC_DEL), &mcfDel);
		mCmdMod.Init(eLC_MOD, CmdId(eLC_MOD), GetParamsRegex(eLC_MOD), &mcfMod);
		mCmdLst.Init(eLC_LST, CmdId(eLC_LST), "", &mcfLst);
		mCmdHelp.Init(eLC_HELP, CmdId(eLC_HELP), "", &mcfHelp);
		mCmdr.Add(&mCmdAdd);
		mCmdr.Add(&mCmdDel);
		mCmdr.Add(&mCmdMod);
		mCmdr.Add(&mCmdLst);
		mCmdr.Add(&mCmdHelp);
	}

	virtual int OpCommand(const string &str, cConnDC*conn)
	{
		return this->DoCommand(str,conn);
	}
	
	virtual int UsrCommand(const string &str , cConnDC *conn)
	{
		return this->DoCommand(str,conn);
	}
	
	virtual int DoCommand(const string &str, cConnDC * conn)
	{
		ostringstream os;
		cCommand *Cmd = mCmdr.FindCommand(str);
		if (Cmd != NULL && this->IsConnAllowed(conn, Cmd->GetID()))
		{
			mCmdr.ExecuteCommand(Cmd, os, conn);
			this->mOwner->mServer->DCPublicHS(os.str().c_str(),conn);
			return 1;
		}
		return 0;
	}


	virtual const char * GetParamsRegex(int) = 0;
	virtual LIST_TYPE *GetTheList() = 0;
	virtual const char *CmdSuffix() {return "";}
	virtual const char *CmdPrefix() {return "\\+";}
	virtual void ListHead(ostream *os){}
	virtual bool IsConnAllowed(cConnDC *conn,int cmd){return true;}
	virtual bool ReadDataFromCmd(cfBase *cmd, int CmdID, DATA_TYPE &data) = 0;

	virtual const char *CmdWord(int cmd)
	{
		switch(cmd)
		{
			case eLC_ADD: return "add"; break;
			case eLC_DEL: return "del"; break;
			case eLC_MOD: return "mod"; break;
			case eLC_LST: return "lst"; break;
			case eLC_HELP: return "h"; break;
			default: return "???";
		}
	}

	virtual const char *CmdSuffixWithSpace(int cmd)
	{
		static string id;
		id = CmdSuffix();
		if(!(cmd == eLC_LST || cmd == eLC_HELP)) id += " ";
		return id.c_str();
	}
	
	virtual const char *CmdId(int cmd)
	{
		static string id;
		id = CmdPrefix();
		id += CmdWord(cmd);
		id += CmdSuffixWithSpace(cmd);
		return id.c_str();
	}

	virtual void GetHelpForCommand(int cmd, ostream &os)
	{
		os << this->CmdId(cmd) << this->GetParamsRegex(cmd) << "\r\n";
	}
	
	virtual void GetHelp(ostream &os)
	{
		os << "No help available" << "\r\n";	
	}
	
	virtual OWNER_TYPE * GetPlugin() { return this->mOwner; }
protected:


	class cfBase : public ::cDCCommand::sDCCmdFunc
	{
		public:
		~cfBase(){};
		tListConsole<DATA_TYPE, LIST_TYPE, OWNER_TYPE> *GetConsole(){
			return (tListConsole<DATA_TYPE, LIST_TYPE, OWNER_TYPE> *)(mCommand->mCmdr->mOwner);
		}
		virtual LIST_TYPE *GetTheList() { if(this->GetConsole() != NULL) return this->GetConsole()->GetTheList(); else return NULL;}
 		virtual void GetSyntaxHelp(ostream &os, cCommand *cmd)
		{
			this->GetConsole()->GetHelpForCommand(cmd->GetID(),os);
		}

	};

	class cfAdd : public cfBase
	{
	public:
		virtual ~cfAdd(){};
		virtual bool operator()()
		{
			DATA_TYPE Data;
			if ( this->GetConsole() &&  this->GetConsole()->ReadDataFromCmd(this, eLC_ADD, Data)) {
				LIST_TYPE *list = this->GetTheList();
				if (list) {
					if (!list->FindData(Data)) {
						DATA_TYPE *AddedData = list->AddData(Data);
						if (AddedData) {
							list->OnLoadData(*AddedData);
							(*this->mOS) << "Successfully added: " << *AddedData << "\r\n";
							return true;
						} else {
							(*this->mOS) << "Error: Cannot add";
						}
					} else {
						(*this->mOS) << "Error: Already exists";
					}
				}
			} else {
				(*this->mOS) << "\r\n";
			}
			return false;
		}
	} mcfAdd;

	class cfDel : public cfBase
	{
	public:
		virtual ~cfDel(){};
		virtual bool operator()()
		{
			DATA_TYPE Data;
			if ( this->GetConsole() && this->GetConsole()->ReadDataFromCmd(this, eLC_DEL, Data))
			{
				if (this->GetTheList() && this->GetTheList()->FindData(Data))
				{
					this->GetTheList()->DelData(Data);
					(*this->mOS) << "Deleted successfuly";
					return true;
				}
			}
			(*this->mOS) << "Data not found ";
			return false;
		}
	} mcfDel;

	class cfMod : public cfBase
	{
	public:
		virtual ~cfMod(){};
		virtual bool operator()()
		{
			DATA_TYPE Data, * pOrig;
			tListConsole<DATA_TYPE, LIST_TYPE, OWNER_TYPE> *Console;
			Console =  this->GetConsole();
			if ( Console && Console->ReadDataFromCmd(this, eLC_MOD, Data))
			{
				if (this->GetTheList() && (pOrig = this->GetTheList()->FindData(Data)))
				{
					if( Console->ReadDataFromCmd(this, eLC_MOD, *pOrig)) 
					{
						this->GetTheList()->UpdateData(*pOrig);
						(*this->mOS) << "Successfully modified: " << *pOrig << "\r\n";
						return true;
					} else {
						(*this->mOS) << "Error in data";
						return false;
					}
				}
			}
			(*this->mOS) << "Data not found ";
			return false;
		}
	} mcfMod;

	class cfLst : public cfBase
	{
	public:
		virtual ~cfLst(){};
		virtual bool operator()()
		{
			DATA_TYPE *pData;
			this->GetConsole()->ListHead(this->mOS);
			for(int i = 0; i < this->GetTheList()->Size(); i++)
			{
				pData = (*this->GetTheList())[i];
				(*this->mOS) << (*pData) << "\r\n";
			}

			return true;
		}
	} mcfLst;
	
	class cfHelp : public cfBase
	{
		public:
			virtual ~cfHelp(){};
			virtual bool operator()()
			{
				this->GetConsole()->GetHelp(*this->mOS);
				return true;
			}
	} mcfHelp;

	cDCCommand mCmdAdd;
	cDCCommand mCmdDel;
	cDCCommand mCmdMod;
	cDCCommand mCmdLst;
	cDCCommand mCmdHelp;

	//OWNER_TYPE *mOwner;
	cCmdr mCmdr;
};

};

#endif