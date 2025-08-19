#include "server.h"
#include "ui_Server.h"
#include <QCoreApplication>  // 添加头文件
#include <QDir>              // 添加头文件

Server::Server(QWidget *parent) :
    QWidget(parent), ui(new Ui::Server) {
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/icon/install.ico"));
    //拉起数据库
    QString dbPath = QCoreApplication::applicationDirPath() + "/Users.db";
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if(db.open()) {
        ui->textBrowser->append("数据库已经拉起");
    }
    //创建表
    // 检查USERS表是否已存在
    QSqlQuery checkQuery;
    if (checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='USERS';")) {
        if (checkQuery.next()) {
            // 表已存在，直接打开
            ui->textBrowser->append("USER表已存在");
        } else {
            // 表不存在，创建新表
            QString createSql = "CREATE TABLE USERS(user TEXT PRIMARY KEY, passport TEXT);";
            QSqlQuery createQuery;
            if (createQuery.exec(createSql)) {
                ui->textBrowser->append("创建表完成");
            } else {
                ui->textBrowser->append("创建表失败: " + createQuery.lastError().text());
            }
        }
    } else {
        ui->textBrowser->append("检查表格存在性失败: " + checkQuery.lastError().text());
    }
    checkQuery.clear();
    //创建聊天记录表
    //Recording表
    if (checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='RECO';")) {
        if (checkQuery.next()) {
            // 表已存在，直接打开
            ui->textBrowser->append("RECO表存在");
        }else {
            //表不存在，创建新表
            QString createSql = "CREATE TABLE RECO(time TEXT, user TEXT,message TEXT);";
            QSqlQuery createQuery;
            if (createQuery.exec(createSql)) {
                ui->textBrowser->append("RECO创建表完成");
            } else {
                ui->textBrowser->append("RECO创建表失败: " + createQuery.lastError().text());
            }
        }
    }else {
        ui->textBrowser->append("RECO检查表格存在性失败: " + checkQuery.lastError().text());
    }
    //创建服务器对象，设置监听
    tcpServer = new QTcpServer(this);
    if (tcpServer->listen(QHostAddress::AnyIPv4, 11451)) {
        ui->textBrowser->append("服务器已打开.");
    }else {
        ui->textBrowser->append("服务器打开失败！");
        return;
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}

Server::~Server() {
    if (tcpServer!=nullptr&&tcpServer->isListening()) {
        tcpServer->close();
    }
    delete tcpServer;
    delete ui;
}

//登陆处理函数
bool Server::userLogin(const QString& user, const QString& passport) {
    ui->textBrowser->append("正在试图登陆"+user+"密码："+passport);
    //查数据库看有无该用户
    QSqlQuery checkQuery;
    QString sql="SELECT * FROM USERS WHERE user=?;";
    //预处理语句进入执行器
    checkQuery.prepare(sql);
    //填充数据
    checkQuery.bindValue(0, user);
    //执行
    if (checkQuery.exec()) {
        ui->textBrowser->append("正在查找该用户……");
        if (checkQuery.next()) {
            // 找到用户，验证密码
            ui->textBrowser->append("找到该用户，验证密码中……");
            const QString dbPassport = checkQuery.value(1).toString(); // 获取第2列
            if (dbPassport == passport) {
                // 检查用户是否在线
                if (onlineUsers.contains(user)) {
                    ui->textBrowser->append("用户已在线，拒绝登陆");
                    return false;
                }
                ui->textBrowser->append("登陆成功");
                // 添加用户到在线列表
                onlineUsers.insert(user);
                return true;
            }
            ui->textBrowser->append("密码错误");
            return false;
        }
        // 用户不存在
        ui->textBrowser->append("用户不存在");
        return false;
    }
    ui->textBrowser->append("异常，语句执行失败 在server.cpp 109行左右");
    return false;
}

bool Server::userRegister(QString& user, QString& passport) {
    //先查数据库看看有无该用户
    ui->textBrowser->append("正在试图注册"+user+"密码："+passport);
    //查数据库看有无该用户
    QSqlQuery checkQuery;
    QString sql="SELECT * FROM USERS WHERE user=?;";
    //预处理语句进入执行器
    checkQuery.prepare(sql);
    //填充数据
    checkQuery.bindValue(0, user);
    //执行
    if (checkQuery.exec()) {
        ui->textBrowser->append("正在查找该用户……");
        if (checkQuery.next()) {
            //找到用户，拒绝注册
            ui->textBrowser->append("该用户存在，注册失败！");
            return false;
        }
        // 用户不存在，创建新用户
        QSqlQuery insertQuery;
        QString insertSql = "INSERT INTO USERS VALUES(?,?);";
        //处理预处理SQL指令
        insertQuery.prepare(insertSql);
        insertQuery.bindValue(0, user);
        insertQuery.bindValue(1, passport);
        //执行命令
        if (insertQuery.exec()) {
            ui->textBrowser->append(user+"注册成功！");
            return true;
        }
        ui->textBrowser->append("注册失败，未知错误！");
        return false;
    }
    ui->textBrowser->append("未进入查询指令！");
    return false;
}
//聊天记录存储函数
void Server::reco(const QString& time, const QString& sender, const QString& msg) {
    QSqlQuery checkQuery;
    checkQuery.prepare("INSERT INTO RECO VALUES(?,?,?);");
    checkQuery.bindValue(0, time);
    checkQuery.bindValue(1, sender);
    checkQuery.bindValue(2, msg);
    if (checkQuery.exec()) {
        ui->textBrowser->append("聊天记录已经入库");
    }else {
        ui->textBrowser->append("聊天记录添加数据库指令异常");
    }
}
// 处理新连接
void Server::handleNewConnection() {
    //有待处理的链接就把新的链接放到QList里
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
        clients.append(clientSocket);
        //放进去后，链接读到内容的槽函数
        connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readData()));
        //断开连接的槽函数
        connect(clientSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        ui->textBrowser->append("新客户端连接: " + clientSocket->peerAddress().toString()+":"+QString::number(clientSocket->peerPort()));
    }
}

