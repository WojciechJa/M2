#include "stdafx.h"
#include "constants.h"
#include "block_country.h"

#define DEC_ITER(iter)			std::vector<T_BLOCK_IP*>::iterator	iter
#define DO_ALL_BLOCKED_IP(iter)	for ((iter)=s_blocked_ip.begin(); (iter)!=s_blocked_ip.end(); ++(iter))

#define DEC_EXCEPTION_ITER(iter)	std::set<std::string>::iterator	iter

typedef struct S_BLOCK_IP {
	DWORD	ip_from;
	DWORD	ip_to;
} T_BLOCK_IP;

//--------------------------------------
// static variables
std::vector<T_BLOCK_IP*>	s_blocked_ip;
std::set<std::string>		s_block_exception;
// static variables
//--------------------------------------

//--------------------------------------
// static functions
static void __add_block_exception(const char *login)
{
	sys_log(1, "BLOCK_EXCEPTION_ADD : %s", login);

	DEC_EXCEPTION_ITER(iter);
	std::string	string_login(login);

	iter = s_block_exception.find(string_login);

	// can not find
	if (iter==s_block_exception.end())
	{
		s_block_exception.emplace(string_login);
	}
}

static void __del_block_exception(const char *login)
{
	sys_log(1, "BLOCK_EXCEPTION_DEL : %s", login);

	DEC_EXCEPTION_ITER(iter);
	std::string	string_login(login);

	iter = s_block_exception.find(string_login);

	// ok : find
	if (iter!=s_block_exception.end())
	{
		s_block_exception.erase(iter);
	}
}
// static functions
//--------------------------------------

void add_blocked_country_ip(TPacketBlockCountryIp *data)
{
	T_BLOCK_IP	*block_ip = M2_NEW T_BLOCK_IP;

	block_ip->ip_from	= data->ip_from;
	block_ip->ip_to		= data->ip_to;

	s_blocked_ip.emplace_back(block_ip);

	sys_log(1, "BLOCKED_IP = %u - %u", block_ip->ip_from, block_ip->ip_to);
}

void block_exception(TPacketBlockException *data)
{
	if (NULL==data) return;

	if (BLOCK_EXCEPTION_CMD_ADD!=data->cmd && BLOCK_EXCEPTION_CMD_DEL!=data->cmd)
		return;

	switch (data->cmd)
	{
		case BLOCK_EXCEPTION_CMD_ADD:
			__add_block_exception(data->login);
			break;
		case BLOCK_EXCEPTION_CMD_DEL:
			__del_block_exception(data->login);
			break;
	}
}

bool is_blocked_country_ip(const char *user_ip)
{
	DEC_ITER(iter);
	T_BLOCK_IP		*block_ip;
	DWORD			ip_number;
	struct in_addr	st_addr;

#ifndef __WIN32__
	if (0 == inet_aton(user_ip, &st_addr))
#else
	unsigned long in_address;
	in_address = inet_addr(user_ip);
	st_addr.s_addr = in_address;
	if (INADDR_NONE == in_address)
#endif
	{
		sys_log(0, "BLOCKED_COUNTRY_IP (%s) : YES", user_ip);
		return true;
	}
	ip_number = htonl(st_addr.s_addr);

	DO_ALL_BLOCKED_IP(iter)
	{
		block_ip	= *iter;
		if ( block_ip->ip_from <= ip_number  &&  ip_number <= block_ip->ip_to )
		{
			sys_log(0, "BLOCKED_COUNTRY_IP (%s) : YES", user_ip);
			return true;
		}
	}

	sys_log(0, "BLOCKED_COUNTRY_IP (%s) : NO", user_ip);
	return false;
}

bool is_block_exception(const char *login)
{
	std::string		login_string(login);
	std::set<std::string>::iterator	iter;

	iter = s_block_exception.find(login_string);
	if (iter != s_block_exception.end())
		return true;

	return false;
}
//martysama0134's ceqyqttoaf71vasf9t71218
