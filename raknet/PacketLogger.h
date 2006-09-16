/// \file
/// \brief This will write all incoming and outgoing messages to the console window, or to a file if you override it and give it this functionality.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of Raknet is subject to the appropriate licence agreement.
/// "Shareware" Licensees with Rakkarsoft LLC are subject to the
/// shareware license found at
/// http://www.rakkarsoft.com/shareWareLicense.html
/// "Commercial" Licensees are subject to the commercial license found at
/// http://www.rakkarsoft.com/sourceCodeLicense.html
/// Custom license users are subject to the terms therein.
/// All other users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __PACKET_LOGGER_H
#define __PACKET_LOGGER_H

class RakPeerInterface;
#include "NetworkTypes.h"
#include "PluginInterface.h"

/// This will write all incoming and outgoing messages to the console window, or to a file if you override it and give it this functionality.
class PacketLogger : public PluginInterface
{
public:
	PacketLogger();
	~PacketLogger();

	virtual void OnAttach(RakPeerInterface *peer);

	/// Events on low level sends and receives.  These functions may be called from different threads at the same time.
	virtual void OnDirectSocketSend(const char *data, const unsigned bitsUsed, PlayerID remoteSystemID);
	virtual void OnDirectSocketReceive(const char *data, const unsigned bitsUsed, PlayerID remoteSystemID);
	virtual void OnInternalPacket(InternalPacket *internalPacket, unsigned frameNumber, PlayerID remoteSystemID, unsigned time, bool isSend);

	/// Logs out a header for all the data
	virtual void LogHeader(void);

	/// Override this to log strings to wherever.  Log should be threadsafe
	virtual void WriteLog(const char *str);
protected:
	RakPeerInterface *rakPeer;
};

#endif
