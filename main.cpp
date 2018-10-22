/* The program is used for display a group of gif on the screen which supports hdmi interface.
 *
 * All copyrights reserved by <xiansheng929@163.com> 2018.
 *
 * If you need those instances, you'd better contected with the author.
 */
#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "global.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(cmdLineParser(a));

    //emergency stuffs
    signal(SIGINT, emergensySignalHandler);
    signal(SIGQUIT, emergensySignalHandler);
    signal(SIGSEGV, emergensySignalHandler);
    signal(SIGKILL, emergensySignalHandler);

    w.show();
    return a.exec();
}


playArgc_t cmdLineParser(QApplication& a)
{
    playArgc_t pa;

    QString value;
    QCoreApplication::setApplicationVersion(SOFT_VERSION);

    QCommandLineOption cmdFile("f");
    cmdFile.setDescription("Specified the display file, if not we use a demo.");
    cmdFile.setValueName("Display file");

    QCommandLineOption cmdDir("d");
    cmdDir.setDescription("Specified the movie stuffs directory, current directory is as default;\
                            We'll display a demo if there hasn't any movie.\
                            And -f is primary than -d's args.");
    cmdDir.setValueName("Stuffs directory");
    QCommandLineOption cmdMode("m");
    cmdMode.setValueName("Display mode");
    cmdMode.setDescription("Specified the display mode, default mode is loop playback(mode: loop). You can set the mode as: singal");
    QCommandLineOption cmdDebug("D");
    cmdDebug.setDescription("Debug mode allow you get more debug info in cmdLine(0: close, 1: more, 2:much more), default is closed.");
    cmdDebug.setValueName("Debug mode");

    QCommandLineOption cmdSpeed("s");
    cmdSpeed.setDescription("Specified the display speed multiple factor(1: 1x, 2: 2x, ... 1 is the default value)");
    cmdSpeed.setValueName("Display speed");


    QCommandLineOption cmdScale("scale");
    cmdScale.setDescription("Specified the display scale both in with and height(1: 1x, 2: 2x, ... 1 is the default value.)");
    cmdScale.setValueName("Display scale");

    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(cmdDir);
    parser.addOption(cmdMode);
    parser.addOption(cmdDebug);
    parser.addOption(cmdFile);
    parser.addOption(cmdSpeed);
    parser.addOption(cmdScale);
    parser.process(a);

    //use -f as primary than -d's args
    if(parser.isSet(cmdFile))
    {
        pa.filePath = parser.value(cmdFile);
        pa.stuffDir.remove(0, pa.stuffDir.size());
    }else if(parser.isSet(cmdDir))
        pa.stuffDir = parser.value(cmdDir);

    if(parser.isSet(cmdMode))
    {
        value = parser.value(cmdMode);
        if(value.contains("loop")) pa.playMode = DISPLAY_LOOP;
        else if(value.contains("singal")) pa.playMode = DISPLAY_SINGLE;
        else
        {
            qWarning() << "Wrong argument with the Display mode. Now, we use default display mode." << endl;
        }
    }

    if(parser.isSet(cmdDebug))
    {
        value = parser.value(cmdDebug);
        if(value.contains("1")) pa.debugFlag = DEBUG_GRADE_ONE;
        else if(value.contains("2")) pa.debugFlag = DEBUG_GRADE_TWO;
        else if(value.contains("0")) pa.debugFlag = DEBUG_DISABLE;
        else
        {
            qWarning() << "Wrong argument with Debug mode. Now, wo use default debug mode." << endl;
        }
    }


    if(parser.isSet(cmdSpeed))
    {
        bool ok = false;
        pa.speed = parser.value(cmdSpeed).toInt(&ok);
        if(!ok || (pa.speed <= 0))
        {
            qWarning() << "Display speed parsed failed, now we use the default value.";
            pa.speed = DISPLAY_SPEED_nX;
        }
    }

    if(parser.isSet(cmdScale))
    {
        bool ok = false;
        pa.scale = parser.value(cmdScale).toInt(&ok);
        if(!ok || (pa.scale <= 0))
        {
            qWarning() << "Parser Scale value failed, now we use the default value.";
            pa.scale = DISPLAY_SCALE;
        }
    }

    return pa;
}


void emergensySignalHandler(int sigNum)
{
    qWarning() << "signal num: " << sigNum << endl;
    //do your emergency work immediately
    //...
    exit(0);
}
