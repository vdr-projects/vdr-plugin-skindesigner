#ifndef __DISPLAYCHANNEL_H
#define __DISPLAYCHANNEL_H

#include <vdr/skins.h>
#include "config.h"
#include "coreengine/definitions.h"
#include "coreengine/viewdisplaychannel.h"

class cSDDisplayChannel : public cSkinDisplayChannel {
private:
    cViewChannel *view;
    bool ok;
public:
    cSDDisplayChannel(cViewChannel *channelView, bool WithInfo);
    virtual ~cSDDisplayChannel();
    virtual void SetChannel(const cChannel *Channel, int Number);
    virtual void SetEvents(const cEvent *Present, const cEvent *Following);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};
#endif //__DISPLAYCHANNEL_H
