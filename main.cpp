#include <QApplication>
#include <QPushButton>
#include <ui_Server.h>
#include <QIcon>
#include <QDir>
#include <QLibraryInfo>
#include "server.h"

int main(int argc, char* argv[]) {
    //打包要求锁定插件，防止找不到程序入口
    QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/icon/install.ico"));

    qputenv("QT_PLUGIN_PATH", QCoreApplication::applicationDirPath().toUtf8() + "/plugins");

    Server server;
    server.show();
    return QApplication::exec();
}