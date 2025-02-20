#pragma once

#include "AbstractSingleton.h"

class IAbstractChat : public TAbstractSingleton<IAbstractChat>
{
	public:
		IAbstractChat() {}
		virtual ~IAbstractChat() {}

		virtual void AppendChat(int iType, const char * c_szChat) = 0;
};
//martysama0134's ceqyqttoaf71vasf9t71218
