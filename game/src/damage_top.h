/*
/\><><><><><><><><><><><><><><><><><><><><><><><><><><><>/\
\/        |===================================|          \/
/\        |      System: Top Damage v1.2      |          /\
\/        | Poprawiona wersja przez kamil2321 |          \/
/\        |===================================|          /\
\/                     GG: 1889862                       \/
/\             strona: https://kamil2321.pl              /\
\/             Forum: https://metin2time.eu              \/
/\                   Google: kamil2321                   /\
\/                  YouTube: kamil2321                   \/
/\          |===================================|        /\
\/  Ca³kowity zakaz handlowania itp. Do u¿ytku w³asnego! \/
/\  Nie pozwalam wstawiaæ na inne fora bez mojej zgody.  /\
\/    W razie problemów/ poprawek itp. proszê pisaæ.     \/
/\><><><><><><><><><><><><><><><><><><><><><><><><><><><>/\
*/
#ifndef __DAMAGE_TOP_H__
#define __DAMAGE_TOP_H__

#include "packet.h"

//typedef struct SDamage {
//	SDamage() { dwDamageValue = 0; }
//
//	char szAttackerName[CHARACTER_NAME_MAX_LEN + 1];
//	BYTE bAttackerLevel;
//	BYTE bAttackerEmpire;
//
//	char szVictimName[CHARACTER_NAME_MAX_LEN + 1];
//	DWORD dwDamageValue;
//	BYTE bDamageType;
//} TDamage;

enum EDamageTopTypes
{
	DAMAGE_TOP_TYPE_NONE,
	DAMAGE_TOP_TYPE_HIT_PVP,
	DAMAGE_TOP_TYPE_SKILL_PVP,
	DAMAGE_TOP_TYPE_HIT_PVM,
	DAMAGE_TOP_TYPE_SKILL_PVM,
	DAMAGE_TOP_TYPE_MAX,
};

class CDamageTop : public singleton<CDamageTop>
{
public:
	CDamageTop();
	virtual ~CDamageTop();

	void		Initialize();

	TDamage 	GetDamage(LPCHARACTER pkChar, BYTE bType);
	void 		UpdateDamage(LPCHARACTER pkChar, LPCHARACTER pkVictim, BYTE bType, DWORD dam);

	void 		UpdateDamageExtern(LPDESC d, const char* c_pData);
	void 		SendGGUpdateDamage(TDamage damageT);

	void		RegisterHitDamage(LPCHARACTER pkVictim, LPCHARACTER pkAttacker, DWORD dam);
	void		RegisterSkillDamage(LPCHARACTER pkVictim, LPCHARACTER pkAttacker, DWORD dam);

	void 		GetSendVector(std::vector<TDamage>& retVector);
	void 		OpenDamageTop(LPCHARACTER pkChar);
private:
	std::vector<TDamage> vecDamageTop;
};
#endif
