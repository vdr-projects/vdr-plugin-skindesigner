#ifndef __VIEWELEMENTSDC_H
#define __VIEWELEMENTSDC_H

#include "viewelement.h"
#include "../extensions/scrapmanager.h"
#include "../services/dvbapi.h"

/******************************************************************
* cVeDcChannelInfo
******************************************************************/
class cVeDcChannelInfo : public cViewElement {
private:
public:
    cVeDcChannelInfo(void);
    virtual ~cVeDcChannelInfo(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c, int number);
};
/******************************************************************
* cVeDcChannelGroup
******************************************************************/
class cVeDcChannelGroup : public cViewElement {
private:
    const char *GetChannelSep(const cChannel *c, bool prev);
public:
    cVeDcChannelGroup(void);
    virtual ~cVeDcChannelGroup(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c);
};
/******************************************************************
* cVeDcEpgInfo
******************************************************************/
class cVeDcEpgInfo : public cViewElement {
private:
    bool EventHasTimer(const cEvent *e);
public:
    cVeDcEpgInfo(void);
    virtual ~cVeDcEpgInfo(void);
    void SetTokenContainer(void);
    void Set(const cEvent *p, const cEvent *f);
};
/******************************************************************
* cVeDcProgressBar
******************************************************************/
class cVeDcProgressBar : public cViewElement {
private:
    int currentLast;
    int startTime;
    int duration;
    int GetLiveBuffer(void);
public:
    cVeDcProgressBar(void);
    virtual ~cVeDcProgressBar(void);
    void Close(void);
    void SetTokenContainer(void);
    void Set(const cEvent *p);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcStatusInfo
******************************************************************/
class cVeDcStatusInfo : public cViewElement {
private:
    bool CheckMails(void);
public:
    cVeDcStatusInfo(void);
    virtual ~cVeDcStatusInfo(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c);
};
/******************************************************************
* cVeDcAudioInfo
******************************************************************/
class cVeDcAudioInfo : public cViewElement {
private:
    int lastNumAudioTracks;
    int lastAudioChannel;
    char *lastTracDesc;
    char *lastTrackLang;
public:
    cVeDcAudioInfo(void);
    virtual ~cVeDcAudioInfo(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcScreenResolution
******************************************************************/
class cVeDcScreenResolution : public cViewElement {
private:
    int lastScreenWidth;
    int lastScreenHeight;
public:
    cVeDcScreenResolution(void);
    virtual ~cVeDcScreenResolution(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcSignalQuality
******************************************************************/
class cVeDcSignalQuality : public cViewElement {
private:
    int lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
public:
    cVeDcSignalQuality(void);
    virtual ~cVeDcSignalQuality(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDcScraperContent
******************************************************************/
class cVeDcScraperContent : public cViewElement, public cScrapManager {
private:
public:
    cVeDcScraperContent(void);
    virtual ~cVeDcScraperContent(void);
    void SetTokenContainer(void);
    void Set(const cEvent *e);
};
/******************************************************************
* cVeDcEcmInfo
******************************************************************/
class cVeDcEcmInfo : public cViewElement {
private:
    int channelSid;
    sDVBAPIEcmInfo lastEcmInfo;
    bool CompareECMInfos(sDVBAPIEcmInfo *ecmInfo);
public:
    cVeDcEcmInfo(void);
    virtual ~cVeDcEcmInfo(void);
    void Close(void);
    void SetTokenContainer(void);
    void Set(const cChannel *c);
    bool Parse(bool forced = false);
};

#endif //__VIEWELEMENTSDC_H