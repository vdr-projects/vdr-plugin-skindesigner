#include "displaychannel.h"

cSDDisplayChannel::cSDDisplayChannel(cViewChannel *channelView, bool WithInfo) {
    view = channelView;
    ok = view->Init();
    if (!ok)
        esyslog("skindesigner: Error initiating displaychannel view - aborting");
}

cSDDisplayChannel::~cSDDisplayChannel() {
    view->Close();
}

void cSDDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
    if (!ok)
        return;
    view->SetChannel(Channel, Number);
}

void cSDDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
    if (!ok)
        return;
    view->SetEvents(Present, Following);
}

void cSDDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
    if (!ok)
        return;
    view->SetMessage(Type, Text);
}

void cSDDisplayChannel::Flush(void) {
    if (!ok)
        return;
    view->Flush(false);
}
