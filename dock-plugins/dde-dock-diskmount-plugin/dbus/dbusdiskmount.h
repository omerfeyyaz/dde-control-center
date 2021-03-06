/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DBusDiskMount -p dbusdiskmount com.deepin.daemon.DiskMount.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DBUSDISKMOUNT_H_1437372053
#define DBUSDISKMOUNT_H_1437372053

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <QDBusArgument>
#include <QDebug>

struct DiskInfo {
    QString id;
    QString name;
    QString type;
    QString path;
    QString mountPoint;
    QString icon;

    bool canUnmount;
    bool canEject;

    qulonglong used;
    qulonglong total;
};
typedef QList<DiskInfo> DiskInfoList;
Q_DECLARE_METATYPE(DiskInfo)
Q_DECLARE_METATYPE(DiskInfoList)
// Marshall the DiskInfoList data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const DiskInfo &diskInfo);
// Retrieve the DiskInfoList data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, DiskInfo &diskInfo);

/*
 * Proxy class for interface com.deepin.daemon.DiskMount
 */
class DBusDiskMount: public QDBusAbstractInterface
{
    Q_OBJECT

    Q_SLOT void __propertyChanged__(const QDBusMessage &msg)
    {
        QList<QVariant> arguments = msg.arguments();
        if (3 != arguments.count()) {
            return;
        }
        QString interfaceName = msg.arguments().at(0).toString();
        if (interfaceName != "com.deepin.daemon.DiskMount") {
            return;
        }
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        foreach(const QString & prop, changedProps.keys()) {
            const QMetaObject *self = metaObject();
            for (int i = self->propertyOffset(); i < self->propertyCount(); ++i) {
                QMetaProperty p = self->property(i);
                if (p.name() == prop) {
                    Q_EMIT p.notifySignal().invoke(this);
                }
            }
        }
    }
public:
    static inline const char *staticServerPath()
    { return "com.deepin.daemon.DiskMount"; }
    static inline const char *staticInterfacePath()
    { return "/com/deepin/daemon/DiskMount"; }
    static inline const char *staticInterfaceName()
    { return "com.deepin.daemon.DiskMount"; }

public:
    DBusDiskMount(QObject *parent = 0);

    ~DBusDiskMount();

    Q_PROPERTY(DiskInfoList DiskList READ diskList NOTIFY DiskListChanged)
    inline DiskInfoList diskList() const
    {
        return qvariant_cast< DiskInfoList >(property("DiskList"));
    }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Eject(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("Eject"), argumentList);
    }

    inline QDBusPendingReply<> Mount(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("Mount"), argumentList);
    }

    inline QDBusPendingReply<> Unmount(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("Unmount"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void Changed(const int in0, const QString &in1);
    void Error(const QString &id, const QString &reason);
// begin property changed signals
    void DiskListChanged();
};

namespace com
{
namespace deepin
{
namespace daemon
{
typedef ::DBusDiskMount DiskMount;
}
}
}
#endif
