/*
 *  Squeeze2upnp - LMS to uPNP gateway
 *
 *  Squeezelite : (c) Adrian Smith 2012-2014, triode1@btinternet.com
 *  Additions & gateway : (c) Philippe 2014, philippe_44@outlook.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SQUEEZE2UPNP_H
#define __SQUEEZE2UPNP_H

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

#include "upnp.h"
#include "ithread.h"
#include "squeezedefs.h"
#include "squeezeitf.h"
#include "util.h"

/*----------------------------------------------------------------------------*/
/* typedefs */
/*----------------------------------------------------------------------------*/

#define MAX_PROTO		128
#define MAX_RENDERERS	32
#define MAGIC			0xAABBCCDD
#define RESOURCE_LENGTH	250
#define	SCAN_TIMEOUT 	15
#define SCAN_INTERVAL	30

#define	HTTP_DEFAULT_MODE	-3


enum 	eMRstate { UNKNOWN, STOPPED, PLAYING, PAUSED, TRANSITIONING };
enum 	{ AVT_SRV_IDX = 0, REND_SRV_IDX, CNX_MGR_IDX, NB_SRV };

struct sService {
	char Id			[RESOURCE_LENGTH];
	char Type		[RESOURCE_LENGTH];
	char EventURL	[RESOURCE_LENGTH];
	char ControlURL	[RESOURCE_LENGTH];
	Upnp_SID		SID;
	s32_t			TimeOut;
};

typedef struct sMRConfig
{
	char		StreamLength[SQ_STR_LENGTH];
	sq_mode_t	ProcessMode;   		// DIRECT, STREAM, FULL
	bool		NoZeroVolume;		// prevent volume to be set at 0 between tracks
	bool		SeekAfterPause;
	bool		ByteSeek;
	u16_t		VolumeCorrector;	// not yet
	bool		Enabled;
	char		Name[SQ_STR_LENGTH];
	int 		VolumeOnPlay;		// change only volume when playing has started or disable volume commands
	bool		VolumeFeedback;
	bool		AcceptNextURI;
	int			MinGapless;
	bool		SendMetaData;
	bool		SendCoverArt;
	int			MaxVolume;
	int			UPnPRemoveCount;
	char		RawAudioFormat[SQ_STR_LENGTH];
	bool		MatchEndianness;
	bool		AutoPlay;
	bool 		AllowFlac;
	bool		RoonMode;
} tMRConfig;

struct sMR {
	u32_t Magic;
	bool  InUse;
	tMRConfig Config;
	sq_dev_param_t	sq_config;
	bool on;
	char UDN			[RESOURCE_LENGTH];
	char DescDocURL		[RESOURCE_LENGTH];
	char FriendlyName	[RESOURCE_LENGTH];
	char PresURL		[RESOURCE_LENGTH];
	char Manufacturer	[RESOURCE_LENGTH];
	in_addr_t 		ip;
	enum eMRstate 	State;
	char			*CurrentURI;
	char			*NextURI;
	int				NextDuration;
	char			ProtoInfo[SQ_STR_LENGTH];		// a bit patchy ... used for faulty NEXTURI players
	sq_metadata_t	MetaData;
	sq_action_t		sqState;
	s64_t			Elapsed, Duration;
	u8_t			*seqN;
	void			*WaitCookie, *StartCookie;
	tQueue			ActionQueue;
	unsigned		TrackPoll, StatePoll;
	bool			UPnPTimeOut, UPnPConnected;
	int	 			SqueezeHandle;
	struct sService Service[NB_SRV];
	struct sAction	*Actions;
	ithread_mutex_t Mutex;
	ithread_t 		Thread;
	u8_t			Volume;
	bool			Muted;
	char			*ProtocolCap[MAX_PROTO + 1];
	bool			ProtocolCapReady;
	u16_t			ErrorCount;
	int				UPnPMissingCount;
	bool			Running;
};

extern UpnpClient_Handle   	glControlPointHandle;
extern unsigned int 		glPort;
extern char 				glIPaddress[];
extern char 				glUPnPSocket[];
extern u8_t		   			glMac[6];
extern s32_t				glLogLimit;
extern tMRConfig			glMRConfig;
extern sq_dev_param_t		glDeviceParam;
extern u32_t				gluPNPScanInterval;
extern u32_t				gluPNPScanTimeout;
extern struct sMR			glMRDevices[MAX_RENDERERS];

struct sMR 		*mr_File2Device(const char *FileName);
sq_dev_handle_t	mr_GetSqHandle(struct sMR *Device);
int 			CallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie);
int 			CallbackActionHandler(Upnp_EventType EventType, void *Event, void *Cookie);


#endif
