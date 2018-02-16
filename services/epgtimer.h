/*
 * service.h: EPG2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <vdr/timers.h>
#include <vdr/epg.h>

#include <list>

//***************************************************************************
// Timer - Skin Interface
//***************************************************************************

class cEpgEvent_Interface_V1 : public cEvent
{
   public:

      cEpgEvent_Interface_V1(tEventID EventID)
         : cEvent(EventID) {}

      // #TODO ... getter

   protected:

      // #TODO ... attributes
};

//***************************************************************************
// Timer - Skin Interface
//***************************************************************************

class cEpgTimer_Interface_V1 : public cTimer
{
   public:

      enum TimerType
      {
         ttRecord = 'R',   // Aufnahme-Timer
         ttView   = 'V',   // Umschalt-Timer
         ttSearch = 'S'    // Such-Timer
      };

      cEpgTimer_Interface_V1(bool Instant = false, bool Pause = false, const cChannel* Channel = 0)
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
	 : cTimer(Instant, Pause, Channel) {}
#else
	 : cTimer(Instant, Pause, (cChannel*)Channel) {}
#endif

      long TimerId()               const { return timerid; }
      long EventId()               const { return eventid; }
      const char* VdrName()        const { return vdrName ? vdrName : ""; }
      const char* VdrUuid()        const { return vdrUuid ? vdrUuid : ""; }
      int isVdrRunning()           const { return vdrRunning; }
      int isLocal()                const { return local; }
      int isRemote()               const { return !isLocal(); }
      int isRecordTimer()          const { return type == ttRecord; }
      int isSwithTimer()           const { return type == ttView; }
      char State()                 const { return state; }
      int  hasState(char s)        const { return state == s; }
      const char* StateInfo()      const { return stateInfo ? stateInfo : ""; }
      char Action()                const { return action; }
      char Type()                  const { return type; }
      time_t CreateTime()          const { return createTime; }
      time_t ModTime()             const { return modTime; }

   protected:

      long timerid;
      long eventid;

      char* vdrName;
      char* vdrUuid;
      int local;
      int vdrRunning;

      char state;
      char* stateInfo;
      char action;

      char type;
      time_t createTime;
      time_t modTime;
};

//***************************************************************************
// Timer - Service Interface
//***************************************************************************

struct cEpgTimer_Service_V1
{
   std::list<cEpgTimer_Interface_V1*> epgTimers;
};

#define EPG2VDR_TIMER_SERVICE "Epg2Vdr_Timer_Service-v1.0"

#ifdef EPG2VDR

//***************************************************************************
// Class cEpgEvent
//***************************************************************************

class cEpgEvent : public cEpgEvent_Interface_V1
{
   public:

      cEpgEvent(tEventID EventID);
      virtual ~cEpgEvent() {}

      // #TODO ... setter
};

//***************************************************************************
// Class cEpgTimer
//***************************************************************************

class cEpgTimer : public cEpgTimer_Interface_V1
{
   public:

      cEpgTimer(bool Instant = false, bool Pause = false, cChannel* Channel = 0);
      virtual ~cEpgTimer();

      void setTimerId(long id)    { timerid = id; }
      void setEventId(long id)    { eventid = id; }
      void setState(char s, const char* info);
      void setAction(char a);
      void setVdr(const char* name, const char* uuid = 0, int running = 0);
};

#endif // EPG2VDR

//***************************************************************************

#endif // _SERVICE_H_
