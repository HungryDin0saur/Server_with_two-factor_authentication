#include <QtNetwork>
#include <QtWidgets>
#include "MyServer.h"

// ----------------------------------------------------------------------
MyServer::MyServer(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt)
                                                    , m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this); 
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0, 
                              "Server Error", 
                              "Unable to start the server:" 
                              + m_ptcpServer->errorString()
                             );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), 
            this,         SLOT(slotNewConnection())
           );

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;    
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);

    users.append(logpass1);
    users.append(logpass2);
    users.append(logpass3);
}

// ----------------------------------------------------------------------
/*virtual*/ void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()), 
            this,          SLOT(slotReadClient())
           );

    sendToClient(pClientSocket, "Server Response: Connected!");
}

// ----------------------------------------------------------------------
void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        for(quint64 i=0;i<users.size();i++)
        {
            if(str == users.at(i))
            {
                QString strMessage =
                    time.toString() + " " + "Попытка авторизации: " + str.left(str.indexOf("&")) + " ";
                m_ptxt->append(strMessage);

                authCode = QDateTime::currentDateTime().time().toString().at(3);
                authCode += QDateTime::currentDateTime().time().toString().at(4);
                authCode = QString::number(authCode.toInt() * 90 + 673);

                m_ptxt->append("Отправленный код авторизации: " + authCode);

                Py_Initialize();
                PyRun_SimpleString("import smtplib\n"
                                   "import datetime\n"
                                   "now = datetime.datetime.now()\n"
                                   "code = int((str(now.time()).partition('.')[0]).partition(':')[2].partition(':')[0]) * 90 + 673\n"
                                   "print(code)\n"
                                   "smtpObj = smtplib.SMTP('smtp.gmail.com', 587)\n"
                                   "print(smtpObj.starttls())\n"
                                   "smtpObj.login('serverproject3432432432@gmail.com','alknsd&akjwnsd*Q@OJLAS')\n"
                                   "message = (str(code))\n"
                                   "smtpObj.sendmail('serverproject3432432432@mail.ru','nilsonkevin9@gmail.com', message)\n"
                                   "smtpObj.quit()\n"
                                   );
                Py_Finalize();


                sendToClient(pClientSocket,
                             "Введите код авторизации"
                            );
                m_nNextBlockSize = 0;

                return;
            }
            if(i == (users.size()-1))
            {
                            if(str == authCode)
                            {
                                QString strMessage =
                                    time.toString() + " " + "Код двухэтапной авторизации принят";
                                m_ptxt->append(strMessage);

                                sendToClient(pClientSocket,
                                             "Код двухэтапной авторизации принят"
                                            );
                                m_nNextBlockSize = 0;
                            }
                            else
                            {
                                QString strMessage =
                                    time.toString() + " " + "Попытка авторизации: " + str.left(str.indexOf("&")) + " не успешна(";
                                m_ptxt->append(strMessage);

                                sendToClient(pClientSocket,
                                             "Авторизация не пройдена"
                                            );
                                m_nNextBlockSize = 0;
                            }
            }
        }
    }
}

// ----------------------------------------------------------------------
void MyServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}
