//#include <Qt/qglobal.h>
#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_
#include <Qt/qdebug.h>
#include <Qt/qthread.h>

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    explicit WorkerThread(QObject *parent = 0)
        : QThread(parent)
    {
        qDebug() << "Worker Thread : " << QThread::currentThreadId();
    }

protected:
    virtual void run(){
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        int nValue = 0;
        while (nValue < 100)
        {
            // 休眠50毫秒
            msleep(50);
            ++nValue;

            // 准备更新
            emit resultReady(nValue);
        }
    }
signals:
    void resultReady(int value);
};
#endif