// 读取客户端数据
void Server::readData() {
    auto *clientSocket = qobject_cast<QTcpSocket*>(sender());
    // 获取客户端信息
    QHostAddress clientAddress = clientSocket->peerAddress();
    quint16 clientPort = clientSocket->peerPort();
    ui->textBrowser->append("客户端："+clientAddress.toString()+":"+QString::number(clientPort)+"已连接");
    //收客户端信息
    QTextStream in(clientSocket);
    QString str=in.readAll();
    ui->textBrowser->append("该客户端指令："+str);
    // 创建输出流用于回传数据
    QTextStream out(clientSocket);
    //分割客户端的信息，然后执行操作
    QStringList order=str.split(" ");
    if (order.size()>=3) {
        if (order[0]=="LOGIN") {
            // 在此处理登陆逻辑
            bool ok=userLogin(order[1], order[2]);
            // 把登陆成功或者失败的信息发回客户端
            out << (ok ? "LOGIN SUCCESS" : "LOGIN FAILED");
            out.flush();

            if (ok) {
                // 登录成功则记录用户映射
                clientUserMap[clientSocket] = order[1];
                ui->textBrowser->append("用户 " + order[1] + " 登录成功");
            }
            //成功发回信息后应该断开连接，注意客户端收到消息才断开链接
        }else if (order[0]=="REGISTER") {
            //在此处理注册逻辑
            bool ok=userRegister(order[1], order[2]);
            //同样给客户端回信息
            out << (ok ? "REGISTER SUCCESS" : "REGISTER FAILED");
            out.flush();
            const QString suc=ok?"成功":"失败";
            ui->textBrowser->append("注册信息发回客户端，注册 "+suc);
            //成功发回信息后应该断开连接，客户端收到才断开链接
        }else {
            //这里进行其他处理
            if (order[0]=="MSG") {
                if (order.size() >= 3) {
                    // 提取被引号包裹的消息内容
                    QString fullMessage = str.mid(4); // 跳过 "MSG "
                    int firstQuote = static_cast<int>(fullMessage.indexOf('"'));
                    int lastQuote = static_cast<int>(fullMessage.lastIndexOf('"'));

                    if (firstQuote != -1 && lastQuote != -1 && firstQuote < lastQuote) {
                        QString msg = fullMessage.mid(firstQuote + 1, lastQuote - firstQuote - 1);
                        msg.replace("\\\"", "\""); // 还原转义的引号
                        // 提取用户名（引号后的部分）
                        QString sender = fullMessage.mid(lastQuote + 1).trimmed();
                        // 获取时间日期
                        QDateTime currentDateTime = QDateTime::currentDateTime();
                        QString dateTimeString = "[" + currentDateTime.toString("yyyy-MM-dd hh:mm:ss") + "]";
                        // 构造广播消息
                        QString broadcastMsg = dateTimeString + sender + ": " + msg + "\n";
                        // 广播消息并存储记录
                        for (QTcpSocket* client : clients) {
                            if (client && client->state() == QAbstractSocket::ConnectedState) {
                                QTextStream clientOut(client);
                                clientOut << broadcastMsg;
                                clientOut.flush();
                            }
                        }
                        reco(dateTimeString, sender, msg);
                    } else {
                        ui->textBrowser->append("消息格式错误: " + str);
                    }
                }
            }
        }
    }else if (order.size()==1) {
        //这里是有关聊天记录的功能
        if (order[0]=="HISTORY") {
            //查聊天记录数据库，然后遍历并发送给查询者客户端
            QSqlQuery checkQuery;
            QString sql="SELECT * FROM RECO;";
            if(checkQuery.exec(sql)) {
                int count = 0;
                while (checkQuery.next()) {
                    QString time = checkQuery.value(0).toString();
                    QString user = checkQuery.value(1).toString();
                    QString content = checkQuery.value(2).toString();
                    // 使用明确的格式，确保有换行符
                    QString message = QString("%1 %2: %3\n").arg(time, user, content);
                    // 直接写入套接字，避免QTextStream缓冲
                    clientSocket->write(message.toUtf8());
                    clientSocket->flush(); // 立即发送
                    count++;
                }
                QString xx=QString::number(count)+"条历史记录";
                ui->textBrowser->append("已发送"+xx);
            }else {
                ui->textBrowser->append("error 查聊天记录");
            }
        }
    }else {
        //此处可以加其他功能，只需匹配客户端的指令码即可。
        ui->textBrowser->append("未知错误：位置 server.cpp 250行左右");
    }
}

// 客户端断开连接
void Server::clientDisconnected() {
    auto *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        // 从在线用户列表中移除
        if (clientUserMap.contains(clientSocket)) {
            QString user = clientUserMap[clientSocket];
            onlineUsers.remove(user);
            clientUserMap.remove(clientSocket);
            ui->textBrowser->append("用户 " + user + " 已下线");
        }
        clients.removeAll(clientSocket);
        clientSocket->deleteLater();
        ui->textBrowser->append("客户端断开连接");
        //在这里把下线用户移出在线名单
    }
}