#include "stdafx.h"
#include "ClientManager.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"
#include "Cache.h"
#include "Main.h"
#include "buffer_manager.h"

#include "fstream"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include <boost/scoped_ptr.hpp>
#include <memory>

#include "../../common/tables.h"

std::vector<std::string> splitName(const std::string &s, char separator)
{
	std::vector<std::string> result;
	std::stringstream ss (s);
	std::string item;
	
	while (getline(ss, item, separator))
	{
		result.push_back(item);
	}
	
	return result;
}

CLocationSystemCache* CClientManager::GetLocationSystemCache(DWORD pid)
{
	itertype(m_mapLocationSystemCache) it = m_mapLocationSystemCache.find(pid);
	
	if (it == m_mapLocationSystemCache.end())
		return NULL;
	
	return it->second;
}

void CClientManager::CreateLocationSystemCache(DWORD pid)
{
	TLocationSystemData	data;
	data.pid = pid;
	
	for (unsigned int i = 0; i < MAX_LOCATION_POSITION + 1; i++)
	{
		data.map_index[i] = 0;
		data.global_x[i] = 0;
		data.global_y[i] = 0;
	}
	
	InsertLocationSystemCache(data);
}

void CClientManager::InsertLocationSystemCache(TLocationSystemData &data, bool bSkip)
{
	CLocationSystemCache* cache;
	cache = GetLocationSystemCache(data.pid);
	
	if (!cache)
	{
		cache = new CLocationSystemCache;
		m_mapLocationSystemCache.insert(TLocationSystemCacheMap::value_type(data.pid, cache));
	}
	
	cache->Put(&data, bSkip);
}

void CClientManager::UpdateLocationSystemCache(DWORD pid)
{
	std::string szNames;
	
	TLocationSystemData	data;
	data.pid = pid;
	
	for (unsigned int i = 0; i < MAX_LOCATION_POSITION + 1; i++)
	{
		data.map_index[i] = 0;
		data.global_x[i] = 0;
		data.global_y[i] = 0;
	}
	
	char szQuery[QUERY_MAX_LEN];
	
	snprintf(szQuery, sizeof(szQuery), "SELECT window_pos, map_index, global_x, global_y, name FROM srv1_player.player_location WHERE pid = %d", pid);
	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows != 0)
	{
		SQLResult *pRes = pMsg->Get();
		
		if (pRes->uiNumRows)
		{
			MYSQL_ROW row;
			
			while ((row = mysql_fetch_row(pRes->pSQLResult)))
			{
				int idx = 1;
				int position = atoi(row[0]);
				
				data.map_index[position] = atoi(row[idx++]);
				data.global_x[position] = atoi(row[idx++]);
				data.global_y[position] = atoi(row[idx++]);
				
				szNames += std::to_string(position);
				szNames += "#";
				szNames += row[4];
				szNames += "#";
			}
			
			strlcpy(data.name, szNames.c_str(), sizeof(data.name));
		}
	}
	
	szNames.clear();
	InsertLocationSystemCache(data, true);
}

void CClientManager::UpdateLocationSystemData()
{
	itertype(m_mapLocationSystemCache) it = m_mapLocationSystemCache.begin();
	
	while (it != m_mapLocationSystemCache.end())
	{
		CLocationSystemCache* pCache = (it++)->second;
		
		if (pCache->CheckFlushTimeout())
			pCache->Flush();
	}
}

void CClientManager::GetLocationSystemData(CPeer *peer, DWORD dwHandle, const char *data)
{
	const TLocationSystemBaseData* p = (const TLocationSystemBaseData*) data;
	CLocationSystemCache* pCache = GetLocationSystemCache(p->pid);
	
	if (!pCache)
		return;
	
	TLocationSystemBaseData pack;
	pack.pid = p->pid;
	
	const int dataSize = m_mapLocationSystemCache.size();
	
	std::vector<TLocationSystemData*> locationData;
	locationData.reserve(dataSize);
	
	itertype(m_mapLocationSystemCache) it = m_mapLocationSystemCache.begin();
	
	while (it != m_mapLocationSystemCache.end())
	{
		CLocationSystemCache* pCache = (it++)->second;
		TLocationSystemData* data = pCache->Get(false);
		
		if (data)
			locationData.push_back(data);
	}
	
	TEMP_BUFFER buffer;
	TLocationSystemPlayerData playerData;
	
	for (size_t dataCache = 0; dataCache < dataSize; dataCache++)
	{
		if (locationData[dataCache]->pid == p->pid)
		{
			for (unsigned int i0 = 0; i0 < MAX_LOCATION_POSITION + 1; i0++)
				playerData.map_index[i0] = locationData[dataCache]->map_index[i0];

			//XD
			for (unsigned int i2 = 0; i2 < MAX_LOCATION_POSITION + 1; i2++)
				playerData.posx[i2] = locationData[dataCache]->global_x[i2];

			for (unsigned int i3 = 0; i3 < MAX_LOCATION_POSITION + 1; i3++)
				playerData.posy[i3] = locationData[dataCache]->global_y[i3];
			//p100 code

			strlcpy(playerData.name, locationData[dataCache]->name, sizeof(playerData.name));
			
			buffer.write(&playerData, sizeof(playerData));
			break;
		}
	}
	
	const int count = buffer.size() / sizeof(playerData);
	
	peer->EncodeHeader(HEADER_DG_LOCATION_SYSTEM_SEND_DATA, dwHandle, sizeof(pack) + sizeof(count) + buffer.size());
	peer->Encode(&pack, sizeof(pack));
	peer->Encode(&count, sizeof(count));
	
	if (buffer.size() != 0)
		peer->Encode(buffer.read_peek(), buffer.size());
}

