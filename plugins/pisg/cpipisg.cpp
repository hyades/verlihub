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

#include "cpipisg.h"
#include "src/cserverdc.h"
#include <time.h>

using namespace nVerliHub;

cpiPisg::cpiPisg() : 
	mStats(NULL),
	mStatsTimer(300.0,0.0,cTime().Sec()),
	mFreqSearchA(cTime(), 300.0, 10),
	mFreqSearchP(cTime(), 300.0, 10)
{
	mName = "Pisg";
	mVersion = PISG_VERSION;
}

cpiPisg::~cpiPisg()
{
	logFile.close();
}


void cpiPisg::OnLoad(cServerDC *server)
{
	cVHPlugin::OnLoad(server);
	mServer = server;
	logFile.open(server->mConfigBaseDir + "/" + "pisg.log");
}

bool cpiPisg::RegisterAll()
{
	RegisterCallBack("VH_OnParsedMsgChat");
	return true;
}

bool cpiPisg::OnParsedMsgChat(cConnDC *conn, cMessageDC *msg)
{
	if(logFile.is_open && conn != NULL && conn->mpUser != NULL && msg != NULL) {
		 time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime (&rawtime);
		logFile << "[[" << (timeinfo->tm_year + 1900) << "%s-%s-%s|%s:%s]] <" << conn->mpUser->mNick.c_str() << "> " << msg->ChunkString(eCH_CH_MSG).c_str() << endl;
	//date =  os.date ("*t") io.write("[["..date.year.."-"..formatTF(date.month).."-"..formatTF(date.day).."|"..formatTF(date.hour)..":"..formatTF(date.min).."]] <"..nick.."> "..data..lf)
	}
	return true;
}

REGISTER_PLUGIN(cpiPisg);
