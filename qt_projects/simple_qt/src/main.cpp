#include <QApplication>
#include <QTextCodec>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLCDNumber>
#include <QtGui>

#include "mainwnd.h"
int main(int argc,char *argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QApplication	app(argc,argv);

    //QWidget *window = new QWidget();
    MainWnd *window = new MainWnd();

    window->setWindowTitle("转发器");
    window->show();

	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	//char *info = "you should see some chinese words: 你好，世界, 大好人r周犇!";
	//QString infoString = info;
	//QLabel	label(infoString);
	//label.show();
	app.exec();
}
