#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget {
Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server() override;

private:
    Ui::Server *ui;
    //服务器
    QTcpServer *tcpServer=nullptr;
    //数据库
    QSqlDatabase db;
    //链接数组
    QList<QTcpSocket*> clients;
    //登陆处理函数
    bool userLogin(const QString& user, const QString& passport);
    //注册处理函数
    bool userRegister(QString& user,QString& passport);
    //把聊天记录存到数据库
    void reco(const QString& time, const QString& sender,const QString& msg);
    // 存储在线用户
    QList<QString> onlineUsers;
private slots:
    // 处理新连接
    void handleNewConnection();
    // 读取客户端数据
    void readData();
    // 处理客户端断开连接,注册或者登陆完成后断开连接，然后链接新聊天链接
    void clientDisconnected();
};


#endif //SERVER_H