void CClientManager::ManagerLocationSystemData(CPeer* peer, DWORD dwHandle, const char* data)
{
	const TLocationManagerData* p = (const TLocationManagerData*) data;
	CLocationSystemCache* pCache = GetLocationSystemCache(p->pid);
	TLocationSystemData* playerData = pCache->Get(false);
	
	if (!pCache)
		return;

	if (p->position < 0 || p->position > MAX_LOCATION_POSITION)
		return;
	
	switch (p->header)
	{
		case HEADER_LOCATION_SYSTEM_DELETE:
		{
			char szQuery[QUERY_MAX_LEN];
			
			if (playerData->map_index[p->position] == 0)
				return;
			
			snprintf(szQuery, sizeof(szQuery), "DELETE FROM srv1_player.player_location WHERE pid = %d AND window_pos = %d", p->pid, p->position);
			CDBManager::instance().DirectQuery(szQuery);
			
			playerData->map_index[p->position] = 0;
			playerData->global_x[p->position] = 0;
			playerData->global_y[p->position] = 0;
			strlcpy(playerData->name, p->name, sizeof(playerData->name));
			
			pCache->Put(playerData);
		}
		break;
		
		case HEADER_LOCATION_SYSTEM_CREATE:
		{
			char szQuery[QUERY_MAX_LEN];
			
			if (playerData->map_index[p->position] != 0)
				return;
			
			snprintf(szQuery, sizeof(szQuery), "INSERT INTO srv1_player.player_location SET pid = %d, map_index = %d, global_x = %d, global_y = %d, window_pos = %d", p->pid, p->map_index, p->global_x, p->global_y, p->position);
			CDBManager::instance().DirectQuery(szQuery);
			
			std::vector <std::string> vec_Name;
			
			for (unsigned int i = 0; i < MAX_LOCATION_POSITION; i++)
				vec_Name.push_back("");
			
			std::vector<std::string> vec_splitName = splitName(p->name, '#');
			
			for (unsigned int j = 0; j < vec_splitName.size(); j++)
			{
				if (j % 2 - 1)
					vec_Name[atoi(vec_splitName[j].c_str())] = vec_splitName[j + 1].c_str();
			}
			
			for (unsigned int k = 0; k < MAX_LOCATION_POSITION; k++)
			{
				if (k == p->position)
				{
					snprintf(szQuery, sizeof(szQuery), "UPDATE srv1_player.player_location SET name = '%s' WHERE window_pos = %d", vec_Name[k].c_str(), p->position);
					CDBManager::instance().DirectQuery(szQuery);
				}
			}
			
			playerData->map_index[p->position] = p->map_index;
			playerData->global_x[p->position] = p->global_x;
			playerData->global_y[p->position] = p->global_y;
			strlcpy(playerData->name, p->name, sizeof(playerData->name));
			
			pCache->Put(playerData);
		}
		break;
		
		case HEADER_LOCATION_SYSTEM_WARP:
		{
			if (playerData->map_index[p->position] == 0)
				return;
			
			TEMP_BUFFER buffer;
			TLocationSystemWarpData	dataWarp;
			
			dataWarp.pid = p->pid;
			dataWarp.map_index = playerData->map_index[p->position];
			dataWarp.global_x = playerData->global_x[p->position];
			dataWarp.global_y = playerData->global_y[p->position];
			
			buffer.write(&dataWarp, sizeof(dataWarp));
			
			peer->EncodeHeader(HEADER_DG_LOCATION_SYSTEM_WARP_DATA, dwHandle, sizeof(TLocationSystemWarpData) + buffer.size());
			peer->Encode(&dataWarp, sizeof(TLocationSystemWarpData));
			
			if (buffer.size() != 0)
				peer->Encode(buffer.read_peek(), buffer.size());
		}
		break;
	}
}