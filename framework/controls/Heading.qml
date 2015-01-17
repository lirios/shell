/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2012 Sebastian Kügler <sebas@kde.org>
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

import QtQuick 2.1
import QtQuick.Controls 1.2 as QtControls
import Hawaii.Themes 1.0

/**
 * A heading label used for subsections of texts.
 *
 * The characteristics of the text will be automatically set according to the
 * Hawaii theme. Use this components for section titles or headings in your UI,
 * for example page or section titles.
 *
 * Example usage:
 *
 * @code
 * import Hawaii.Controls 1.0 as Controls
 * [...]
 * Column {
 *     Controls.Title { text: "Fruit sweetness on the rise" }
 *     Controls.Heading { text: "Apples in the sunlight"; level: 2 }
 *     Controls.Paragraph { text: "Long text about fruit and apples [...]" }
 *   [...]
 * }
 * @endcode
 *
 * The most important property is "text", which applies to the text property of
 * Label. See QtQuick Controls Label and primitive QML Text element API for
 * additional properties, methods and signals.
 */
QtControls.Label {
    id: root

    /**
     * The level determines how big the section header is display, values
     * between 1 (big) and 5 (small) are accepted
     */
    property int level: 1

    property int step: 2

    height: Math.round(paintedHeight * 1.2)
    font.pointSize: headerPointSize(level)
    font.weight: Font.Light
    wrapMode: Text.WordWrap
    opacity: 0.8

    function headerPointSize(l) {
        var n = Theme.defaultFont.pointSize;
        var s;
        if (l > 4)
            s = n
        else if (l < 2)
            s = n + (5*step)
        else
            s = n + ((5-level)*2)
        return s;
    }
}
