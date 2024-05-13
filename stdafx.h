#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

#include <pv/static/util.h>

#ifdef Q_OS_WIN
#define WIN_DEVICE_CHECK
#endif

#ifdef Q_OS_MACX
#define MAC_DEVICE_CHECK
#endif

#ifdef Q_OS_LINUX
#define MAC_DEVICE_CHECK
#endif

struct BufferSet
{
    qint32 bufferFirst=0;
    qint32 lineHeight;
    const qint32 bufferSize=9000;
};
