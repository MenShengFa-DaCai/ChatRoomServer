#include <QApplication>
#include <QPushButton>
#include <ui_Server.h>
#include <QIcon>

#include "server.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/icon/install.ico"));
    Server server;
    server.show();
    return QApplication::exec();
}
