#include "viewdisplaychannel.h"
#include "../config.h"

/************************************************************************************
* cViewChannel
************************************************************************************/

cViewChannel::cViewChannel(void) {
    veCustomTokens = NULL;
    ClearVariables();
    viewId = eViewType::DisplayChannel;
    viewName = strdup("displaychannel");
    numViewElements = (int)eVeDisplayChannel::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    SetViewElements();
    veMessage = NULL;
    veChannelInfo = NULL;
    veChannelGroup = NULL;
    veEpgInfo = NULL;
    veProgressBar = NULL;
    veStatusInfo = NULL;
    veScraperContent = NULL;
    veEcmInfo = NULL;
}

cViewChannel::~cViewChannel() {
}

void cViewChannel::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayChannel::background));
    viewElementNames.insert(pair<string, int>("channelgroup", (int)eVeDisplayChannel::channelgroup));
    viewElementNames.insert(pair<string, int>("channelinfo", (int)eVeDisplayChannel::channelinfo));
    viewElementNames.insert(pair<string, int>("epginfo", (int)eVeDisplayChannel::epginfo));
    viewElementNames.insert(pair<string, int>("progressbar", (int)eVeDisplayChannel::progressbar));
    viewElementNames.insert(pair<string, int>("statusinfo", (int)eVeDisplayChannel::statusinfo));
    viewElementNames.insert(pair<string, int>("audioinfo", (int)eVeDisplayChannel::audioinfo));
    viewElementNames.insert(pair<string, int>("ecminfo", (int)eVeDisplayChannel::ecminfo));
    viewElementNames.insert(pair<string, int>("screenresolution", (int)eVeDisplayChannel::screenresolution));
    viewElementNames.insert(pair<string, int>("signalquality", (int)eVeDisplayChannel::signalquality));
    viewElementNames.insert(pair<string, int>("devices", (int)eVeDisplayChannel::devices));
    viewElementNames.insert(pair<string, int>("currentweather", (int)eVeDisplayChannel::currentweather));
    viewElementNames.insert(pair<string, int>("scrapercontent", (int)eVeDisplayChannel::scrapercontent));
    viewElementNames.insert(pair<string, int>("datetime", (int)eVeDisplayChannel::datetime));
    viewElementNames.insert(pair<string, int>("time", (int)eVeDisplayChannel::time));
    viewElementNames.insert(pair<string, int>("message", (int)eVeDisplayChannel::message));
    viewElementNames.insert(pair<string, int>("customtokens", (int)eVeDisplayChannel::customtokens));
}

void cViewChannel::SetViewElementObjects(void) {
    for (int i = 0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        if (dynamic_cast<cVeMessage*>(viewElements[i])) 
        {
            veMessage = dynamic_cast<cVeMessage*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcChannelInfo*>(viewElements[i])) {

            veChannelInfo = dynamic_cast<cVeDcChannelInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcChannelGroup*>(viewElements[i])) 
        {
            veChannelGroup = dynamic_cast<cVeDcChannelGroup*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcEpgInfo*>(viewElements[i])) 
        {
            veEpgInfo = dynamic_cast<cVeDcEpgInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcProgressBar*>(viewElements[i])) 
        {
            veProgressBar = dynamic_cast<cVeDcProgressBar*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcStatusInfo*>(viewElements[i])) 
        {
            veStatusInfo = dynamic_cast<cVeDcStatusInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcScraperContent*>(viewElements[i])) 
        {
            veScraperContent = dynamic_cast<cVeDcScraperContent*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcEcmInfo*>(viewElements[i])) 
        {
            veEcmInfo = dynamic_cast<cVeDcEcmInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeCustomTokens*>(viewElements[i])) 
        {
            veCustomTokens = dynamic_cast<cVeCustomTokens*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDevices*>(viewElements[i])) 
        {
            viewElements[i]->SetDetached();
        }
    }
}

void cViewChannel::ClearVariables(void) {
    cView::ClearVariables();
    channelChange = false;
    displayChannelGroups = false;
    if (veCustomTokens)
        veCustomTokens->Reset();
}

void cViewChannel::SetChannel(const cChannel *channel, int number) {
    channelChange = true;
    bool wasChannelGroups = displayChannelGroups;
    displayChannelGroups = false;

    if (veChannelInfo) {
        veChannelInfo->Set(channel, number);
    }

    if (channel) {
        if (!channel->GroupSep()) {
            if (wasChannelGroups)
                Clear((int)eVeDisplayChannel::channelgroup);
            if (veStatusInfo)
                veStatusInfo->Set(channel);
            if (veEcmInfo)
                veEcmInfo->Set(channel);
        } else {
            displayChannelGroups = true;
            Clear((int)eVeDisplayChannel::channelinfo, true);
            Clear((int)eVeDisplayChannel::epginfo, true);
            Clear((int)eVeDisplayChannel::statusinfo, true);
            Clear((int)eVeDisplayChannel::progressbar, true);
            Clear((int)eVeDisplayChannel::screenresolution, true);
            Clear((int)eVeDisplayChannel::signalquality, true);
            Clear((int)eVeDisplayChannel::audioinfo, true);
            Clear((int)eVeDisplayChannel::ecminfo, true);
            Clear((int)eVeDisplayChannel::devices, true);
            Clear((int)eVeDisplayChannel::customtokens, true);
            if (veChannelGroup)
                veChannelGroup->Set(channel);
        }
    }
}

void cViewChannel::SetEvents(const cEvent *present, const cEvent *following) {
    Clear((int)eVeDisplayChannel::epginfo);
    Clear((int)eVeDisplayChannel::progressbar);
    Clear((int)eVeDisplayChannel::scrapercontent);
    if (veProgressBar)
        veProgressBar->Set(present);

    if (!present && !following)
        return;
    
    if (veEpgInfo)
        veEpgInfo->Set(present, following);

    if (veScraperContent)
        veScraperContent->Set(present);
}

void cViewChannel::SetMessage(eMessageType type, const char *text) {
    if (veMessage) {
        if (text)
            veMessage->Set(type, text);
        else
            veMessage->Clear();
    }
}

void cViewChannel::Flush(void) {
    if (init) {
        sdOsd.LockFlush();
        Render((int)eVeDisplayChannel::background);
        Render((int)eVeDisplayChannel::currentweather);
    }
    
    if (!displayChannelGroups) {
        //normal display
        Render((int)eVeDisplayChannel::channelinfo);
        Render((int)eVeDisplayChannel::epginfo);
        Render((int)eVeDisplayChannel::statusinfo);
        Render((int)eVeDisplayChannel::scrapercontent);
        Render((int)eVeDisplayChannel::progressbar, channelChange);
        Render((int)eVeDisplayChannel::screenresolution);
        Render((int)eVeDisplayChannel::signalquality);
        Render((int)eVeDisplayChannel::audioinfo);
        Render((int)eVeDisplayChannel::ecminfo);
        Render((int)eVeDisplayChannel::devices);
        Render((int)eVeDisplayChannel::customtokens);
        Render((int)eVeDisplayChannel::message);
    } else {
        //channelgroup display
        Render((int)eVeDisplayChannel::channelgroup);
    }
    Render((int)eVeDisplayChannel::datetime);
    Render((int)eVeDisplayChannel::time);
    channelChange = false;
    cView::Flush();
}

