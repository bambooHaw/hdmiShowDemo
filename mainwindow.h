#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QScreen>
#include <QDateTime>
#include <QTimer>
#include <QMutex>
#include <QDir>
#include "global.h"

#define DISPLAY_COORDINATE_X    0
#define DISPLAY_COORDINATE_Y    0
#define MAINWINDOW_WIDTH    1920
#define MAINWINDOW_HEIGHT   1080
#define DISPLAY_WIDTH   480
#define DISPLAY_HEIGHT  270

#define LABEL_VERSION_WIDTH 260
#define LABEL_VERSION_HEIGHT 30

typedef struct _winArgc
{
    mutable QMutex mutex;    //thread safe locker, "mutable" to make " const value()" to accessalbe
    QTimer watchDog;
    QDir *dir;
    QList<QFileInfo> *fileInfo;
    QMovie* movie;
    int x;  //display coordinate
    int y;
    int height; //whole screen
    int width;
    int h;  //display rect
    int w;
    _winArgc()
    {

        dir = NULL;
        fileInfo = NULL;
        movie = NULL;
        x = DISPLAY_COORDINATE_X;
        y = DISPLAY_COORDINATE_Y;
        width = MAINWINDOW_WIDTH;
        height = MAINWINDOW_HEIGHT;
        w = DISPLAY_WIDTH;
        h = DISPLAY_HEIGHT;
    }
    playArgc_t pa;
}winArgc_t;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(playArgc_t pa,  QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    winArgc_t wa;
    void setWinArgs(playArgc_t pa, QMainWindow *MainWindow);
    void releaseWinArgs();
    void setMovieArgc(QString filePath);
    void findNewMovieAndDisplay();
    void displayDefault(QMovie *movie);
public slots:
    void slotWatchDog();
    void slotMovieFrameHandler(const QRect &rect);
};

#endif // MAINWINDOW_H
