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
#ifndef CCONNDC_H
#define CCONNDC_H
#include "casyncconn.h"
#include "cmessagedc.h"
#include "creguserinfo.h"
#include "ctimeout.h"

namespace nVerliHub {
	using namespace nSocket;
	using namespace nUtils;
	using namespace nTables;
	namespace nTables {
		class cConnType;
	};
	namespace nProtocol {
		class cDCProto;
	}
	using nProtocol::cDCProto;

	class cUser;
	namespace nSocket {
		class cServerDC;
	};

	namespace nEnums {

		/**
		 * Login flags.
		 * These flags are used to track the status of the login procedure
		 * of a single user in the hub.
		 */
		typedef enum
		{
			/// The key sends by the client is valid.
			eLS_KEYOK   = 1 << 0,
			/// The client sent $ValidateNick protocol message.
			eLS_VALNICK = 1 << 1,
			/// The user does not need to send the password
			/// or the password sends is valid. The hub just requests
			/// for a password only if the user is registered.
			eLS_PASSWD  = 1 << 2,
			/// The version of the client is valid.
			eLS_VERSION = 1 << 3,
			/// The client sent $MyINFO protocol message
			/// and it parsed correctly.
			eLS_MYINFO  = 1 << 4,
			/// The user is allowed to enter the hub
			eLS_ALLOWED  = 1 << 5,
			/// The client received by the hub $NickList protocol message
			/// with the list of all users in the hub.
			eLS_NICKLST = 1 << 6,
			/// All the flags explained above. This means the login
			/// procedure is complete.
			eLS_LOGIN_DONE = eLS_KEYOK | eLS_VALNICK | eLS_PASSWD | eLS_VERSION | eLS_MYINFO | eLS_ALLOWED | eLS_NICKLST
		} tLogStatus;

		/**
		 * Timeout flags for login procedure.
		 */
		typedef enum
		{
			/// The hub is waiting for $Key protocol message.
			/// This timeout is set after $Lock is received.
			eTO_KEY=0,
			/// The hub is waiting for $ValidateNick protocol message.
			/// This timeout is set after $Key is received.
			eTO_VALNICK,
			/// Login timeout of login procedure.
			/// This timeout is set when a cConnDC instance is created.
			eTO_LOGIN,
			/// The hub is waiting for $MyINFO protocol message.
			/// This timeout is set after $ValidateNick is received.
			eTO_MYINFO,
			/// The hub is waiting for the user to set a valid password.
			/// This timeout is set when the user should set a password
			/// in cServerDC::AfterUserLogin() call.
			eTO_SETPASS,
			/// This is NOT a timeout flag but it is used to count the
			/// number of flags.
			eTO_MAXTO
		} tTimeOut;

		/**
		 * Support flags for client and hub features sent in $Support
		 * protocol message.
		 */
		typedef enum
		{
			/// OpPlus feature.
			eSF_OPPLUS = 1,
			/// NoHello feature.
			eSF_NOHELLO = 1 << 1,
			/// NoGetINFO feature.
			eSF_NOGETINFO = 1 << 2,
			/// Passive user feature.
			eSF_PASSIVE = 1 << 3,
			/// Quicklist feature.
			eSF_QUICKLIST = 1 << 4,
			/// BOTinfo feature.
			eSF_BOTINFO = 1 << 5,
			/// ZLib feature.
			eSF_ZLIB = 1 << 6
		} tSupportFeature;
	};

	using namespace nEnums;

	class cDCBanRecord;
	namespace nSocket {

		/// @addtogroup Core
		/// @{
		/**
		 * Connection factory to create and delete a connection of type cConnDC
		 * between DC users.
		 * This class is used by cAsyncConn and cServerDC to create and delete
		 * user connection.
		 * @author Daniel Muller
		 */
		class cDCConnFactory : public cConnFactory
		{
		public:
				/**
				 * Class constructor.
				 * @param server A pointer to cServerDC instance.
				 */
				cDCConnFactory(cServerDC *server);

				/**
				 * Class destructor.
				 */
				virtual ~cDCConnFactory()
				{}

				/**
				 * Create a new connection for the given socket.
				 * This method will also assign a protocol handler to
				 * the cAsyncConn instance, assign a country zone if GeoIP
				 * is installed and increment the number of the user in the hub.
				 * @param sd Socket identifier of the connection.
				 * @return A new connection object that is an instance of cAsyncConn class.
				 */
				virtual cAsyncConn * CreateConn(tSocket sd=0);

				/**
				 * Delete a connection.
				 * This method will also decrement the number of
				 * the user in the hub.
				 * @param connection A pointer to the connection to delete.
				 */
				virtual void DeleteConn(cAsyncConn * &connection);
			protected:
				/// Pointer to cServerDC instance.
				cServerDC *mServer;
		};
		/**
		 * Network connection class for asynchronous or non-blocking connection
		 * between two enteties that talks a DC protocol.
		 * @author Daniel Muller
		 */

		class cConnDC : public cAsyncConn
		{
			friend class nProtocol::cDCProto;
			public:
				/**
				 * Class constructor.
				 * @param sd Socket identifier of the connection.
				 * @param server Pointer to cAsyncSocketServer instance.
				 */
				cConnDC(int sd = 0, cAsyncSocketServer *server=NULL);

				/**
				 * Class destructor.
				 */
				virtual ~cConnDC();

				/**
				 * Check if the given timeout is expired..
				 * @param timeout The timeout.
				 * @param now Current time.
				 * @return 1 if timeout is expired or 0 otherwise.
				 */
				int CheckTimeOut(tTimeOut timeout, cTime &now);

