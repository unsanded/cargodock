#include "seafilemodel.h"

SeafileModel::SeafileModel(QObject *parent):
    FolderBase(parent),
    m_server(NULL)
{
}

SeafileServer *SeafileModel::server(){
    return m_server;
}

void SeafileModel::setServer(SeafileServer *server)
{
    if (m_server == server)
        return;

    m_server = server;
    emit serverChanged(server);
}

void SeafileModel::authenticate(QString username, QString password)
{
    server()->aquireToken(username, password);
}

void SeafileModel::onFile(QSharedPointer<SeafileFile> file)
{
    if(true){ //file->parent->path() == currentPath){
        Item::Ptr item ( new Item);
        if(file->isRepo){
            QSharedPointer<SeafileLibrary> lib (qSharedPointerCast<SeafileLibrary>(file));
            item->name = lib->id;
        }
        else
        {
            item->name = file->name;
        }
        if(file->isDir)
        {
            item->type = Folder;
        }
        else
        {
            item->type = File;
        }
        item->friendlyName = file->name;
        item->path = currentPath;
        appendItem(item);
    }
}

void SeafileModel::onLoadFinished()
{
    setLoading(false);
}

void SeafileModel::loadDirectory(const QString &path)
{
    currentPath = path;
    QStringList pathList = path.split('/', QString::SkipEmptyParts);
    clearItems();
    setLoading(true);

    if (pathList.isEmpty()){
        // this is a request for the root. ie. the list of libraries.
        server()->loadLibraries();
    }
    else
    {
        QByteArray library = pathList.takeFirst().toUtf8();
        QByteArray newPath = pathList.join('/').toUtf8() + '/';
        server()->loadDirectory(library, newPath);
    }
}

void SeafileModel::init()
{
    QVariant token   ( configValue("token:blowfish") );
    QVariant baseUrl ( configValue("address")        );

    if(!baseUrl.type() != QVariant::String){
        qWarning() << "INVALID URL" << baseUrl;
        emit error(QString("Seafile: invalid address of type ") + token.typeName());
    }
    if(!token.type() != QVariant::String){
        qWarning() << "INVALID TOKEN" << token;
        emit error(QString("Seafile: invalid token of type ") + token.typeName());
    }
    this->m_server = new SeafileServer(baseUrl.toByteArray(), token.toByteArray());
    this->m_server->ping(true);

    connect(this->m_server, SIGNAL(loadingDone()),
            this          , SLOT(onLoadFinished())
            );
    connect(
            this->m_server, SIGNAL(file(QSharedPointer<SeafileFile>)),
            this          , SLOT  (onFile(QSharedPointer<SeafileFile>))
                );
}

QVariant SeafileModel::data(const QModelIndex &index, int role) const
{
    return FolderBase::data(index, role);
}


SeafileServer *SeafileModel::getServer(){
    return server();
}
