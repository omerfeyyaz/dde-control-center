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
 * Command line was: qdbusxml2cpp -c ShortcutDbus -p shortcutdbus file
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

#include "shortcutdbus.h"

/*
 * Implementation of interface class ShortcutDbus
 */

enum KeyType{
    KeyTypeSystem = 0, // 0
    KeyTypeCustom,
    KeyTypeMedia,
    KeyTypeWM,
    KeyTypeMetacity,
    UnKnow
};

ShortcutDbus::ShortcutDbus(QObject *parent)
    : QDBusAbstractInterface(staticServerPath(), staticInterfacePath(), staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
    connect(this, &ShortcutDbus::Changed, this, &ShortcutDbus::updateShortcutList);
    connect(this, &ShortcutDbus::KeyEvent, [&](bool arg, const QString &str){
        if(arg)
            emit KeyPressEvent(str);
        else
            emit KeyReleaseEvent(str);
    });
    connect(this, &ShortcutDbus::Added, this, &ShortcutDbus::onAdded);
    connect(this, &ShortcutDbus::Deleted, this, &ShortcutDbus::onDeleted);

    QStringList list;
    list << "launcher" << "show-desktop" << "lock-screen" << "file-manager" << "screenshot"
         << "screenshot-window" << "screenshot-delayed"
         << "terminal"
#if !defined(PLATFORM_SUNWAY) && !defined(ARCH_MIPSEL)
         << "terminal-quake"
#endif
         << "logout" << "switch-layout"
         << "preview-workspace" << "expose-windows" << "expose-all-windows"
         << "switch-group" << "switch-group-backward" << "switch-applications"
#if !defined(PLATFORM_SUNWAY) && !defined(ARCH_MIPSEL)
         << "switch-applications-backward" << "wm-switcher";
#else
         << "switch-applications-backward";
#endif

    QDBusInterface dbus("com.deepin.daemon.InputDevice.InputDevices",
                        "/com/deepin/daemon/InputDevice/TouchPad",
                        "com.deepin.daemon.InputDevice.TouchPad");
    if(dbus.property("Exist").toBool())
        list << "disable-touchpad";

    foreach (QString str, list) {
        ShortcutInfo info = ShortcutInfo{SystemList, UnKnow, getId(), str, "", tr("None")};
        m_systemList.append(info);
        m_idToInfoListHash[str] = &m_systemList.last();
        m_intIdToInfoMap[info.id] = &m_systemList.last();
    }

    list.clear();
    list << "close" << "maximize" << "unmaximize" << "begin-move"
         << "begin-resize";

    foreach (QString str, list) {
        ShortcutInfo info = ShortcutInfo{WindowList, UnKnow, getId(), str, "", tr("None")};
        m_windowList.append(info);
        m_idToInfoListHash[str] = &m_windowList.last();
        m_intIdToInfoMap[info.id] = &m_windowList.last();
    }

    list.clear();
    list << "switch-to-workspace-left" << "switch-to-workspace-right"
         << "move-to-workspace-left" << "move-to-workspace-right";

    foreach (QString str, list) {
        ShortcutInfo info = ShortcutInfo{WorkspaceList, UnKnow, getId(), str, "", tr("None")};
        m_workspaceList.append(info);
        m_idToInfoListHash[str] = &m_workspaceList.last();
        m_intIdToInfoMap[info.id] = &m_workspaceList.last();
    }

    updateShortcutList("", -1);
}

ShortcutDbus::~ShortcutDbus()
{
}

int ShortcutDbus::getId()
{
    return m_intId++;
}

QString ShortcutDbus::getId(int type, const QString &id) const
{
    return QString::number(type)+id;
}

void ShortcutDbus::updateShortcutList(const QString &id, qint32 type)
{
    if(type == -1){
        QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(List(), this);
        connect(result, &QDBusPendingCallWatcher::finished, this, &ShortcutDbus::getListFinished);
    }else{
        ShortcutInfo *info = m_strIdAndTypeToInfoMap[getId(type, id)];
        if(info){
            QDBusPendingReply<QString> info_json = Query(id, type);
            info_json.waitForFinished();
            info->shortcut = QJsonDocument::fromJson(info_json.value().toUtf8()).object()["Accels"].toArray()[0].toString();
            if(info->shortcut.isEmpty())
                info->shortcut = tr("None");

            switch (info->listType) {
            case SystemList:
                emit systemListChanged(m_systemList);
                break;
            case WindowList:
                emit windowListChanged(m_windowList);
                break;
            case WorkspaceList:
                emit workspaceListChanged(m_workspaceList);
                break;
            case CustomList:
                emit customListChanged(m_customList);
                break;
            default:
                break;
            }
        }
    }
}

void ShortcutDbus::onAdded(const QString &id, qint32 type)
{
    if(type == KeyTypeCustom){
        QDBusPendingReply<QString> info_json = Query(id, type);
        info_json.waitForFinished();
        QString name = QJsonDocument::fromJson(info_json.value().toUtf8()).object()["Name"].toString();
        m_customList << ShortcutInfo{CustomList, type, getId(), id, name, tr("None")};
        m_strIdAndTypeToInfoMap[getId(type, id)] = &m_customList.last();
        m_intIdToInfoMap[m_customList.last().id] = &m_customList.last();

        emit customListChanged(m_customList);
    }
}

void ShortcutDbus::onDeleted(const QString &id, qint32 type)
{
    if(type == KeyTypeCustom){
        const ShortcutInfo *info = m_strIdAndTypeToInfoMap[getId(type, id)];
        if(info)
            m_customList.removeOne(*info);

        emit customListChanged(m_customList);
    }
}

void ShortcutDbus::getListFinished(QDBusPendingCallWatcher *call)
{
    m_customList.clear();

    QDBusPendingReply<QString> result = *call;

    QJsonArray list = QJsonDocument::fromJson(result.value().toUtf8()).array();

    foreach (QJsonValue obj, list) {
        QJsonObject map = obj.toObject();

        int type = map["Type"].toInt();
        QString shortcuts = map["Accels"].toArray()[0].toString();
        if(shortcuts.isEmpty())
            shortcuts = tr("None");
        QString name = map["Name"].toString();
        QString id = map["Id"].toString();

        switch (type) {
        case KeyTypeSystem://express
        case KeyTypeMetacity://express
        case KeyTypeWM:{
            ShortcutInfo *info = m_idToInfoListHash.value(id, NULL);
            if(info){
                info->type = type;
                info->title = name;
                info->shortcut = shortcuts;
                m_strIdAndTypeToInfoMap[getId(type, id)] = info;
            }
            break;
        }
        case KeyTypeCustom:{
            m_customList << ShortcutInfo{CustomList, type, getId(), id, name, shortcuts};
            m_strIdAndTypeToInfoMap[getId(type, id)] = &m_customList.last();
            m_intIdToInfoMap[m_customList.last().id] = &m_customList.last();
            break;
        }
        default:
            break;
        }
    }

    emit systemListChanged(m_systemList);
    emit windowListChanged(m_windowList);
    emit workspaceListChanged(m_workspaceList);
    emit customListChanged(m_customList);

    call->deleteLater();
}

