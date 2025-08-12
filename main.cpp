#include <QApplication>
#include <QPushButton>
#include <ui_Server.h>

#include "server.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    Server server;
    server.show();
    return QApplication::exec();
}
