#include "../config.h"
#include "view.h"

// --- cView -------------------------------------------------------------

cView::cView(void) {
    globals = NULL;
    viewName = NULL;
    attribs = new cViewAttribs((int)eViewAttribs::count);
    numViewElements = 0;
    viewElements = NULL;
    viewElementsHorizontal = NULL;
    fader = NULL;
    shifter = NULL;
    shifting = false;
    currentTvFrame = NULL;
    newTvFrame = NULL;
    menuInit = false;
}

cView::~cView() {
    for (int i=0; i< numViewElements; i++)
        delete viewElements[i];
    delete[] viewElements;
    if (viewElementsHorizontal) {
        for (int i=0; i< numViewElements; i++)
            delete viewElementsHorizontal[i];
        delete[] viewElementsHorizontal;
    }
    delete attribs;
    free(viewName);
    delete fader;
    delete shifter;
    shifting = false;
    sdOsd.DeleteOsd();
}

/*******************************************************************
* Public Functions
*******************************************************************/
bool cView::ReadFromXML(void) {
    const char *xmlFile;
    switch (viewId) {
        case eViewType::DisplayChannel:
            xmlFile = "displaychannel.xml";
            break;
        case eViewType::DisplayMenu:
            xmlFile = "displaymenu.xml";
            break;
        case eViewType::DisplayMessage:
            xmlFile = "displaymessage.xml";
            break;
        case eViewType::DisplayReplay:
            xmlFile = "displayreplay.xml";
            break;
        case eViewType::DisplayVolume:
            xmlFile = "displayvolume.xml";
            break;
        case eViewType::DisplayTracks:
            xmlFile = "displayaudiotracks.xml";
            break;
        default:
            return false;
    }
    cXmlParser parser;
    parser.SetOsd(&sdOsd);
    if (!parser.ReadView(this, xmlFile)) {
        return false;
    }
    if (!parser.ParseView()) {
        return false;
    }

    //read additional plugin menu templates
    bool ok = true;
    if (viewId == eViewType::DisplayMenu) {
        plgManager->InitPluginMenuIterator();
        map <int,skindesignerapi::sPlugMenu> *plugMenus = NULL;
        string plugName = "";
        int plugId = -1;
        while ( plugMenus = plgManager->GetPluginMenus(plugName, plugId) ) {
            for (map <int,skindesignerapi::sPlugMenu>::iterator it = plugMenus->begin(); it != plugMenus->end(); it++) {
                int templateNumber = it->first;
                int menuType = it->second.type;
                cString templateName = cString::sprintf("plug-%s-%s", plugName.c_str(), it->second.tplname.c_str());
                if (parser.ReadPluginView(*templateName)) {
                    ok = parser.ParsePluginView(plugName, plugId, templateNumber, menuType);
                } else {
                    dsyslog("skindesigner: template %s for plugin %s not available", *templateName, plugName.c_str());
                }
            }
        }
    }
    return ok;
}

void cView::SetGlobals(cGlobals *globals) {
    this->globals = globals;
    attribs->SetGlobals(globals);
    for (int i=0; i < numViewElements; ++i) {
        if (viewElements[i]) {
            viewElements[i]->SetGlobals(globals);
        }
        if (viewElementsHorizontal && viewElementsHorizontal[i]) {
            viewElementsHorizontal[i]->SetGlobals(globals);
        }
    }
}

void cView::SetContainer(int x, int y, int width, int height) {
    container.SetX(x);
    container.SetY(y);
    container.SetWidth(width);
    container.SetHeight(height);
}

void cView::SetAttributes(vector<stringpair> &attributes) { 
    attribs->Set(attributes); 
}

void cView::AddViewElement(const char *sViewElement, cViewElement *viewElement) {
    int id = ViewElementId(sViewElement);
    if (id == ATTR_UNKNOWN)
        return;
    viewElement->SetId(id);    
    viewElement->SetTokenContainer();
    eOrientation orientation = viewElement->Orientation();
    if (viewElementsHorizontal && orientation == eOrientation::horizontal) {
        viewElementsHorizontal[id] = viewElement;
    } else {
        viewElements[id] = viewElement;
    }
}

bool cView::ValidViewElement(const char *viewElement) {
    if (ViewElementId(viewElement) != ATTR_UNKNOWN)
        return true;
    return false;
}

