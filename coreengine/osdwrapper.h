#ifndef __OSDWRAPPER_H
#define __OSDWRAPPER_H

#include <vdr/osd.h>
#include <vdr/thread.h>

class cSdOsd {
private:
    cOsd *osd;
    cMutex mutex;
    bool flushLocked;
public:
    cSdOsd(void);
    virtual ~cSdOsd(void);
    void Lock(void);
    void Unlock(void);
    void LockFlush(void);
    void UnlockFlush(void);
    bool CreateOsd(int x, int y, int width, int height);
    void DeleteOsd(void);
    cPixmap *CreatePixmap(int layer, cRect &viewPort, cRect &drawPort);
    void DestroyPixmap(cPixmap *pix);
    void Flush(void);
};

#endif //__OSDWRAPPER_H