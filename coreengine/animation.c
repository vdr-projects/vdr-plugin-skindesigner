#include "animation.h"
#include <math.h>

/******************************************************************
* cAnimation
******************************************************************/
cAnimation::cAnimation(cScrollable *scrollable) : cThread("scroller") {
    this->scrollable = scrollable;
    this->detachable = NULL;
    this->fadable = NULL;
    this->shiftable = NULL;
    this->blinkable = NULL;
    waitOnWakeup = false;
    keepSleeping = false;
    doAnimation = true;
    modeIn = false;
    doFlush = true;
    blinkFunc = -1;
}

cAnimation::cAnimation(cDetachable *detachable, bool wait, bool animation) : cThread("detached") {
    this->scrollable = NULL;
    this->detachable = detachable;
    this->fadable = NULL;
    this->shiftable = NULL;
    this->blinkable = NULL;
    waitOnWakeup = wait;
    keepSleeping = false;
    doAnimation = animation;
    modeIn = false;
    doFlush = true;
    blinkFunc = -1;
}

cAnimation::cAnimation(cFadable *fadable, bool fadein) : cThread("fadable") {
    this->scrollable = NULL;
    this->detachable = NULL;
    this->fadable = fadable;
    this->shiftable = NULL;
    this->blinkable = NULL;
    waitOnWakeup = false;
    keepSleeping = false;
    doAnimation = true;
    modeIn = fadein;
    doFlush = true;
    blinkFunc = -1;
}

cAnimation::cAnimation(cShiftable *shiftable, cPoint &start, cPoint &end, bool shiftin, bool doFlush) : cThread("shiftable") {
    this->scrollable = NULL;
    this->detachable = NULL;
    this->fadable = NULL;
    this->shiftable = shiftable;
    this->blinkable = NULL;
    waitOnWakeup = false;
    keepSleeping = false;
    doAnimation = true;
    modeIn = shiftin;
    shiftstart = start;
    shiftend = end;
    this->doFlush = doFlush;
    blinkFunc = -1;
}

cAnimation::cAnimation(cBlinkable *blinkable, int func) : cThread("blinking") {
    this->scrollable = NULL;
    this->detachable = NULL;
    this->fadable = NULL;
    this->shiftable = NULL;
    this->blinkable = blinkable;
    waitOnWakeup = false;
    keepSleeping = false;
    doAnimation = true;
    modeIn = false;
    doFlush = true;
    blinkFunc = func;
}

cAnimation::~cAnimation(void) {
    sleepWait.Signal();
    Cancel(2);
}

void cAnimation::WakeUp(void) {
    sleepWait.Signal();
}

void cAnimation::ResetSleep(void) {
    keepSleeping = true;
    sleepWait.Signal();
}

void cAnimation::Stop(bool deletePixmaps) { 
    sleepWait.Signal();
    Cancel(2);
    if (scrollable && deletePixmaps)
        scrollable->StopScrolling();
}

void cAnimation::Action(void) {
    if (scrollable) {
        Scroll();
    } else if (detachable) {
        Detach();
    } else if (fadable) {
        Fade();
    } else if (shiftable) {
        Shift();
    } else if (blinkable) {
        Blink();
    }
}

void cAnimation::Sleep(int duration) {
    //sleep should wake up itself, so no infinit wait allowed
    if (duration <= 0)
        return;
    do {
        keepSleeping = false;
        sleepWait.Wait(duration);
    } while (keepSleeping);
}

void cAnimation::Wait(void) {
    //wait has to be waked up from outside
    sleepWait.Wait(0);
}

void cAnimation::Scroll(void) {
    int delay = scrollable->ScrollDelay();
    Sleep(delay);
    if (!Running()) return;    
    
    eOrientation orientation = scrollable->ScrollOrientation();
    int scrollTotal = 0;
    if (orientation == eOrientation::horizontal) {
        scrollTotal = scrollable->ScrollWidth();
    } else if (orientation == eOrientation::vertical) {
        scrollTotal = scrollable->ScrollHeight();
    }

    eScrollMode mode = scrollable->ScrollMode();
    bool carriageReturn = (mode == eScrollMode::carriagereturn) ? true : false;
    
    eScrollSpeed speed = scrollable->ScrollSpeed();
    int frameTime = 30;
    if (speed == eScrollSpeed::slow)
        frameTime = 50;
    else if (speed == eScrollSpeed::medium)
        frameTime = 30;
    else if (speed == eScrollSpeed::fast)
        frameTime = 15;

    if (!Running()) return;    
    
    scrollable->StartScrolling();
    
    int drawPortX = 0;
    int drawPortY = 0;
    int scrollDelta = 1;

    bool doSleep = false;
    while (Running()) {
        if (doSleep) {
            Sleep(delay);
            doSleep = false;
        }
        if (!Running()) return;
        uint64_t now = cTimeMs::Now();
        
        cPoint drawPortPoint(0,0);
        if (orientation == eOrientation::horizontal) {

            drawPortX -= scrollDelta;
            if (abs(drawPortX) > scrollTotal) {
                Sleep(delay);
                if (carriageReturn) {
                    drawPortX = 0;
                    doSleep = true;
                } else {
                    scrollDelta *= -1;
                    drawPortX -= scrollDelta;
                }
            }            
            drawPortPoint.SetX(drawPortX);

        } else if (orientation == eOrientation::vertical) {

            drawPortY -= scrollDelta;
            if (abs(drawPortY) > scrollTotal) {
                Sleep(delay);
                drawPortY = 0;
                doSleep = true;
            } 
            drawPortPoint.SetY(drawPortY);

        }
        
        if (!Running()) return;
        scrollable->SetDrawPort(drawPortPoint);

        if (!Running()) return;
        scrollable->Flush();

        if (orientation == eOrientation::horizontal && !carriageReturn && (drawPortX == 0)) {
            scrollDelta *= -1;
            doSleep = true;
        }

        int delta = cTimeMs::Now() - now;
        if (delta < frameTime)
            Sleep(frameTime - delta);
    }
}