bool cView::ValidViewList(const char *viewList) {
    if (!strcmp(viewList, "menuitems"))
        return true;
    return false;
}

void cView::PreCache(void) {
    bool rootView = (container.Width() == 0) ? true : false;
    if (rootView) {
        SetContainer(0, 0, cOsd::OsdWidth(), cOsd::OsdHeight());
        attribs->SetContainer(0, 0, cOsd::OsdWidth(), cOsd::OsdHeight());
        attribs->Cache();
        rootView = true;
    }
    //set frame for scaling tv picture
    tvFrame = attribs->TvFrame();
    //cache viewelements
    int contX = rootView ? 0 : ((attribs->X() > -1) ? attribs->X() : 0);
    int contY = rootView ? 0 : ((attribs->Y() > -1) ? attribs->Y() : 0);
    for (int i=0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        viewElements[i]->SetContainer(contX, contY, attribs->Width(), attribs->Height());
        viewElements[i]->Cache();
    }
    if (viewElementsHorizontal) {   
        for (int i=0; i < numViewElements; i++) {
            if (!viewElementsHorizontal[i])
                continue;
            viewElementsHorizontal[i]->SetContainer(contX, contY, attribs->Width(), attribs->Height());
            viewElementsHorizontal[i]->Cache();
        }
    }

    //cleanup viewelements
    for (int i=0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        if (!viewElements[i]->Execute()) {
            delete viewElements[i];
            viewElements[i] = NULL;
        }
    }
    if (viewElementsHorizontal) {   
        for (int i=0; i < numViewElements; i++) {
            if (!viewElementsHorizontal[i])
                continue;
            if (!viewElementsHorizontal[i]->Execute()) {
                delete viewElementsHorizontal[i];
                viewElementsHorizontal[i] = NULL;
            }
        }
    }
    //set viewelement objects for each view
    SetViewElementObjects();
}

bool cView::Init(void) {
    int osdX = attribs->X();
    int osdY = attribs->Y();
    int osdWidth  = attribs->Width();
    int osdHeight = attribs->Height();
    return sdOsd.CreateOsd(osdX, osdY, osdWidth, osdHeight);
}

void cView::Clear(int ve, bool forceClearBackground) {
    if (!viewElements[ve])
        return;
    viewElements[ve]->Clear(forceClearBackground);
}

void cView::Render(int ve, bool force) {
    if (!viewElements[ve])
        return;
    if (viewElements[ve]->Parse(force))
        viewElements[ve]->Render();
}

void cView::Close(void) {
    delete fader;
    fader = NULL;
    delete shifter;
    shifter = NULL;
    if (initFinished && ShiftTime() > 0) {
        cRect shiftbox = CoveredArea();
        cPoint ref = cPoint(shiftbox.X(), shiftbox.Y());
        cPoint end = ShiftStart(shiftbox);
        shifter = new cAnimation((cShiftable*)this, end, ref, false);
        shifter->Shift();
        delete shifter;
        shifter = NULL;
    } else if (initFinished && FadeTime() > 0) {
        fader = new cAnimation((cFadable*)this, false);
        fader->Fade();
        delete fader;
        fader = NULL;
    }
    UnScaleTv();
    ClearVariables();
    for (int i=0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        viewElements[i]->Close();
    }
    sdOsd.Flush();
    sdOsd.DeleteOsd();
}

int cView::FadeTime(void) {
    return attribs->FadeTime();
}

void cView::SetTransparency(int transparency, bool force) {
    for (int i = 0; i < numViewElements; i++) {
        if (viewElements[i] && (!viewElements[i]->Fading() || force)) {
            viewElements[i]->SetTransparency(transparency);
        }
    }
}

int cView::ShiftTime(void) {
    return attribs->ShiftTime();
}

int cView::ShiftMode(void) {
    return attribs->ShiftMode();
}

void cView::SetPosition(cPoint &position, cPoint &reference, bool force) {
    for (int i = 0; i < numViewElements; i++) {
        if (viewElements[i] && (!viewElements[i]->Shifting() || force)) {
            viewElements[i]->SetPosition(position, reference);
        }
    }
}

void cView::RegisterAnimation(void) {
    sdOsd.AddAnimation();
}

void cView::UnregisterAnimation(void) {
    sdOsd.RemoveAnimation();
}

