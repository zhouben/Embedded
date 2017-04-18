#include "mainwnd.h"
#include "work_thread.h"

MainWnd::MainWnd(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    QLineEdit  *edt = new QLineEdit;
    edt->setMaxLength(100);

    QLCDNumber *lcd = new QLCDNumber;
    QSlider *slider = new QSlider(Qt::Horizontal);

    slider->setRange(0,99);
    QObject::connect(slider, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)));
    slider->setValue(35);

	const char *info = "you should see some chinese words: 你好，世界!";
	QString infoString = info;
	QLabel	label(infoString);

    QPushButton *pStartButton = new QPushButton(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(edt);
    layout->addWidget(lcd);
    layout->addWidget(slider);
    layout->addWidget(pStartButton);

    // 创建开始按钮、进度条
    m_pProgressBar = new QProgressBar(this);

    //设置文本、进度条取值范围
    pStartButton->setText(QString("开始"));
    m_pProgressBar->setFixedHeight(25);
    m_pProgressBar->setRange(0, 100);
    m_pProgressBar->setValue(0);

    // 连接信号槽
    connect(pStartButton, SIGNAL(clicked(bool)), this, SLOT(startThread()));

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(&label);
    vlayout->addItem(layout);
    vlayout->addWidget(m_pProgressBar);

    setLayout(vlayout);
}

MainWnd::~MainWnd()
{
}
void MainWnd::startThread()
{
    WorkerThread *workerThread = new WorkerThread(this);
    connect(workerThread, SIGNAL(resultReady(int)), this, SLOT(handleResults(int)));
    // 线程结束后，自动销毁
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
}