void cAnimation::Detach(void) {
    if (waitOnWakeup) {
        Wait();
        int delay = 50 + detachable->Delay();
        Sleep(delay);
    } else {
        int delay = detachable->Delay();
        if (delay > 0)
            Sleep(delay);
    }
    if (!Running()) return;
    detachable->ParseDetached();
    if (!Running()) return;
    detachable->RenderDetached();
    if (!Running()) return;
    if (!doAnimation)
        detachable->Flush();
    if (!Running()) return;
    if (doAnimation) {
        detachable->StartAnimation();        
    }
}

void cAnimation::Fade(void) {
    int fadetime = fadable->FadeTime();
    int frametime = 1000 / FPS;
    int step = 100.0f / ((double)fadetime / (double)frametime);
    uint64_t start = cTimeMs::Now();
    int transparency = 0;
    if (modeIn) {
        transparency = 100 - step;
    } else {
        transparency = step;
    }
    //wait configured delay if not already done by detacher
    if (!fadable->Detached()) {
        int delay = fadable->Delay();
        if (delay > 0)
            Sleep(delay);
    }
    while (Running() || !modeIn) {
        uint64_t now = cTimeMs::Now();
        if (Running() || !modeIn)
            fadable->SetTransparency(transparency, !modeIn);
        if (Running() || !modeIn)
            fadable->Flush();
        int delta = cTimeMs::Now() - now;
        if ((Running()  || !modeIn) && (delta < frametime)) {
            Sleep(frametime - delta);
        }
        if ((int)(now - start) > fadetime) {
            if ((Running() && modeIn) && transparency > 0) {
                fadable->SetTransparency(0);
                fadable->Flush();
            } else if (!modeIn && transparency < 100) {
                fadable->SetTransparency(100, true);
                fadable->Flush();                
            } 
            break;
        }
        if (modeIn) {
            transparency -= step;
            if (transparency < 0)
                transparency = 0;
        } else {
            transparency += step;
            if (transparency > 100)
                transparency = 100;            
        }
    }
}

void cAnimation::Shift(void) {
    int shifttime = shiftable->ShiftTime();
    eShiftMode mode = (eShiftMode)shiftable->ShiftMode();
    //in shiftmode slowedDown shifting is done starting with slowratio % faster
    //at start. Then speed reduces linear to (100 - slowratio)% at end
    //for me 60 is a nice value :-) 
    int slowRatio = 60;

    int frametime = 1000 / FPS;
    int steps = (double)shifttime / (double)frametime;
    if (steps < 2)
        return;
    int stepXLinear = 0;
    int stepYLinear = 0;
    if (shiftstart.X() == shiftend.X()) {
        stepYLinear = (shiftend.Y() - shiftstart.Y()) / steps;
    } else if (shiftstart.Y() == shiftend.Y()) {
        stepXLinear = (shiftend.X() - shiftstart.X()) / steps;
    } else {
        stepXLinear = (shiftend.X() - shiftstart.X()) / steps;
        stepYLinear = (shiftend.Y() - shiftstart.Y()) / steps;        
    }
    int stepX = stepXLinear;
    int stepY = stepYLinear;

    cPoint pos;
    if (modeIn)
        pos = shiftstart;
    else
        pos = shiftend;

    //wait configured delay if not already done by detacher
    if (!shiftable->Detached()) {
        int delay = shiftable->Delay();
        if (delay > 0)
            Sleep(delay);
    }

    shiftable->SetStartShifting();
    uint64_t start = cTimeMs::Now();
    while (Running() || !modeIn) {
        uint64_t now = cTimeMs::Now();
        if (Running() || !modeIn)
            shiftable->SetPosition(pos, shiftend);
        if ((Running() || !modeIn) && doFlush)
            shiftable->Flush();
        int delta = cTimeMs::Now() - now;
        if ((Running()  || !modeIn) && (delta < frametime)) {
            cCondWait::SleepMs(frametime - delta);
        }
        if ((int)(now - start) > shifttime) {
            if ((Running() && modeIn) && pos != shiftend) {
                shiftable->SetPosition(shiftend, shiftend);
                shiftable->Flush();
            }
            break;
        }
        if (mode == eShiftMode::slowedDown) {
            double t = (double)(now - start) / (double)shifttime;
            double factor = 1.0f + (double)slowRatio / 100.0f - 2.0f * ((double)slowRatio / 100.0f) * t;
            stepX = stepXLinear * factor;
            stepY = stepYLinear * factor;
        }
        if (modeIn) {
            pos.Set(pos.X() + stepX, pos.Y() + stepY);
        } else {
            pos.Set(pos.X() - stepX, pos.Y() - stepY);
        }
    }
    shiftable->SetEndShifting();
}

void cAnimation::Blink(void) {
    int freq = blinkable->BlinkFreq(blinkFunc);
    bool blinkOn = false;
    while (Running()) {
        Sleep(freq);
        if (Running())
            blinkable->DoBlink(blinkFunc, blinkOn);
        if (Running())
            blinkable->Flush();  
        blinkOn = !blinkOn;
    }
}
