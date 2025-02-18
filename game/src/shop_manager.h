#ifndef __INC_METIN_II_GAME_SHOP_MANAGER_H__
#define __INC_METIN_II_GAME_SHOP_MANAGER_H__

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	void	Destroy();

	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);

	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);

	void	Buy(LPCHARACTER ch, BYTE pos);

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
#ifdef __EXTENDED_ITEM_COUNT__
	void Sell(LPCHARACTER ch, WORD wCell, int bCount = 0, BYTE bType = 0);
#else
	void Sell(LPCHARACTER ch, WORD wCell, BYTE bCount = 0, BYTE bType = 0);
#endif
#else
#ifdef __EXTENDED_ITEM_COUNT__
	void Sell(LPCHARACTER ch, WORD wCell, int bCount = 0, BYTE bType = 0);
#else
	void Sell(LPCHARACTER ch, WORD wCell, BYTE bCount = 0, BYTE bType = 0);
#endif
#endif

#ifdef __EXTENDED_ITEM_COUNT__
	LPSHOP CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, int bItemCount);
#else
	LPSHOP CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, BYTE bItemCount);
#endif
	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;

	bool	ReadShopTableEx(const char* stFileName);
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
