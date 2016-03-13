class SeafileRequest;
#ifndef SEAFILEREQUESTS_H
#define SEAFILEREQUESTS_H
#include "seafileserver.h"
#include <QUrlQuery>

#if __cplusplus <= 199711L
//shut up old compilers
#define override
#endif


/**
 * @brief The SeafileRequest class represents a network request to a seafile server.
 */
class SeafileRequest: public QObject, public QNetworkRequest{
    Q_OBJECT
    friend class SeafileServer;
public:

    SeafileRequest(SeafileServer* server, QByteArray subUrl);
    bool isAuthenticaticated(){
        return m_isAuthenticated;
    }
    void authenticate(QByteArray token){
        this->m_isAuthenticated = true;
        setRawHeader("Authorization", "Token " + token);
    }
signals:
    void gotFile(const QByteArray& path, QSharedPointer<SeafileFile> file);
    void done();

protected:
    QNetworkReply* reply;
    SeafileServer* server;
    bool m_isAuthenticated;

    /**
     * @brief setSubUrl sets the part of the url that is not the base address.
     * @param subUrl Anything that comes after `server->baseAddress()` to perform this request.
     */
    void setSubUrl(QByteArray subUrl);
    /**
     * @brief checkAndHandleErrors checks for common errors when a reply is returned.
     * @return
     */
    bool checkAndHandleErrors();

    virtual QNetworkReply* send(QNetworkAccessManager& nam);
};

struct SeafilePingRequest : public SeafileRequest{
    SeafilePingRequest(SeafileServer* server);
Q_OBJECT
protected slots:
   virtual void onResponse() override;
signals:
    void pong(bool auth);
};

/**
 * @brief The seaFilePingAuthRequest struct a ping request with authentication.
 * This can be used to test whether the authentication token is valid. So this request has to be authenticated by the server before being sent.
 */
struct seaFilePingAuthRequest: public SeafileRequest{
    seaFilePingAuthRequest(SeafileServer* server);
    Q_OBJECT
protected slots:
   virtual void onResponse() override;
signals:
    void pong(bool auth);
};

struct SeafileLoadLibrariesRequest : public SeafileRequest{
    SeafileLoadLibrariesRequest(SeafileServer* server);
    Q_OBJECT
protected slots:
    virtual void onResponse();
};

/**
 * @brief The SeafileLibraryRequest struct represents a request for a specific library.
 */
struct SeafileLibraryRequest : public SeafileRequest{
    SeafileLibraryRequest(SeafileServer* server, const QByteArray libraryId, QByteArray subUrl);
    void setSubUrl(QByteArray suburl) ;

private:
    QByteArray m_libId;
    Q_OBJECT
};

struct SeafileLoadDirectoryRequest:  public SeafileLibraryRequest {
    SeafileLoadDirectoryRequest(SeafileServer* server, const QByteArray libId, const QByteArray path);
    Q_OBJECT
public slots:
    virtual void onResponse() override;
protected:
    QByteArray m_path;
};


#endif // SEAFILEREQUESTS_H
