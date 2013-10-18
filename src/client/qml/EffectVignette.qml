/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

Item {
    id: rootItem

    /*!
        This property defines the source item that is going to be used as source
        for the generated effect.
    */
    property variant source

    ShaderEffect {
        property variant source: ShaderEffectSource {
            sourceItem: rootItem.source
            sourceRect: Qt.rect(0, 0, 0, 0)
            hideSource: false
            smooth: true
            visible: false
        }

        property real dividerValue: 1.0

        anchors.fill: parent

        // Based on http://kodemongki.blogspot.com/2011/06/kameraku-custom-shader-effects-example.html
        fragmentShader: "
varying highp vec2 qt_TexCoord0;
uniform highp float qt_Opacity;
uniform highp float dividerValue;
uniform lowp sampler2D source;

void main()
{
    highp vec2 uv = qt_TexCoord0.xy;
    lowp vec4 orig = texture2D(source, uv);

    highp float cr = pow(0.1, 2.0);
    highp float pt = pow(uv.x - 0.5, 2.0) + pow(uv.y - 0.5, 2.0);
    highp float d = pt - cr;
    highp float cf = 1.0;

    if (d > 0.0)
        cf = 1.0 - 2.0 * d;

    lowp vec3 col = cf * orig.rgb;

    if (uv.x < dividerValue)
        gl_FragColor = qt_Opacity * vec4(col, 1.0);
    else
        gl_FragColor = qt_Opacity * orig;
}
"
    }
}
