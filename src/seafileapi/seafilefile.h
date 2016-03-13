class SeafileFile;
class SeafileLibrary;
#ifndef SEAFILEFILE_H
#define SEAFILEFILE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedPointer>
#include <QString>

QSharedPointer<SeafileFile> seafileFileFromJson(QJsonObject& doc);


class SeafileFile
{
public:
    QString name;
    bool    isDir;
    bool    isRepo;
    int     size;
    SeafileLibrary* library;
    QSharedPointer<SeafileFile> parent;
    virtual QString path(){
        return parent->path() + '/' + name;
    }
    virtual ~SeafileFile(){}
};

struct SeafileFolder: public SeafileFile{
    QList<QSharedPointer<SeafileFile> > contents;
};

struct SeafileLibrary: public SeafileFolder{
    QString owner;
    QString description;
    QString id;

    // SeafileFile interface
public:
    virtual QString path()
    {
        return '/' + name;
    }
};

#endif // SEAFILEFILE_H
