/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef QMLGSETTINGS_H
#define QMLGSETTINGS_H

#include <QtQml/QQmlPropertyMap>
#include <QtQml/QQmlParserStatus>

namespace Hawaii {
class QGSettings;
}

class QmlGSettingsSchema;

class QmlGSettings : public QQmlPropertyMap, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QmlGSettingsSchema *schema READ schema NOTIFY schemaChanged)
public:
    QmlGSettings(QObject *parent = 0);

    QmlGSettingsSchema *schema() const;

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void schemaChanged();
    void settingsChanged();

protected:
    QVariant updateValue(const QString &key, const QVariant &input) Q_DECL_OVERRIDE;

private:
    QmlGSettingsSchema *m_schema;
    Hawaii::QGSettings *m_settings;
};

#endif // QMLGSETTINGS_H
