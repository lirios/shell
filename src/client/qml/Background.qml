/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell 1.0
import Hawaii.Shell.Settings 1.0

Element {
    property var settings: BackgroundSettings {
        id: settings
        onTypeChanged: performChanges(currentRenderer, settings)
        onColorShadingChanged: performChanges(currentRenderer, settings)
        onPrimaryColorChanged: performChanges(currentRenderer, settings)
        onSecondaryColorChanged: performChanges(currentRenderer, settings)
        onWallpaperUrlChanged: performChanges(currentRenderer, settings)
        onFillModeChanged: performChanges(currentRenderer, settings)
    }

    property int fadeDuration: 400

    property var currentRenderer: renderer1
    property var nextRenderer: renderer2

    BackgroundRenderer {
        id: renderer1
        anchors.fill: parent
        opacity: 1.0
    }

    BackgroundRenderer {
        id: renderer2
        anchors.fill: parent
        opacity: 0.0
    }

    Component.onCompleted: {
        // Initialize the first renderer with settings
        updateSettings(currentRenderer, settings);

        // Unload the wallpaper on the second render since we
        // are not showing it anyway
        nextRenderer.unloadBackground();
    }

    function updateSettings(renderer, from) {
        renderer.type = from.type
        renderer.primaryColor = from.primaryColor;
        renderer.secondaryColor = from.secondaryColor;
        renderer.colorShading = from.colorShading;
        renderer.wallpaperUrl = from.wallpaperUrl;
        renderer.fillMode = from.fillMode;
    }

    SequentialAnimation {
        id: swapAnimation

        ScriptAction {
            script: {
                // Update next renderer settings
                updateSettings(nextRenderer, settings);
            }
        }

        ParallelAnimation {
            NumberAnimation {
                target: nextRenderer
                property: "opacity"
                easing.type: Easing.Linear
                to: 1.0
                duration: fadeDuration
            }
            NumberAnimation {
                target: currentRenderer
                property: "opacity"
                easing.type: Easing.Linear
                to: 0.0
                duration: fadeDuration
            }
        }

        ScriptAction {
            script: {
                // Swap renderers
                var saved = nextRenderer;
                nextRenderer = currentRenderer;
                currentRenderer = saved;

                // Unload next renderer wallpaper to save some memory
                nextRenderer.unloadBackground();
            }
        }
    }

    function isChanged(from, to) {
        /*
         * This method returns:
         *  - 0: no changes
         *  - 1: changes (swap renderers)
         *  - 2: changes (don't swap renderers)
         */

        if (from.type != to.type)
            return 1;

        switch (to.type) {
        case BackgroundSettings.ColorBackground:
            switch (to.colorShading) {
            case BackgroundSettings.SolidColorShading:
                if (from.primaryColor != to.primaryColor)
                    return 1;
                break;
            case BackgroundSettings.HorizontalColorShading:
            case BackgroundSettings.VerticalColorShading:
                if ((from.primaryColor != to.primaryColor) || (from.secondaryColor != to.secondaryColor))
                    return 1;
                break;
            }
            break;
        case BackgroundSettings.WallpaperBackground:
            if (from.wallpaperUrl != to.wallpaperUrl)
                return 1;
            else if (from.fillMode != to.fillMode)
                return 2;
            break;
        default:
            break;
        }

        return 0;
    }

    function performChanges(from, to) {
        // If something changed swap the two background renderers
        var changed = isChanged(from, to);
        if (changed == 1)
            swapAnimation.start();
        else if (changed == 2)
            updateSettings(currentRenderer, settings);
    }
}
