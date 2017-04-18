#ifndef _MAINWND_H_
#define _MAINWND_H_
#include <QTextCodec>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLCDNumber>
#include <QWidget>
#include <QtGui>
#include <QProgressBar>

class MainWnd : public QWidget
{
    Q_OBJECT;
public:
    MainWnd(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~MainWnd();

private slots:
    // 更新进度
    void handleResults(int value)
    {
        m_pProgressBar->setValue(value);
    }

    // 开启线程
    void startThread();

private:
    QProgressBar *m_pProgressBar;
};

#endif
