class SeafileServer;
#ifndef SEAFILEAPI_H
#define SEAFILEAPI_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QCache>

#include "seafilefile.h"
#include "seafilerequests.h"

class SeafileServer : public QObject
{
    friend class SeafileRequest;
    Q_ENUMS(ServerStatus)
    Q_OBJECT
    Q_PROPERTY(ServerStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QByteArray baseAddress READ baseAddress WRITE setBaseAddress NOTIFY baseAddressChanged)
    Q_PROPERTY(QByteArray authenticationToken READ authenticationToken WRITE setAuthenticationToken NOTIFY authenticationTokenChanged)

    QList<SeafileLibrary> libraries;
public:
    explicit SeafileServer(QByteArray baseAddress="", QObject *parent = 0);
    explicit SeafileServer(QByteArray baseAddress, QByteArray token, QObject *parent = 0);

    enum ServerStatus {
        UnknownConnectable,
        UnConnectable,
        Connectable,
        Authenticated,
    };
    ServerStatus m_status;

    Q_INVOKABLE void aquireToken(QString username, QString password);
    void updateLibraries();

    QSharedPointer<SeafileFile> getFile(QString path);


ServerStatus status() const
{
    return m_status;
}

void setAuthenticationToken(QByteArray token, bool ping=true){
    qDebug() << "CHANEGED TOKEN TO" << token;
    if(this->authToken == token)
        return;
    
    this->authToken=token;
    if(status() == Authenticated){
        m_status = Connectable;
        emit(statusChanged(m_status));
        qWarning() << "SEAFILE: changed a working token";
    }
    emit authenticationTokenChanged(token);
    if(ping) this->ping(true);
}

QByteArray baseAddress() const
{
    return m_baseAddress;
}

QByteArray authenticationToken() const
{
    return authToken;
}

signals:
    void authenticationFailed();
    void tokenAquired();
    void error(QString reason);
    void statusChanged(ServerStatus status);
    void pong();
    void loadingDone();
    void file(QSharedPointer<SeafileFile> file);

    void baseAddressChanged(QByteArray baseAddress);


    void authenticationTokenChanged(QByteArray authenticationToken);

public slots:
    // in (expected) chronological order;
    void ping(bool authenticate=true);
    void onPong(bool auth);
    void loadLibraries();
    void loadDirectory(const QByteArray& library, const QByteArray& path);
    void onLoadingDone();

    void setBaseAddress(QByteArray baseAddress)
    {
        if (m_baseAddress == baseAddress)
            return;

        setStatus(UnknownConnectable);
        m_baseAddress = baseAddress;
        emit baseAddressChanged(baseAddress);
    }

protected slots:
    void setStatus(ServerStatus status);
    void onFoundFile(QByteArray path, QSharedPointer<SeafileFile> file);
    void onTokenReceived();

protected:
    void authenticateRequest(SeafileRequest* request);

    void sendRequest (SeafileRequest *request);
    /**
     * @brief connectRequest is called whenever a request is sent. This connects several signals of the request.
     * @param request
     * @param reply
     */
    void connectRequest(SeafileRequest* request, QNetworkReply* reply);

    QByteArray authToken;
    QByteArray m_baseAddress;
    QNetworkAccessManager& nam;

private:
    QCache<QByteArray, QSharedPointer<SeafileFile> > fileCache;
};

#endif // SEAFILEAPI_H
