#include <QtWidgets>
#include <QtGui>
#include "MyClient.h"
#include <QDebug>

// ----------------------------------------------------------------------
MyClient::MyClient(const QString& strHost, 
                   int            nPort, 
                   QWidget*       pwgt /*=0*/
                  ) : QWidget(pwgt)
                    , m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

    m_ptxtInfo  = new QTextEdit;
    m_ptxtInputLogin = new QLineEdit;
    m_ptxtInputPassword = new QLineEdit;
    m_ptxtInputCode = new QLineEdit;

    m_ptxtInfo->setReadOnly(true);

    QPushButton* pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));

    pcmd1 = new QPushButton("&Send code");
    connect(pcmd1, SIGNAL(clicked()), SLOT(slotSendToServer1()));

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;    
    pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addWidget(m_ptxtInputLogin);
    pvbxLayout->addWidget(m_ptxtInputPassword);
    pvbxLayout->addWidget(m_ptxtInputCode);
    pvbxLayout->addWidget(pcmd);
    pvbxLayout->addWidget(pcmd1);
    setLayout(pvbxLayout);

    pcmd1->setEnabled(false);
    m_ptxtInputCode->setEnabled(false);
}

// ----------------------------------------------------------------------
void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        m_ptxtInfo->append(time.toString() + " " + str);
        m_nNextBlockSize = 0;


        if(str == "Введите код авторизации")
        {
            QMessageBox::information(this, "Информация", "Код второго этапа авторизации отправлен на вашу электронную почту!");
            m_ptxtInputCode->setEnabled(true);
            pcmd1->setEnabled(true);
        }
    }
}

// ----------------------------------------------------------------------
void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError = 
        "Error: " + (err == QAbstractSocket::HostNotFoundError ? 
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ? 
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ? 
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    m_ptxtInfo->append(strError);
}

// ----------------------------------------------------------------------
void MyClient::slotSendToServer()
{
    QString auth = m_ptxtInputLogin->text() + "&" + m_ptxtInputPassword->text();
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << auth;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}

void MyClient::slotSendToServer1()
{
    QString auth = m_ptxtInputCode->text();
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << auth;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    pcmd1->setEnabled(false);
    m_ptxtInputCode->setEnabled(false);
}

// ------------------------------------------------------------------
void MyClient::slotConnected()
{
    m_ptxtInfo->append("Received the connected() signal");
}
