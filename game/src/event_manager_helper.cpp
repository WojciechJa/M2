#include "stdafx.h"
#include "event_manager_helper.h"

#ifdef ENABLE_EVENT_MANAGER
bool IsDontHaveEndTimeEvent(BYTE eventIndex)
{
    switch (eventIndex)
    {
        case EMPIRE_WAR_EVENT:
        case TOURNAMENT_EVENT:
            return true;
    }
    return false;
}

void SetTimeToString(TEventManagerData* eventData)
{
    if (eventData->startTime <= 0)
        snprintf(eventData->startTimeText, sizeof(eventData->startTimeText), "1970-01-01 00:00:00");
    else
    {
        const time_t startTime = eventData->startTime;
        const struct tm vEventStartKey = *localtime(&startTime);
        snprintf(eventData->startTimeText, sizeof(eventData->startTimeText), "%d-%02d-%02d %02d:%02d:%02d",
            vEventStartKey.tm_year + 1900, vEventStartKey.tm_mon + 1, vEventStartKey.tm_mday,
            vEventStartKey.tm_hour, vEventStartKey.tm_min, vEventStartKey.tm_sec);
    }

    if (eventData->endTime <= 0)
        snprintf(eventData->endTimeText, sizeof(eventData->endTimeText), "1970-01-01 00:00:00");
    else
    {
        const time_t endTime = eventData->endTime;
        const struct tm vEventEndKey = *localtime(&endTime);
        snprintf(eventData->endTimeText, sizeof(eventData->endTimeText), "%d-%02d-%02d %02d:%02d:%02d",
            vEventEndKey.tm_year + 1900, vEventEndKey.tm_mon + 1, vEventEndKey.tm_mday,
            vEventEndKey.tm_hour, vEventEndKey.tm_min, vEventEndKey.tm_sec);
    }
}
#endif