void cView::Flush(bool animFlush) {
    if (init) {
        init = false;
        StartAnimation();
        menuInit = true;
        //LockFlush was set at startup of view to avoid display
        //of not positioned pixmaps for shifting and fading 
        sdOsd.UnlockFlush();
    }
    if (menuInit) {
        ScaleTv();
        WakeViewElements();
        menuInit = false;
    }
    if (animFlush)
        sdOsd.AnimatedFlush();
    else
        sdOsd.Flush();
}

void cView::Debug(void) {
    esyslog("skindesigner: ---> view %s", viewName);
    attribs->Debug();
    for (int i=0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        esyslog("skindesigner: debugging ve %d", i);
        viewElements[i]->Debug(true);
    }
}

/*******************************************************************
* Protected Functions
*******************************************************************/
void cView::ClearVariables(void) { 
    init = true;
    initFinished = false;
    newTvFrame = NULL;
    currentTvFrame = NULL;
    menuInit = false;
}

int cView::ViewElementId(const char *name) {
    map<string, int>::iterator hit = viewElementNames.find(name);
    if (hit != viewElementNames.end())
        return (int)hit->second;
    return ATTR_UNKNOWN;
}

void cView::StartAnimation(void) {
    if (ShiftTime() > 0) {
        cRect shiftbox = CoveredArea();
        cPoint ref = cPoint(shiftbox.X(), shiftbox.Y());
        cPoint start = ShiftStart(shiftbox);
        SetPosition(start, ref);
        shifter = new cAnimation((cShiftable*)this, start, ref, true);
        shifter->Start();
    } else if (FadeTime() > 0) {
        if (fader)
            return;
        SetTransparency(100);
        sdOsd.Flush();
        fader = new cAnimation((cFadable*)this, true);
        fader->Start();
    }
    initFinished = true;
}

void cView::WakeViewElements(void) {
    for (int i = 0; i < numViewElements; i++) {
        if (viewElements[i]) {
            viewElements[i]->WakeUp();
        }
    }
    if (!viewElementsHorizontal)
        return;
    for (int i = 0; i < numViewElements; i++) {
        if (viewElementsHorizontal[i]) {
            viewElementsHorizontal[i]->WakeUp();
        }
    }
}

cPoint cView::ShiftStart(cRect &shiftbox) {
    eShiftType type = (eShiftType)attribs->ShiftType();
    cPoint start;
    if (type == eShiftType::none) {
        start = attribs->ShiftStartpoint();
    } else if (type == eShiftType::left) {
        start.SetX(-shiftbox.Width());
        start.SetY(shiftbox.Y());
    } else if (type == eShiftType::right) {
        start.SetX(attribs->Width());
        start.SetY(shiftbox.Y());        
    } else if (type == eShiftType::top) {
        start.SetX(shiftbox.X());
        start.SetY(-shiftbox.Height());        
    } else if (type == eShiftType::bottom) {
        start.SetX(shiftbox.X());
        start.SetY(attribs->Height());
    }
    return start;
}

cRect cView::CoveredArea(void) {
    cRect unionArea;
    for (int i = 0; i < numViewElements; i++) {
        if (viewElements[i] && !viewElements[i]->Shifting()) {
            unionArea.Combine(viewElements[i]->CoveredArea());
        }
    }
    return unionArea;
}

void cView::ScaleTv(void) {
    bool scale = false;
    if (newTvFrame) {
        if (currentTvFrame) {
            if (*newTvFrame != *currentTvFrame) {
                scale = true;                
            }
        } else {
            scale = true;
        }
        currentTvFrame = newTvFrame;
    } else {
        if (tvFrame != cRect::Null) {
            scale = true;
            currentTvFrame = &tvFrame;
        }
    }
    if (currentTvFrame && scale) {
        DoScaleTv(currentTvFrame);
    }
}

void cView::UnScaleTv(void) {
    if (currentTvFrame) {
        DoScaleTv(&cRect::Null);
        currentTvFrame = NULL;
    }
}

void cView::DoScaleTv(const cRect *frame) {
    if (*frame == cRect::Null) {
        cDevice::PrimaryDevice()->ScaleVideo(cRect::Null);
    } else {
        cRect scalingWindow = cDevice::PrimaryDevice()->CanScaleVideo(*frame);
        if (scalingWindow != cRect::Null) {
            cDevice::PrimaryDevice()->ScaleVideo(scalingWindow);
        }
    }
}
