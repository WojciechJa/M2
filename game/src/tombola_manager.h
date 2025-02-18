#pragma once
class CTombola : public singleton<CTombola>
{
	public:
		typedef struct tombola_item
		{
			DWORD	dwVnum;
			DWORD	dwCount;
			DWORD	dwChance;

			tombola_item()
			{
				dwVnum = 0;
				dwCount = 0;
				dwChance = 0;
			}
		} TTombolaItem;

		typedef std::vector<TTombolaItem> TTombolaVec;
		typedef struct tombola_group
		{
			DWORD dwPrice;
			DWORD dwPriceType;
			TTombolaItem	pkMainItem;
			TTombolaVec pkItem;
		} TTombolaGroup;


		typedef std::map<BYTE, TTombolaGroup> TTombolaMap;


		CTombola();
		~CTombola();

		void						Initialize();

		TTombolaVec					CreatePrize(BYTE byStage);

		void						RequestOpen(LPCHARACTER ch);
		void						RequestStart(LPCHARACTER ch, BYTE byGroup);
		static void					GiveItem(LPCHARACTER ch);
	protected:
		static void					ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch);

	private:
		TTombolaMap					m_map_pkItem;

		enum ETombolaPriceType
		{
			TOMBOLA_PRICE_YANG,
			TOMBOLA_PRICE_CASH,
		};
};
