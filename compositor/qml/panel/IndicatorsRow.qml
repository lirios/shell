/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import Fluid.Controls 1.0
import Fluid.Material 1.0
import Liri.Shell 1.0
import "../indicators"
import "../launcher"

Row {
    id: indicatorsView

    height: parent.height

    signal indicatorTriggered(var indicator)

    DateTimeIndicator {
        onClicked: indicatorTriggered(caller)
    }

    NotificationsIndicator {
        onClicked: indicatorTriggered(caller)
    }

    SoundIndicator {
        onClicked: indicatorTriggered(caller)
    }

    NetworkIndicator {
        onClicked: indicatorTriggered(caller)
    }

    StorageIndicator {
        onClicked: indicatorTriggered(caller)
    }

    BatteryIndicator {
        onClicked: indicatorTriggered(caller)
    }
}
