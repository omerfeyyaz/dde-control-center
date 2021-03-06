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
 * Command line was: qdbusxml2cpp -c DBusSoundEffects -p dbussoundeffects com.deepin.daemon.SoundEffect.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DBUSSOUNDEFFECTS_H_1447211852
#define DBUSSOUNDEFFECTS_H_1447211852

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.daemon.SoundEffect
 */
class DBusSoundEffects: public QDBusAbstractInterface
{
    Q_OBJECT

    Q_SLOT void __propertyChanged__(const QDBusMessage &msg)
    {
        QList<QVariant> arguments = msg.arguments();
        if (3 != arguments.count()) {
            return;
        }
        QString interfaceName = msg.arguments().at(0).toString();
        if (interfaceName != "com.deepin.daemon.SoundEffect") {
            return;
        }
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        foreach(const QString & prop, keys) {
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
    static inline const char *staticInterfaceName()
    { return "com.deepin.daemon.SoundEffect"; }

public:
    DBusSoundEffects(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DBusSoundEffects();

    Q_PROPERTY(bool Enabled READ enabled WRITE setEnabled NOTIFY EnabledChanged)
    inline bool enabled() const
    { return qvariant_cast< bool >(property("Enabled")); }
    inline void setEnabled(bool value)
    { setProperty("Enabled", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> PlaySystemSound(const QString &in0)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        return asyncCallWithArgumentList(QStringLiteral("PlaySystemSound"), argumentList);
    }

Q_SIGNALS: // SIGNALS
// begin property changed signals
    void EnabledChanged();
};

namespace com
{
namespace deepin
{
namespace daemon
{
typedef ::DBusSoundEffects SoundEffect;
}
}
}
#endif
