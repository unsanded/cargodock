#ifndef PLACESMODEL_H
#define PLACESMODEL_H

#include "folderbase.h"

#include <QList>
#include <QSharedPointer>
#include <QVariantMap>

class PlacesModel : public FolderBase
{
    Q_OBJECT
    Q_PROPERTY(QStringList services READ services NOTIFY servicesChanged)
    Q_PROPERTY(bool useEncryptionPassphrase READ useEncryptionPassphrase
               WRITE setUseEncryptionPassphrase
               NOTIFY useEncryptionPassphraseChanged)
public:
    PlacesModel(QObject* parent = 0);

    Q_INVOKABLE bool verifyEncryptionPassphrase(const QString& passphrase) const;

    /* Adds a service instance with icon and name.
     */
    Q_INVOKABLE void addService(const QString& serviceName,
                                const QString& icon,
                                const QVariantMap& properties);

    /* Updates the properties of the given service instance.
     */
    Q_INVOKABLE void updateService(const QString& uid,
                                   const QVariantMap& properties);

    /* Returns the properties of the given service instance.
     */
    Q_INVOKABLE QVariantMap serviceProperties(const QString& uid) const;


    /* Removes the given service instance.
     */
    Q_INVOKABLE void removeService(const QString& uid);

    /* Lists the UIDs of the current service instances.
     */
    Q_INVOKABLE QVariantMap service(const QString& uid) const;

    int capabilities() const;

    virtual bool linkFile(const QString& path,
                          const QString& source,
                          const FolderBase* sourceModel);

    virtual bool deleteFile(const QString& path);

signals:
    void servicesChanged();
    void useEncryptionPassphraseChanged();

protected:
    virtual void init();

    virtual void loadDirectory(const QString& path);

private:
    QStringList services() const;
    Item::Ptr makeItem(const QString& uid,
                       const QString& name,
                       const QString& section,
                       const QString& icon,
                       const QString& type,
                       bool selectable);

    bool useEncryptionPassphrase() const;
    void setUseEncryptionPassphrase(bool value);
};

#endif // PLACESMODEL_H
