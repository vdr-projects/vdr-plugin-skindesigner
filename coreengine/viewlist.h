#ifndef __VIEWLIST_H
#define __VIEWLIST_H

#include "globals.h"
#include "../libskindesignerapi/tokencontainer.h"
#include "listelements.h"
#include "area.h"

class cViewList {
protected:
    int plugId;
    int plugMenuId;
    cViewListAttribs *attribs;
    cRect container;
    cGlobals *globals;
    int numElements;
    eOrientation orientation;
    cViewElement *listElement;
    cViewElement *currentElement;
    cListElement **listElements;
    virtual void Prepare(int start, int step) {};
public:
    cViewList(void);
    virtual ~cViewList(void);
    void SetGlobals(cGlobals *globals);    
    void SetContainer(int x, int y, int width, int height);
    void SetAttributes(vector<stringpair> &attributes);
    void SetPlugId(int id) { plugId = id; };
    void SetPlugMenuId(int id) { plugMenuId = id; };
    static cViewList *CreateViewList(const char *name);
    static cViewElement *CreateListElement(const char *name);
    static cViewElement *CreateCurrentElement(const char *name);
    void AddListElement(cViewElement *listElement);
    void AddCurrentElement(cViewElement *currentElement);
    virtual void PreCache(void);
    int NumItems(void);
    eOrientation Orientation(void);
    void Draw(eMenuCategory menuCat);
    void Clear(void);
    virtual void Close(void);
    void SetTransparency(int transparency);
    void Debug(void);
};

class cViewListDefault : public cViewList {
private:
    cLeMenuDefault **listDefault;
    int avrgFontWidth;
    const cFont *listFont;
    int *colX;
    int *colWidths;
    const char *plugName;
protected:
    void Prepare(int start, int step);
public:
    cViewListDefault(void);
    virtual ~cViewListDefault(void);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    void SetPlugin(const char *plugName) { this->plugName = plugName; };
    void Set(const char *text, int index, bool current, bool selectable);
    const cFont *GetListFont(void);
    int GetListWidth(void);
};

class cViewListMain : public cViewList {
private:
    cLeMenuMain **listMain;
    cCeMenuMain *currentMain;
protected:
    void Prepare(int start, int step);
public:
    cViewListMain(void);
    virtual ~cViewListMain(void);
    void Set(const char *text, int index, bool current, bool selectable);
    const char *GetPlugin(void);
};

class cViewListSchedules : public cViewList {
private:
    cLeMenuSchedules **listSchedules;
    cCeMenuSchedules *currentSchedules;
    bool epgSearchFav;
protected:
    void Prepare(int start, int step);
public:
    cViewListSchedules(void);
    virtual ~cViewListSchedules(void);
    void IsEpgSearchFav(bool isFav) { epgSearchFav = isFav; };
    void Set(const cEvent *event, int index, bool current, bool selectable, const cChannel *channel, bool withDate, eTimerMatch timerMatch);
};

class cViewListTimers : public cViewList {
private:
    cLeMenuTimers **listTimers;
    cCeMenuTimers *currentTimer;
protected:
    void Prepare(int start, int step);
public:
    cViewListTimers(void);
    virtual ~cViewListTimers(void);
    void Set(const cTimer *timer, int index, bool current, bool selectable);
};

class cViewListChannels : public cViewList {
private:
    cLeMenuChannels **listChannels;
    cCeMenuChannels *currentChannel;
protected:
    void Prepare(int start, int step);
public:
    cViewListChannels(void);
    virtual ~cViewListChannels(void);
    void Set(const cChannel *channel, int index, bool current, bool selectable, bool withProvider);
};

class cViewListRecordings : public cViewList {
private:
    cLeMenuRecordings **listRecordings;
    cCeMenuRecordings *currentRecording;
protected:
    void Prepare(int start, int step);
public:
    cViewListRecordings(void);
    virtual ~cViewListRecordings(void);
    void Set(const cRecording *recording, int index, bool current, bool selectable, int level, int total, int New);
};

class cViewListPlugin : public cViewList {
private:
    cLeMenuPlugin **listPlugin;
    cCeMenuPlugin *currentPlugin;
protected:
    void Prepare(int start, int step);
public:
    cViewListPlugin(void);
    virtual ~cViewListPlugin(void);
    void Set(skindesignerapi::cTokenContainer *tk, int index, bool current, bool selectable);
};

class cViewListAudioTracks : public cViewList {
private:
    skindesignerapi::cTokenContainer *tokenContainer;
    int numTracks;
    cLeAudioTracks **listAudioTracks;
public:
    cViewListAudioTracks(void);
    virtual ~cViewListAudioTracks(void);
    void Close(void);
    void PreCache(void);
    void SetNumtracks(int numTracks);
    void SetTracks(const char * const *tracks);
    void SetCurrentTrack(int index);
    void Draw(void);
};
#endif //__VIEWLIST_H