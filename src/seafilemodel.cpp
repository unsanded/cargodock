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
    Item::Ptr item ( new Item);
    if(file->isRepo){
        QSharedPointer<SeafileLibrary> lib (qSharedPointerCast<SeafileLibrary>(file));
        item->name = lib->id;
        if (!m_libraries.contains(lib->id)) {
            m_libraries.insert(lib->id, lib);
            qDebug() << "Adding library" << lib->id << lib->name;
        }
    } else {
        item->name = file->name;
    }

    if(file->isDir) {
        item->type = Folder;
    } else {
        item->type = File;
    }
    item->friendlyName = file->name;
    if(file->path() == currentPath){
        appendItem(item);
    } else {
        qWarning () << "ignoring file outside current dir: "  <<  file->path();
    }
}

void SeafileModel::onLoadFinished()
{
    setLoading(false);
}

void SeafileModel::loadDirectory(const QString &path)
{
    QStringList pathList = path.split('/', QString::SkipEmptyParts);
    clearItems();
    setLoading(true);

    if (pathList.isEmpty()){
        // this is a request for the root. ie. the list of libraries.
        server()->loadLibraries();
        this->currentPath="";
    }
    else
    {
        QByteArray library = pathList.takeFirst().toUtf8();
        QByteArray newPath = pathList.join('/').toUtf8() + '/';
        server()->loadDirectory(library, newPath);
        this->currentPath = newPath;
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

QString SeafileModel::friendlyBasename(const QString &path) const
{
    QStringList pathList(path.split('/', QString::SkipEmptyParts));

    if(pathList.isEmpty()){
        // it is the root.
        return "<b>sea</b>file";
    }
    if(pathList.length()==1){
        // it is a repo.
        if(m_libraries.contains(pathList[0])){
            return m_libraries[pathList[0]]->name;
        }
        else
        {
            return "[library name unknown]";
        }
    } else {
        // it is a foder of a file: It has no special name.
        return basename(path);
    }
}


SeafileServer *SeafileModel::getServer(){
    return server();
}
