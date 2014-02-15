/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef CORONA_H
#define CORONA_H

#include <QtCore/QObject>
#include <QtQml/QQmlEngine>

#include <HawaiiShell/Package>

namespace Hawaii {

namespace Shell {

class CoronaPrivate;

class HAWAIISHELL_EXPORT Corona : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString shell READ shell NOTIFY shellChanged)
public:
    explicit Corona(QObject *parent = 0);
    ~Corona();

    QQmlEngine *engine() const;

    QString shell() const;

    Package package() const;
    void setPackage(const Package &package);

Q_SIGNALS:
    void shellChanged(const QString &shell);
    void packageChanged(const Package &package);

protected:
    void setShell(const QString &shell);

private:
    Q_DECLARE_PRIVATE(Corona)
    CoronaPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // CORONA_H
