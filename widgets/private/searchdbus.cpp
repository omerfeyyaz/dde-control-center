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
 * Command line was: qdbusxml2cpp -c SearchDbus -p searchdbus file
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "searchdbus.h"

/*
 * Implementation of interface class SearchDbus
 */

SearchDbus::SearchDbus(QObject *parent)
    : QDBusAbstractInterface(staticServerPath(), staticInterfacePath(), staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

SearchDbus::~SearchDbus()
{
}

