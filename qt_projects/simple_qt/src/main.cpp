#include <QApplication>
#include <QTextCodec>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLCDNumber>
#include <QtGui>
int main(int argc,char *argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QApplication	app(argc,argv);

    QWidget *window = new QWidget;

    window->setWindowTitle("转发器");
    QLCDNumber *lcd = new QLCDNumber;
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0,99);
    QObject::connect(slider, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)));
    slider->setValue(35);

	const char *info = "you should see some chinese words: 你好，世界!";
	QString infoString = info;
	QLabel	label(infoString);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(lcd);
    layout->addWidget(slider);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(&label);
    vlayout->addItem(layout);

    window->setLayout(vlayout);
    window->show();

	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	//char *info = "you should see some chinese words: 你好，世界, 大好人r周犇!";
	//QString infoString = info;
	//QLabel	label(infoString);
	//label.show();
	app.exec();
}
