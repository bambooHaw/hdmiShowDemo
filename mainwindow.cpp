#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(playArgc_t pa, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //0. monitor the app
    wa.mutex.tryLock();
    connect(&(wa.watchDog), SIGNAL(timeout()), this, SLOT(slotWatchDog()), Qt::QueuedConnection);
    wa.watchDog.start(1000);    //start the app monitor
    wa.mutex.unlock();

    //1. set MainWindow size first
    setWinArgs(pa, this);
    //2. find movie and display
    findNewMovieAndDisplay();
}

MainWindow::~MainWindow()
{
    releaseWinArgs();
    delete ui;
}

void MainWindow::setWinArgs(playArgc_t pa, QMainWindow *MainWindow)
{
    //get resolution of the win
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect rec = screen->availableGeometry();
    if(rec.isValid())
    {
        wa.mutex.tryLock();
        wa.width = rec.width();
        wa.height = rec.height();
        //get playArgc, set user define display mode
        memcpy(&(wa.pa), &pa, sizeof(playArgc_t));

        //alloc the dir
        wa.dir = new QDir(wa.pa.stuffDir);
        QStringList filter;
        filter<<"*.gif";
        wa.dir->setNameFilters(filter);
        wa.fileInfo = new QList<QFileInfo>(wa.dir->entryInfoList(filter));
        wa.movie = new QMovie;
        connect(wa.movie, SIGNAL(updated(QRect)), MainWindow, SLOT(slotMovieFrameHandler(QRect)));
        wa.mutex.unlock();
    }
    //set main display rect, full screen size
    MainWindow->setFixedSize(wa.width, wa.height);

    if(DEBUG_GRADE_ONE <= wa.pa.debugFlag)
    {
        qDebug() << "width:" << wa.width << endl;
        qDebug() << "height:" << wa.height << endl;
    }
    ui->label_version->setText(QString(SOFT_VERSION));
}

void MainWindow::releaseWinArgs()
{
    delete wa.movie;
    delete wa.fileInfo;
    delete wa.dir;
}

void MainWindow::setMovieArgc(QString filePath)
{
    wa.mutex.tryLock();
    //set data for movie
    wa.movie->setFileName(filePath);
    //get frame size
    QImage m(filePath);
    if(!m.isNull())
    {
        wa.w = m.width();
        wa.h = m.height();
        //alloc space, set scale factor
        QSize size(wa.w * wa.pa.scale, wa.h * wa.pa.scale);
        wa.movie->setScaledSize(size);
        wa.movie->setSpeed(wa.pa.speed * 100);   //DISPLAY_SPEED_nX speed

        if(DEBUG_GRADE_ONE <= wa.pa.debugFlag)
        {
            qDebug() << "frameCount: " << wa.movie->frameCount() << endl;
            qDebug() << "w:" << wa.w;
            qDebug() << "h:" << wa.h;
        }
        //set sub display rect
        ui->label_display->setGeometry(wa.x, wa.y, wa.w*wa.pa.scale, wa.h*wa.pa.scale);
        ui->label_version->setGeometry(0, wa.y+wa.h*wa.pa.scale, LABEL_VERSION_WIDTH, LABEL_VERSION_HEIGHT);
    }
    wa.mutex.unlock();
}

void MainWindow::findNewMovieAndDisplay()
{
    static char i = 0, j = 0;

    qDebug("%s(+ %d): i:%d, count(): %d\n", __func__, __LINE__, i, wa.fileInfo->count());
    if(wa.fileInfo->count() < 1)
    {
        if(j!=0 && DISPLAY_SINGLE == wa.pa.playMode)
            kill(getpid(), SIGKILL);
        setMovieArgc(wa.pa.filePath); //use specified file if the dir is empty
        //3. default display
        displayDefault(wa.movie);
        j++;
    }
    else
    {
        if(i < wa.fileInfo->count())
        {
            setMovieArgc(wa.fileInfo->at(i++).filePath());
            //3. default display
            displayDefault(wa.movie);
        }else
        {
            //if need loop display
            if(DISPLAY_LOOP == wa.pa.playMode)
            {
                i = 0;
                //new display loop
                findNewMovieAndDisplay();
            }
            else
            {
                //display end
                kill(getpid(), SIGKILL);
            }
        }
    }
}

void MainWindow::displayDefault(QMovie* movie)
{
    if(!movie->isValid())
    {
        qWarning("Invalid movie.\n");
    }else
    {
        ui->label_display->setMovie(movie);
        movie->start();
        //movie->setPaused(ture);//pause

        if(DEBUG_GRADE_ONE <= wa.pa.debugFlag)
        {
            qDebug() << "speed: " << wa.pa.speed;
            qDebug() << "scale: " << wa.pa.scale;
        }
    }
}

void MainWindow::slotWatchDog()
{
    QString qStr = QString(SOFT_VERSION) + QString("  ");
    qStr += QDateTime::currentDateTime().toString("yyyy/MM/dd-HH:mm:ss");
    ui->label_version->setText(qStr);
}

void MainWindow::slotMovieFrameHandler(const QRect &rect)
{
    //qDebug() << "currentFrame:" << wa.movie->currentFrameNumber();
    wa.mutex.tryLock();
    if(wa.movie->frameCount() == (wa.movie->currentFrameNumber() + 1))
    {
        wa.movie->stop();
        //display next new movie
        findNewMovieAndDisplay();
    }
    wa.mutex.unlock();

    if(DEBUG_GRADE_ONE <= wa.pa.debugFlag)
    {
        qDebug("framecount: %d, cFramecount: %d\n", wa.movie->frameCount(), wa.movie->currentFrameNumber());
    }

    if(DEBUG_GRADE_TWO <= wa.pa.debugFlag)
    {
        qDebug() << "rect.x() :" << rect.x();
        qDebug() << "rect.y() :" << rect.y();
    }
}
