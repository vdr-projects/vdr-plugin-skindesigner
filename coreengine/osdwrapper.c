#include "osdwrapper.h"

cSdOsd::cSdOsd(void) {
    osd = NULL;
    flushLocked = false;
    animsRunning = 0;
    animsFlushed = 0;
}

cSdOsd::~cSdOsd(void) {
    DeleteOsd();
}

void cSdOsd::Lock(void) {
    mutex.Lock();
}

void cSdOsd::Unlock(void) {
    mutex.Unlock();
}

void cSdOsd::LockFlush(void) {
    Lock();
    flushLocked = true;
    Unlock();
}

void cSdOsd::UnlockFlush(void) {
    Lock();
    flushLocked = false;
    Unlock();
}

bool cSdOsd::CreateOsd(int x, int y, int width, int height) {
    cOsd *newOsd = cOsdProvider::NewOsd(cOsd::OsdLeft() + x, cOsd::OsdTop() + y);
    if (newOsd) {
        tArea Area = { 0, 0, width - 1, height - 1,  32 };
        if (newOsd->SetAreas(&Area, 1) == oeOk) {
            Lock();
            osd = newOsd;
            Unlock();
            return true;
        }
    }
    return false;
}

void cSdOsd::DeleteOsd(void) {
    Lock();
    delete osd;
    osd = NULL;
    Unlock();
    animsRunningMutex.Lock();
    animsRunning = 0;
    animsFlushed = 0;
    animsRunningMutex.Unlock();
}

cPixmap *cSdOsd::CreatePixmap(int layer, cRect &viewPort, cRect &drawPort) {
    if (osd) {
        return osd->CreatePixmap(layer, viewPort, drawPort);
    }
    return NULL;
}

void cSdOsd::DestroyPixmap(cPixmap *pix) {
    if (osd) {
        osd->DestroyPixmap(pix);
    }
}

void cSdOsd::AddAnimation(void) {
    animsRunningMutex.Lock();
    animsRunning++;
    animsRunningMutex.Unlock();
}

void cSdOsd::RemoveAnimation(void) {
    animsRunningMutex.Lock();
    animsRunning--;
    animsRunningMutex.Unlock();
}

void cSdOsd::AnimatedFlush(void) {
    if (osd && !flushLocked) {
        animsRunningMutex.Lock();
        if (animsFlushed + 1 >= animsRunning) {
            animsFlushed = 0;
            osd->Flush();
        } else {
            animsFlushed++;
        }
        animsRunningMutex.Unlock();
    }
}

void cSdOsd::Flush(void) {
    if (osd && !flushLocked) {
        osd->Flush();
    }
}
