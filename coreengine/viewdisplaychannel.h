#ifndef __VIEWDISPLAYCHANNEL_H
#define __VIEWDISPLAYCHANNEL_H

#include "view.h"

class cViewChannel : public cView {
private:
    cVeMessage             *veMessage;
    cVeCustomTokens        *veCustomTokens;
    cVeDcChannelInfo       *veChannelInfo;
    cVeDcChannelGroup      *veChannelGroup;
    cVeDcEpgInfo           *veEpgInfo;
    cVeDcProgressBar       *veProgressBar;
    cVeDcStatusInfo        *veStatusInfo;
    cVeDcScraperContent    *veScraperContent;
    cVeDcEcmInfo           *veEcmInfo;
    bool channelChange;
    bool displayChannelGroups;
    void SetViewElements(void);
    void ClearVariables(void);
    void SetViewElementObjects(void);
public:
    cViewChannel(void);
    virtual ~cViewChannel(void);
    void SetChannel(const cChannel *channel, int number);
    void SetEvents(const cEvent *present, const cEvent *following);
    void SetMessage(eMessageType type, const char *text);
    void Flush(void);
};

#endif //__VIEWDISPLAYCHANNEL_H