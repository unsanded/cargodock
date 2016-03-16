#include "seafilefile.h"
#include <QDebug>

QSharedPointer<SeafileFile> seafileFileFromJson(QJsonObject &doc)
{
    QString type = doc["type"].toString();
    SeafileFile* result;
    if(type == "file"){
        result = new SeafileFile;
        result->isDir  = false;
        result->isRepo = false;
    }
    else
    if(type == "repo"){
        result = new SeafileLibrary;
        result->isDir  = true;
        result->isRepo = true;
        ((SeafileLibrary*) result)->description = doc["description"].toString();
        ((SeafileLibrary*) result)->id = doc["id"].toString();
    }
    else
    if(type == "dir")
    {
        result = new SeafileFolder;
        result->isDir  = true;
        result->isRepo = false;
    }
    else
    {
        qWarning() << "UNKNOWN TYPE" << type;
        return QSharedPointer<SeafileFile>();
    }
    result->name = doc["name"].toString();

    return QSharedPointer<SeafileFile>(result);
}


