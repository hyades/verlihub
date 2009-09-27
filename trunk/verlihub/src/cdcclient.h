/***************************************************************************
*   Original Author: Daniel Muller (dan at verliba dot cz) 2003-05        *
*                                                                         *
*   Copyright (C) 2006-2009 by Verlihub Project                           *
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
#ifndef NDIRECTCONNECTCLIENT_H
#define NDIRECTCONNECTCLIENT_H
#include <string>
#include "cserverdc.h"

using namespace std;

namespace nDirectConnect {
	class cConnDC;

	namespace nTables {

		/**
		  This class represents a redirect

		  @author Simoncelli Davide
		*/

		class cDCClient
		{
			public:
				cDCClient();
				
				virtual ~cDCClient();

				string mName;
				
				string mTagID;
				
				double mMinVersion;
				
				double mMaxVersion;
				
				bool mBan;
				
				/**
				  Enable or disable a client
				*/
				
				int mEnable;
				
				virtual void OnLoad();
				friend ostream &operator << (ostream &, cDCClient &);
		};
	};
};

#endif