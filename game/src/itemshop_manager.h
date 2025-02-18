#pragma once
#include "stdafx.h"

#ifdef __ITEM_SHOP__

enum EEditorFlag 
{
	EDITOR_FLAG_ADD,
	EDITOR_FLAG_EDIT,
	EDITOR_FLAG_DELETE
};

class CItemShopManager : public singleton<CItemShopManager>
{
	std::vector<TItemShopItem*>		m_vec_itemShopItems;
	std::set<LPCHARACTER>			m_set_pkCurrentViewer;
	std::vector<std::string>		m_vec_pkEditors;
	std::vector<TItemShopBuyStats>	m_vec_itemShopBuyStats;
	std::vector<TItemShopBuyStats>	m_vec_itemShopBuyStatsSave;
	std::map<BYTE, std::string>		m_map_itemShopCategory;
	time_t							m_expireTime;
	time_t							m_lastFlushTime;
	bool							m_bNeedFlush;
	time_t							m_nextUpdate;
	bool							m_bNeedUpdate;
	time_t							m_ttDataTime;

public:
	CItemShopManager();
	~CItemShopManager();
	void						Destroy();
	
	void						Initialize(TItemShopRefreshPacket* pItem = nullptr);
	TItemShopItem*				FindItemByID(DWORD id, DWORD category, BYTE bonus_type = 0);
	void						OpenItemShop(LPCHARACTER ch, time_t time, bool reload = false);
	void						SendItem(TItemShopItem* item, BYTE header, LPCHARACTER ch = nullptr);
	void						SendError(BYTE byError, LPCHARACTER ch);
	void						SendCoins(LPCHARACTER ch);
	void						BuyItem(LPCHARACTER ch, DWORD id, DWORD category, int count, BYTE bonus_type);
	void						ManageItem(LPCHARACTER ch, BYTE flag, TItemShopItem item, bool by_system=false);
	void						ReloadItems(LPCHARACTER ch);
	bool						IsViewer(LPCHARACTER ch);
	void						RemoveViewer(LPCHARACTER ch);
	void						ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch = nullptr, bool send_to_all = false);
	void						FlushStatistics();
	void						InitializeStatistics();
	void						RefreshLimitedTime();

	int							GetCooldownManage() const {return m_Cooldown;}
	void						SetCooldownManage() {m_Cooldown = thecore_pulse();}

	static void					FormatTime(int ms, char* buffer, size_t size);

protected:
	int							m_Cooldown;

private:
	bool						IsEditor(LPCHARACTER ch) const;
	void						AddViewer(LPCHARACTER ch);
};
#endif
