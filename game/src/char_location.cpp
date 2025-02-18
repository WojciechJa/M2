#include "stdafx.h"
#include "char_location.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "desc_manager.h"
#include "db.h"
#include "desc_client.h"
#include "config.h"

#include "../../common/length.h"
#include "../../common/tables.h"

PlayerLocationManager::PlayerLocationManager()
{
}
PlayerLocationManager::~PlayerLocationManager()
{
}

void PlayerLocationManager::LoadLocationData(LPCHARACTER ch)
{
	TLocationSystemBaseData	data;
	data.pid = ch->GetPlayerID();
	
	db_clientdesc->DBPacket(HEADER_GD_LOCATION_SYSTEM_GET_DATA, ch->GetDesc()->GetHandle(), &data, sizeof(data));
}

void PlayerLocationManager::ReceiveLocationData(LPCHARACTER ch, const TLocationSystemPlayerData* data, int count)
{
	TPacketGCSendLocationData packet;
	packet.header = HEADER_GC_SEND_LOCATION_DATA;
	
	for (size_t i = 0; i < count; i++)
	{
		const TLocationSystemPlayerData* entry = (data + i);

		TLocationSystemWarpData* p;

		for (unsigned int j = 0; j < MAX_LOCATION_POSITION; j++)
		{
			packet.position[j] = 0;
			packet.map_index[j] = 0;
			packet.posx[j] = entry->posx[j];
			packet.posy[j] = entry->posy[j];
			
			if (entry->map_index[j] != 0)
			{
				packet.position[j] = j;
				packet.map_index[j] = entry->map_index[j];
			}
		}
		
		strlcpy(packet.name, entry->name, sizeof(packet.name));
	}
	
	if (ch->GetDesc())
		ch->GetDesc()->Packet(&packet, sizeof(packet));
}

void PlayerLocationManager::GetManagerLocationData(LPCHARACTER ch, BYTE bAction, BYTE bPosition, const char* name)
{
	const int actionMap[] = {HEADER_LOCATION_SYSTEM_DELETE, HEADER_LOCATION_SYSTEM_CREATE, HEADER_LOCATION_SYSTEM_WARP};
	ManagerLocationData(ch, bPosition, actionMap[bAction - 1], name);
}

void PlayerLocationManager::ManagerLocationData(LPCHARACTER ch, BYTE bPosition, BYTE bType, const char* name)
{
	if (bPosition < 0 || bPosition > MAX_LOCATION_POSITION)
		return;
	
	TLocationManagerData data;
	
	data.header = bType;
	data.pid = ch->GetPlayerID();
	data.position = bPosition;
	
	switch (bType)
	{
		case HEADER_LOCATION_SYSTEM_CREATE:
		{
			bool isBlackList = false;
			const int blackIndexList[] = {};
			char badKey[] = "\a \b \f \n \r \t \v \\ \? \' \" \" % & * ; : < > \? \\ \' / { | } ~"; 
			
			for (auto i : blackIndexList)
			{
				if (i == ch->GetMapIndex())
				{
					isBlackList = true;
					break;
				}
			}
			
			if (isBlackList || ch->GetDungeon())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("W tej lokalizacji nie mozesz stworzyc zapisu powrotu."));
				return;
			}
			
			if (specialChannel == g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Na tym kanale nie mozesz stworzyc zapisu powrotu."));
				return;
			}
			
			if (strpbrk(name, badKey) != NULL)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Podana nazwa zawiera niedozwolone znaki."));
				return;
			}
			
			data.map_index = ch->GetMapIndex();
			data.global_x = ch->GetX();
			data.global_y = ch->GetY();
			strlcpy(data.name, name, sizeof(data.name));
		}
		break;
		
		case HEADER_LOCATION_SYSTEM_DELETE:
		{
			strlcpy(data.name, name, sizeof(data.name));
		}
		break;
		
		case HEADER_LOCATION_SYSTEM_WARP:
		{
			if (!ch->CanWarp() || ch->IsObserverMode() || ch->IsWarping() || ch->IsDead())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("W tym momencie nie mozesz przeniesc sie w zapisane miejsce."));
				return;
			}
			
			if (ch->GetExchange() || ch->IsCubeOpen() || ch->GetMyShop() || ch->GetShop())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Pozamykaj wszystkie okna przed kolejna proba przeniesienia sie w zapisane miejsce."));
				return;
			}
		}
		break;
	}
	
	db_clientdesc->DBPacket(HEADER_GD_LOCATION_SYSTEM_MANAGER_DATA, ch->GetDesc()->GetHandle(), &data, sizeof(data));
	LoadLocationData(ch);
}

void PlayerLocationManager::ReceiveLocationWarpData(LPCHARACTER ch, int iIndex, int iGlobalX, int iGlobalY)
{
	if (ch->GetMapIndex() == iIndex)
		ch->WarpSet(iGlobalX, iGlobalY);
	else
	{
		bool isBlackList = false;
		const int blackIndexList[] = {71, 72, 73};
		
		for (auto i : blackIndexList)
		{
			if (ch->GetMapIndex() == i)
			{
				isBlackList = true;
				break;
			}
		}
		
		if (isBlackList)
		{
			int warpItem, warpCount;
			
			switch (ch->GetMapIndex())
			{
				case 71:
				{
					warpItem = 71095;
					warpCount = 1;
				}
				break;
				
				case 72:
				case 73:
				{
					warpItem = 30190;
					warpCount = 1;
				}
				break;
			}
			
			if (warpItem == 0 || warpCount == 0)
				return;
			
			if (ch->CountSpecifyItem(warpItem) == warpCount)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aby przeniesc sie na ta pozycje, potrzebujesz wymaganej przepustki."));
				return;
			}

			ch->RemoveSpecifyItem(warpItem, warpCount);
			ch->WarpSet(iGlobalX, iGlobalY);
		}
		else
			ch->WarpSet(iGlobalX, iGlobalY);
	}
}