				/**
				 * Reset and clear the given timeout.
				 * @param timeout Time timeout.
				 * @return 1 if timeout is cleared or 0 otherwise.
				 */
				int ClearTimeOut(tTimeOut timeout);

				/**
				 * Close the connection with the given reason after msec milliseconds are elapsed.
				 * @param msed Time in milliseconds before closing the connection.
				 * @param reason The reason.
				 */
				virtual void CloseNice(int msec, int reason = 0);

				/**
				 * Close the connection with the given reason.
				 * @param reason The reason.
				 */
				virtual void CloseNow(int reason = 0);

				/**
				 * Return if the given login status flag is set or not.
				 * @param statusFlag Status flag.
				 * @return Zero if flag is not set or the value of the given status flag if set.
				 */
				unsigned int GetLSFlag(unsigned int statusFlag);

				/**
				 * Return a string describing the timeout.
				 * @param timeout tTimeOut Timeout type.
				 * @return A translated string describing the timeout.
				 */
				const char *GetTimeOutType(tTimeOut timeout);

				/**
				 * Try to guess the class of the user.
				 * Remember a valid class is returned only when the
				 * login procedure of the user is valid.
				 * @return The user class.
				 */
				int GetTheoricalClass()
				{
					if (!mRegInfo)
						return 0;
					if(!mRegInfo->mEnabled)
						return 0;
					//if (mRegInfo->mClass < 0) return 1; /* wtf ?*/
					return mRegInfo->mClass;
				}

				/**
				 * Check if the user needs a password.
				 * @return True if the user must provide a password, false otherwise.
				 */
				bool NeedsPassword();

				/**
				 * Event handler function called when write buffer gets empty.
				 */
				void OnFlushDone();

				/**
				 * This method is called every period of time.
				 * @param now Current time.
				 * @return Always zero.
				 */
				virtual int OnTimer(cTime &now);

				/**
				 * Reset login status flag and set a new value.
				 * @param statusFlag Status flag.
				 */
				void ReSetLSFlag(unsigned int statusFlag);

				/**
				 * Set login status flag.
				 * The other set flags are not resetted.
				 * @param statusFlag Status flag.
				 */
				void SetLSFlag(unsigned int statusFlag);

				/**
				 * Set the cUser instance linked with this connection.
				 * @param usr Pointer to cUser instance.
				 * @return True if successful, otherwise false.
				 */
				bool SetUser(cUser *usr);

				/**
				* Send raw data to the user.
				* @param data Raw data to send.
				* @param addPipe Set it to true if a pipe must be added to the end of data.
				* @param flush Set it to true if raw data should be send immediatly or stored in the internal buffer.
				* @return The number of sent bytes.
				*/
				int Send(string & data, bool addPipe=true, bool flush = true);

				/**
				* Return a pointer to cServerDC instance.
				* @return The pointer to cServerDC instance.
				*/
				inline cServerDC * Server() const
				{
					return (cServerDC*) mxServer;
				}

				/**
				* Set the given timeout.
				* @param timeout The timeout.
				* @param seconds Timeout in seconds.
				* @param now Current time.
				* @return 1 if timeout is set or 0 otherwise.
				*/
				int SetTimeOut(tTimeOut timeout, double seconds, cTime &now);

				/**
				* Log an event.
				* @param ostr The message of the event.
				* @param level The log level of the event.
				* @return 1 if message is logged or 0 otherwise.
				*/
				virtual int StrLog(ostream &ostr, int level);

				/// Pointer to cUser instance.
				cUser * mpUser;

				/// Protocol extenstion supported by the client.
				/// @see tSupportFeature
				/// @todo Move to cProtocol class.
				unsigned mFeatures;

				/// Protocol version
				/// @todo Move to cProtocol class.
				string mVersion;

				/// Pointer to cRegUserInfo instance
				/// to manage user registration.
				cRegUserInfo *mRegInfo;

				/// True if nicklist is sent on user login.
				bool mSendNickList;

				/// True if the hub is sending nicklist to the user.
				bool mNickListInProgress;

				/// True if nicklist should be skipped.
				bool mSkipNickList;

				/// Pointer to cConnType instance
				/// that represents the type of connection.
				cConnType *mConnType;

				/// The country code of the connection.
				string mCC;

				/// Geographic zone according to country code.
				int mGeoZone;

				/// Reason of why connection was closed.
				int mCloseReason;
			private:
				/// The attempt of last login.
				cTime mTimeLastAttempt;

				/// Login status flags.
				/// @see tLogStatus
				unsigned int mLogStatus;

		protected:
			/**
			 * Event handler function called before the connection is closed.
			 * This method will also send the redirect protocol message ($ForceMove) to the user.
			 * @return Always zero.
			 */
			int OnCloseNice();

			/// List of timeout.
			cTimeOut mTO[eTO_MAXTO];

			/// Structure that is used to ping the user
			///and check if he is stil alive
			struct sTimes
			{
				/// Time when the $Key is received.
				/// @todo Is still used?
				cTime key;
				/// Time of last ping.
				cTime ping;
				/**
				 * Constructor.
				 */
				sTimes():key(0l),ping(0l){};
			};

			/// Ping handler.
			sTimes mT;

			/// Search result counter.
			/// @todo Is still used? If yes we have to move it somewhere
			/// outside this class.
			int mSRCounter;
		};
		/// @}
	}; // namespace nSocket
}; // namespace nVerliHub

#endif
