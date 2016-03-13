#ifndef SEAFILEMODEL_H
#define SEAFILEMODEL_H

#include "folderbase.h"

#include <QObject>

#include "seafileapi/seafileserver.h"

class SeafileModel : public FolderBase
{
Q_OBJECT
    Q_PROPERTY(SeafileServer* server READ server NOTIFY serverChanged WRITE setServer)
    SeafileServer *m_server;
public:
    SeafileModel(QObject* parent=0);

    Q_INVOKABLE SeafileServer* getServer();

    Q_INVOKABLE SeafileServer* server();

signals:

    void serverChanged(SeafileServer* server);

public slots:
    void setServer(SeafileServer* server);

protected slots:
    void authenticate(QString username, QString password);
    void onFile(QSharedPointer<SeafileFile> file);
    void onLoadFinished();
    // FolderBase interface
protected:
    virtual void loadDirectory(const QString &path) override;

    // FolderBase interface
protected:
    virtual void init() override;
    void setLoading(bool value){
        if(this->m_isLoading == value) return;
        this->m_isLoading = value;
        emit loadingChanged();
    }



    // FolderBase interface
public:
    virtual void rename(const QString &name, const QString &newName)
    {
        //TODO
        qWarning() << "unimplemented rename " << name << newName;
    }
    virtual QString readFile(const QString &name) const
    {
        //TODO
        qWarning() << "unimplemented readFile" << name;
        return "";
    }

protected:
    virtual bool loading() const
    {
        return this->m_isLoading;
    }
private:
    bool m_isLoading;
    QString currentPath;

    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const;
};



#endif // SEAFILEMODEL_H
