#include <QtGui/QApplication>
#include "mainwindow.hpp"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    QApplication a(argc, argv);
    MainWindow w;
//    w.loadDefaultMusic("D:/sdhumming/Gao/midi/music.list");
    w.show();
    w.setDefaultMusicList();
    return a.exec();
}
