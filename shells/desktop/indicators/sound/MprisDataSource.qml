/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2012 Alex Merry <alex.merry@kdemail.net>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

PlasmaCore.DataSource {
    id: mprisDataSource
    engine: "mpris2"
    connectedSources: current
    interval: 0

    property string current: "@multiplex"

    property bool hasMetadata: getHasMetadata()
    property string title: getMetadata("xesam:title", "")
    property string artist: getMetadata("xesam:artist", []).join(", ")
    property string album: getMetadata("xesam:album", "")
    property url pictureUrl: getMetadata("mpris:artUrl", "")
    property int trackLength: getMetadata("mpris:length", 0) / 1000
    property int trackPosition: getHasData() ? data[current]["Position"] : 0
    property string status: getHasData() ? data[current]["PlaybackStatus"] : "unknown"
    property bool canControl: getHasData() && data[current]["CanControl"]

    readonly property bool available: hasMetadata && canControl

    function startOperation(source, op) {
        var service = mprisDataSource.serviceForSource(source);
        var operation = service.operationDescription(op);
        return service.startOperationCall(operation);
    }

    function openMediaPlayer() {
        startOperation(current, "Raise");
    }

    function playPause() {
        startOperation(current, "PlayPause");
    }

    function prevTrack() {
        startOperation(current, "Previous");
    }

    function nextTrack() {
        startOperation(current, "Next");
    }

    function getHasData() {
        return data[current] !== undefined &&
                data[current]["PlaybackStatus"] !== undefined;
    }

    function getHasMetadata() {
        return data[current] !== undefined &&
                data[current]["Metadata"] !== undefined &&
                data[current]["Metadata"]["mpris:trackid"] !== undefined;
    }

    function getMetadata(entry, def) {
        if (hasMetadata && data[current]["Metadata"][entry] !== undefined)
            return data[current]["Metadata"][entry];
        return def;
    }
}
