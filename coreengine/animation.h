#ifndef __ANIMATION_H
#define __ANIMATION_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "definitions.h"

#define FPS 50

/******************************************************************
* cScrollable
******************************************************************/
class cScrollable {
protected:
    cScrollable(void) {};
    ~cScrollable(void) {};
public:
    virtual int ScrollDelay(void) = 0;
    virtual int ScrollWidth(void) = 0;
    virtual int ScrollHeight(void) = 0;
    virtual eScrollMode ScrollMode(void) = 0;
    virtual eScrollSpeed ScrollSpeed(void) = 0;
    virtual eOrientation ScrollOrientation(void) = 0;
    virtual void StartScrolling(void) = 0;
    virtual void StopScrolling(void) = 0;
    virtual void SetDrawPort(cPoint &point) = 0;
    virtual void Flush(void) = 0;
};

/******************************************************************
* cDetachable
******************************************************************/
class cDetachable {
protected:
    cDetachable(void) {};
    ~cDetachable(void) {};
public:
    virtual int Delay(void) = 0;
    virtual void ParseDetached(void) = 0;
    virtual void RenderDetached(void) = 0;
    virtual void StartAnimation(void) = 0;
    virtual void Flush(void) = 0;
};

/******************************************************************
* cFadable
******************************************************************/
class cFadable {
protected:
    cFadable(void) {};
    ~cFadable(void) {};
public:
    virtual bool Detached(void) = 0;
    virtual int Delay(void) = 0;
    virtual int FadeTime(void) = 0;
    virtual void SetTransparency(int transparency, bool force = false) = 0;
    virtual void Flush(void) = 0;
};

/******************************************************************
* cShiftable
******************************************************************/
class cShiftable {
protected:
    cShiftable(void) {};
    ~cShiftable(void) {};
public:
    virtual bool Detached(void) = 0;
    virtual int Delay(void) = 0;
    virtual int ShiftTime(void) = 0;
    virtual int ShiftMode(void) = 0;
    virtual void SetPosition(cPoint &position, cPoint &reference, bool force = false) = 0;
    virtual void Flush(void) = 0;
};

/******************************************************************
* cBlinkable
******************************************************************/
class cBlinkable {
protected:
    cBlinkable(void) {};
    ~cBlinkable(void) {};
public:
    virtual int BlinkFreq(int func) = 0;
    virtual void DoBlink(int func, bool on) = 0;
    virtual void Flush(void) = 0;
};

/******************************************************************
* cAnimation
******************************************************************/
class cAnimation : public cThread, public cListObject {
private:
    cCondWait sleepWait;
    cScrollable *scrollable;
    cDetachable *detachable;
    cFadable    *fadable;
    cShiftable  *shiftable;
    cBlinkable  *blinkable;
    bool waitOnWakeup;
    bool keepSleeping;
    bool doAnimation;
    bool modeIn;
    int blinkFunc;
    cPoint shiftstart;
    cPoint shiftend;
    void Sleep(int duration);
    void Wait(void);
    void Scroll(void);
    void Detach(void);    
    void Blink(void);
protected:
    virtual void Action(void);
public:
    cAnimation(cScrollable *scrollable);
    cAnimation(cDetachable *detachable, bool wait, bool animation);
    cAnimation(cFadable    *fadable, bool fadein);
    cAnimation(cShiftable  *shiftable, cPoint &start, cPoint &end, bool shiftin);
    cAnimation(cBlinkable  *blinkable, int func);
    ~cAnimation(void);
    void WakeUp(void);
    void ResetSleep(void);
    void Fade(void);
    void Shift(void);
    void Stop(bool deletePixmaps);
};

#endif //__ANIMATION_H