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

#include "cplugs.h"
#include "cvhpluginmgr.h"
#include "cserverdc.h"
#include "stringutils.h"
#include "src/i18n.h"
#include <sys/stat.h>

#define PADDING 15

namespace nVerliHub {
	using namespace nPlugin;
	using namespace nUtils;
	using namespace nEnums;
	namespace nPlugMan {

cPlug::cPlug() :
	mLoadOnStartup(true),
	mReloadNext(false),
	mUnloadNext(false),
	mOwner(NULL)
{}


void cPlug::OnLoad()
{
	if(IsScript()) {
		if(!FindDestPlugin()) {
			mLastError = _("Plugin not found.");
			SaveMe();
			return;
		}
	}
	if(mLoadOnStartup) {
		mReloadNext = false;
		mUnloadNext = false;
		Plugin();
	}

	if(mReloadNext) {
		mReloadNext = false;
		mUnloadNext = false;
		Replug();

	}
	if(mUnloadNext) {
		mUnloadNext = false;
		Plugout();
		SaveMe();
	}
}

cPlug *cPlug::FindDestPlugin() const
{
	return mOwner->FindPlug(mDest);
}

cVHPlugin *cPlug::GetDestPlugin() const
{
	cPlug *OwnerPlugin = FindDestPlugin();
	if(OwnerPlugin)
		return OwnerPlugin->IsLoaded();
	else return NULL;
}

cVHPlugin *cPlug::IsLoaded() const
{
	if(IsScript()) {
		cVHPlugin *plugin = GetDestPlugin();
		if(plugin && plugin->IsScriptLoaded(mPath))
			return plugin;
		else
			return NULL;
	} else {
		cVHPluginMgr *pm = mOwner?mOwner->mPM:NULL;
		if(pm)
			return (cVHPlugin *)pm->GetPluginByLib(mPath);
		else
			return NULL;
	}
}

bool cPlug::Plugin()
{
	cVHPluginMgr *pm = mOwner?mOwner->mPM:NULL;
	if (pm && !IsLoaded() && CheckMakeTime()) {
		if (IsScript()) {
			ostringstream os;
			bool result = false;

			cVHPlugin *dest = GetDestPlugin();
			if (dest) {
				if (dest->SupportsMultipleScripts())
					result = dest->AddScript(mPath, os);
				else if (dest->SupportsScripts())
					result = dest->LoadScript(mPath, os);
				else {
					mLastError = _("Plugin does not support scripts.");
					SaveMe();
					return false;
				}

				if (result)
					os << _("Script loaded.");
				else
					os << _("Error loading script.");
				mLastError = os.str();
				SaveMe();
				return result;
			}
		} else {
			if (pm->LoadPlugin(mPath)) {
				mLoadTime = cTime().Sec();
				mLastError = _("Plugin loaded.");
				SaveMe();
				return true;
			} else {
				mLastError = pm->GetError();
				SaveMe();
				return false;
			}
		}
	}
	return false;
}

bool cPlug::CheckMakeTime()
{
	// script don't have to be checked
	if (IsScript())
		return true;

	mMakeTime = mOwner->GetFileTime(mPath);

	if(mMakeTime && mMakeTime < mOwner->mVHTime) {
		mLastError = _("Warning: The plugin should be recompiled because Verlihub has been recently updated.");
		SaveMe();
		return false;
	}
	return true;
}

bool cPlug::Plugout()
{
	cVHPluginMgr *pm = mOwner?mOwner->mPM:NULL;
	cVHPlugin *pi = IsLoaded();
	if (pm && pi) {
		if(IsScript())
			return pi->UnLoadScript(mPath);
		else
			return pm->UnloadPlugin(pi->Name());
	}
	return false;
}

bool cPlug::Replug()
{
	cVHPluginMgr *pm = mOwner?mOwner->mPM:NULL;
	cVHPlugin *pi=IsLoaded();
	if (pm && pi && CheckMakeTime()) {
		if (pm->ReloadPlugin(pi->Name())) {
			mLastError = _("Plugin reloaded.");
			SaveMe();
			return true;
		} else {
			mLastError = pm->GetError();
			SaveMe();
			return false;
		}
	}
	return false;
}

bool cPlug::IsScript() const
{
	return mDest.size() > 0;
}

void cPlug::SaveMe()
{
	mOwner->UpdateData(*this);
}

ostream& operator << (ostream &os, const cPlug &plug)
{
	os << " [*] " << setw(PADDING) << setiosflags(ios::left) << _("Name") << plug.mNick.c_str() << " [" << (plug.IsLoaded() ? toUpper(_("On")) : toUpper(_("Off"))) << "]";
	os << " [" << (plug.mLoadOnStartup ? toUpper(_("Auto")) : toUpper(_("Manual"))) << "]" << endl;
	os << " [*] " << setw(PADDING) << setiosflags(ios::left) << _("Path") << plug.mPath.c_str() << endl;

	if (!plug.mDesc.empty())
		os << " [*] " << setw(PADDING) << setiosflags(ios::left) << _("Description") << plug.mDesc.c_str() << endl;

	if (!plug.mLastError.empty())
		os << " [*] " << setw(PADDING) << setiosflags(ios::left) << _("Last error") << plug.mLastError.c_str() << endl;

	return os;
}

//--------------------------

cPlugs::cPlugs(cVHPlugin *pi) : tPlugListBase(pi, "pi_plug") , mPM (NULL), mVHTime(0)
{};

void cPlugs::AddFields()
{
	AddCol("nick","varchar(10)","",false,mModel.mNick);
	AddPrimaryKey("nick");
	AddCol("path","varchar(128)","",false,mModel.mPath);
	AddCol("dest","varchar(10)","",true,mModel.mDest);
	AddCol("detail","text","",true,mModel.mDesc);
	AddCol("autoload","tinyint(1)","1",true,mModel.mLoadOnStartup);
	AddCol(  "reload","tinyint(1)","0",true,mModel.mReloadNext);
	AddCol(  "unload","tinyint(1)","0",true,mModel.mUnloadNext);
	AddCol("error","text","",true,mModel.mLastError);
	AddCol("lastload","int(11)","",true,mModel.mLoadTime);
	mMySQLTable.mExtra = "PRIMARY KEY(nick)";
}

cPlug * cPlugs::FindPlug(const string &nick)
{
	iterator it;
	cPlug tmp;
	tmp.mNick = nick;
	return FindData(tmp);
}

void cPlugs::PluginAll(int method)
{
	iterator it;
	bool IsAuto = false;
	switch(method) {
		case ePLUG_AUTOLOAD:
		case ePLUG_AUTOUNLOAD:
		case ePLUG_AUTORELOAD:
			IsAuto = true;
			break;
		default:break;
	};

	for(it = begin(); it != end(); ++it) {
		switch(method) {
			case ePLUG_AUTOLOAD:
			case ePLUG_LOAD:
				if ((!IsAuto || (*it)->mLoadOnStartup) && (*it)->mPath.size() > 0)
					(*it)->Plugin();
				break;
			case ePLUG_AUTORELOAD:
			case ePLUG_RELOAD:
				(*it)->Replug();
				break;
			case ePLUG_AUTOUNLOAD:
			case ePLUG_UNLOAD:
				(*it)->Plugout();
				break;
			default:break;
		};
	}
}

bool cPlugs::CompareDataKey(const cPlug &D1, const cPlug &D2)
{
	return D1.mNick == D2.mNick;
}

void cPlugs::OnLoadData(cPlug &plug)
{
	plug.mOwner = this;
	plug.OnLoad();
}

time_t cPlugs::GetFileTime(const string &filename)
{
	struct stat fs;

	if (stat(filename.data(), &fs) == 0) //success
		return fs.st_ctime;

	return 0;
}

	}; // namespace nPlugMan
}; // namespace nVerliHub
