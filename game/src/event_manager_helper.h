#pragma once
#include "../../common/tables.h"

#ifdef ENABLE_EVENT_MANAGER
struct event_struct_;
typedef struct event_struct_ TEventManagerData;

// Helper functions for game server
bool IsDontHaveEndTimeEvent(BYTE eventIndex);
void SetTimeToString(TEventManagerData* eventData);
#endif