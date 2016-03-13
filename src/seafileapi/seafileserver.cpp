#include "seafileserver.h"

#include <QHttpMultiPart>
#include <QJsonDocument>
#include <qnetworkreply.h>

#include "../network.h"
#include "seafilerequests.h"

SeafileServer::SeafileServer(QByteArray address, QObject *parent):
    QObject(parent),
    m_baseAddress(address),
    nam(*Network::accessManager()),
    fileCache(200)
{  }

SeafileServer::SeafileServer(QByteArray address, QByteArray token, QObject *parent) :
    QObject(parent),
    authToken(token),
    m_baseAddress(address),
    nam(*Network::accessManager()),
    fileCache(200)
{  }

void SeafileServer::aquireToken(QString username, QString password)
{
    QNetworkRequest req;
    QByteArray url = this->baseAddress() + "/auth-token/";
    QByteArray postData("username=" + username.toUtf8());
    postData += "&password=" + password.toUtf8();
    req.setUrl(QString(url));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");


    QNetworkReply* reply = nam.post(req, postData);
    connect(reply, SIGNAL(finished()),
            this , SLOT(onTokenReceived())
            );
    connect(reply, SIGNAL(finished()),
            reply, SLOT(deleteLater())
            );
}

void SeafileServer::onTokenReceived()
{

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply->error() != QNetworkReply::NoError){
        qWarning() << "error getting token" << reply->error() << reply->errorString();
        return;
    }
    QByteArray data (reply->readAll());
    if(data.isEmpty()){
        qWarning() << "Received empty reply while getting token";
    }
    QJsonDocument doc = QJsonDocument::fromJson(data);

    // We use utf8, but there shouldn't be any special chars.
    qDebug() << doc.object()["token"];
    setAuthenticationToken( doc.object()[ "token" ].toString().toUtf8() );

    //verify it right away.
    this->ping(true);
}

void SeafileServer::ping(bool authenticate)
{
    SeafileRequest* request;
    if (authenticate)
        request = new seaFilePingAuthRequest(this);
    else
        request = new SeafilePingRequest(this);

    if(authenticate)
        this->authenticateRequest(request);
    connect(request, SIGNAL(pong(bool)),
            this   , SLOT(onPong(bool)));
    this->sendRequest(request);
}

void SeafileServer::onPong(bool auth)
{
    if(status() != Authenticated)
    {
        if(auth)
            setStatus(SeafileServer::Authenticated);
        else
            setStatus(SeafileServer::Connectable);
    }
    emit pong();
}

void SeafileServer::loadLibraries()
{
    SeafileLoadLibrariesRequest* req = new SeafileLoadLibrariesRequest(this);
    this->authenticateRequest(req);
    this->sendRequest(req);
    connect(
                req, SIGNAL(done()),
                this, SLOT (onLoadingDone())
            );
}

void SeafileServer::loadDirectory(const QByteArray &library, const QByteArray &path)
{
    SeafileLoadDirectoryRequest* req = new SeafileLoadDirectoryRequest(this, library, path);
    this->authenticateRequest(req);
    this->sendRequest(req);
    connect(
                req, SIGNAL(done()),
                this, SLOT (onLoadingDone())
            );
}

void SeafileServer::onLoadingDone()
{
    emit loadingDone();
}

void SeafileServer::setStatus(SeafileServer::ServerStatus status)
{
    if(status == this->m_status) return;

    this->m_status = status;
    emit statusChanged(status);
}

void SeafileServer::onFoundFile(QByteArray path, QSharedPointer<SeafileFile> f)
{
    QSharedPointer<SeafileFile>* existingFile = fileCache[path];
    if(existingFile)
    {
        //update the existing file
        **existingFile = *f;
    }
    else
    {
        //This is a little bit ugly, but it is the most efficient way of keeping files.
        //The cache will keep the most recent files. And the QSharedPointer makes sure it doesn't have to be copied.
        fileCache.insert(path, new QSharedPointer<SeafileFile>(f));
    }
    emit file(f);
}


void SeafileServer::sendRequest(SeafileRequest *request)
{
    QNetworkReply* reply = request->send(this->nam);
    connectRequest(request, reply);
}

void SeafileServer::connectRequest(SeafileRequest *request, QNetworkReply *reply)
{
    reply->setParent(request);
    request->reply = reply;
    connect(reply, SIGNAL(finished()),
            request,SLOT (onResponse()));
    connect(reply, SIGNAL(finished()),
            request, SLOT(deleteLater()));
    connect(request, SIGNAL(gotFile(QByteArray,QSharedPointer<SeafileFile>)),
            this   , SLOT(onFoundFile(QByteArray , QSharedPointer<SeafileFile> ))
            );
}

void SeafileServer::authenticateRequest(SeafileRequest *request)
{
    request->authenticate(this->authToken);
}


