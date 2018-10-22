#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDebug>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


#define SOFT_VERSION "V0.1.0"
#define RESOURCE_GIF_DEMO0 ":/gif/fastSmileDemo.gif"
#define DIR_MOVIES_STORE   "/opt/gif"

#define DISPLAY_SCALE   1
#define DISPLAY_SPEED_nX    1     //1x

enum _PLAY_MODE
{
    DISPLAY_LOOP = 0,
    DISPLAY_SINGLE,
};

enum _DEBUG_FLAG
{
    DEBUG_DISABLE = 0,
    DEBUG_GRADE_ONE,
    DEBUG_GRADE_TWO,
};

typedef struct _playArgc
{
    enum _PLAY_MODE playMode;
    enum _DEBUG_FLAG debugFlag;
    int speed;  //display speed
    int scale; //display scale factor
    QString stuffDir;
    QString filePath;
    _playArgc()
    {
        playMode = DISPLAY_LOOP;
        debugFlag = DEBUG_DISABLE;
        speed = DISPLAY_SPEED_nX;
        scale = DISPLAY_SCALE;
        stuffDir = DIR_MOVIES_STORE;
        filePath = QString(RESOURCE_GIF_DEMO0);
    }
}playArgc_t;


extern  playArgc_t cmdLineParser(QApplication& a);
extern void emergensySignalHandler(int sigNum);
#endif // GLOBAL_H
