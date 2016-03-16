#include "seafilerequests.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>



bool SeafileRequest::checkAndHandleErrors()
{
    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if( reply->error() == QNetworkReply::NoError)
        return true;
    // 404 error or equivalent
    if(
            (reply->error() == QNetworkReply::ContentAccessDenied)
            ||
            (reply->error() == QNetworkReply::AuthenticationRequiredError)
            ||
            (httpCode == 404)
        )
        if(server->status()==SeafileServer::Authenticated){
            if(this->isAuthenticaticated()){
                qWarning() << "WRONG AUTHENTICATION";
                server->setStatus(SeafileServer::Connectable);
            }
            else
            {
                qWarning() << "REQUEST NOT AUTHENTICATED";
            }
            return false;
        }
    qWarning() << "UNKNOWN CONNECTION ERROR "  << reply->error() << reply->errorString();
    server->setStatus(SeafileServer::UnConnectable);
    return false;
}

QNetworkReply *SeafileRequest::send(QNetworkAccessManager &nam){
    return nam.get(*this);
}


void SeafileLoadLibrariesRequest::onResponse()
{
    if(!checkAndHandleErrors()) return;
    QByteArray data(this->reply->readAll());
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonArray responseList = doc.array();

    foreach (QJsonValue val , responseList)
    {
        QJsonObject libraryObject (val.toObject());
        QSharedPointer<SeafileFile> lib(seafileFileFromJson(libraryObject));

        // libraries have an empty path.
        lib->m_path = "";
        emit gotFile("", lib);
    }
    emit done();
}
void SeafileLoadDirectoryRequest::onResponse()
{
    if(!checkAndHandleErrors()) return;
    QByteArray data(this->reply->readAll());
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonArray responseList = doc.array();

    foreach (QJsonValue val , responseList)
    {
        QJsonObject libraryObject (val.toObject());
        QSharedPointer<SeafileFile> lib(seafileFileFromJson(libraryObject));
        lib->m_path = this->m_path;
        emit gotFile(m_path, lib);
    }
    emit done();
}


SeafileRequest::SeafileRequest(SeafileServer *server, QByteArray subUrl):
    server(server),
    m_isAuthenticated(false)
{
    setSubUrl(subUrl);
}

void SeafileRequest::setSubUrl(QByteArray subUrl)
{
    setUrl(QUrl(this->server->baseAddress() + '/' + subUrl));
}

SeafilePingRequest::SeafilePingRequest(SeafileServer *server):
    SeafileRequest(server, "ping/") {}

void SeafilePingRequest::onResponse()
{
    if(checkAndHandleErrors()){
        emit pong(false);
    }
}

seaFilePingAuthRequest::seaFilePingAuthRequest(SeafileServer *server):
    SeafileRequest(server, "auth/ping/"){}

void seaFilePingAuthRequest::onResponse()
{
    if(checkAndHandleErrors()){
        emit pong(true);
    }
}

SeafileLibraryRequest::SeafileLibraryRequest(SeafileServer *server, const QByteArray libraryId, QByteArray subUrl):
    SeafileRequest(server, ""),
    m_libId(libraryId)
{
    setSubUrl(subUrl);
}

void SeafileLibraryRequest::setSubUrl(QByteArray suburl)
{
    SeafileRequest::setSubUrl("repos/" + m_libId + '/' + suburl);
}

SeafileLoadDirectoryRequest::SeafileLoadDirectoryRequest(SeafileServer *server, QByteArray libId, const QByteArray path="/"):
    SeafileLibraryRequest(server, libId, "dir/"),
    m_path(path)
{
    QUrl u = this->url();
    u.setQuery("p=" + path);
    setUrl(u);
}


SeafileLoadLibrariesRequest::SeafileLoadLibrariesRequest(SeafileServer *server):
    SeafileRequest(server, "repos/")
{}
