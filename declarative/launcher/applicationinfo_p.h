/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef APPLICATIONINFO_P_H
#define APPLICATIONINFO_P_H

#include <QtCore/QString>
#include <QtCore/QSettings>

//  W A R N I N G
//  -------------
//
// This file is not part of the Green Island API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

class ApplicationAction;
class ApplicationInfo;

class ApplicationInfoPrivate
{
    Q_DECLARE_PUBLIC(ApplicationInfo)
public:
    ApplicationInfoPrivate(const QString &_appId, ApplicationInfo *parent);
    ~ApplicationInfoPrivate();

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    QVariant localizedValue(const QString &key, const QVariant &defaultValue = QVariant()) const;

    void setState(ApplicationInfo::State value);

    QString appId;
    ApplicationInfo::State state;
    QString fileName;
    QSettings *entry;
    bool focused;
    QList<ApplicationAction *> actions;

protected:
    ApplicationInfo *q_ptr;

private:
    void retrieveActions();
};

#endif // APPLICATIONINFO_P_H

