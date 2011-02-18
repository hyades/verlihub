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
#ifndef CPICHATROOM_H
#define CPICHATROOM_H

#include "src/tlistplugin.h"
#include "src/cvhplugin.h"
#include "crooms.h"
#include "cconsole.h"

using namespace nDirectConnect::nPlugin;
using namespace nDirectConnect;
using namespace nUtils;

typedef tpiListPlugin<cRooms,cRoomConsole> tpiChatroomBase;

/**
\brief a messanger plugin for verlihub

users may leave offline messages for registered users or reg users may leave offline messages for anyone

@author Daniel Muller
*/
class cpiChatroom : public tpiChatroomBase
{
public:
	cpiChatroom();
	virtual ~cpiChatroom();
	virtual void OnLoad(cServerDC *);
	

	virtual bool RegisterAll();
	virtual bool OnUserCommand(cConnDC *, string *);
	virtual bool OnUserLogin(cUser *);
	virtual bool OnUserLogout(cUser *);
	virtual bool OnOperatorCommand(cConnDC *, string *);
	cRoomCfg *mCfg;
};


#endif