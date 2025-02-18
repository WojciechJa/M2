#include <sstream>
#pragma once

#include "../../common/tables.h"

class PlayerLocationManager : public singleton<PlayerLocationManager>
{
	public:
		PlayerLocationManager(); ~PlayerLocationManager();
		
		int				specialChannel = 13099;
		
		void			LoadLocationData(LPCHARACTER ch);
		
		void			ReceiveLocationData(LPCHARACTER ch, const TLocationSystemPlayerData* data, int count);
		void			ReceiveLocationWarpData(LPCHARACTER ch, int iIndex, int iGlobalX, int iGlobalY);
		
		void			ManagerLocationData(LPCHARACTER ch, BYTE bPosition, BYTE bType, const char* name);
		void			GetManagerLocationData(LPCHARACTER ch, BYTE bAction, BYTE bPosition, const char* name);
};