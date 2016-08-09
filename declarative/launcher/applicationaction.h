/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef APPLICATIONACTION_H
#define APPLICATIONACTION_H

#include <QtCore/QObject>

class ApplicationActionPrivate;

class ApplicationAction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(QString command READ command CONSTANT)
    Q_DECLARE_PRIVATE(ApplicationAction)
public:
    ApplicationAction(const QString &name, const QString &iconName,
                      const QString &command, QObject *parent = 0);
    ~ApplicationAction();

    /*!
     * \brief Action name.
     *
     * Returns the action name.
     */
    QString name() const;

    /*!
     * \brief Icon name.
     *
     * Returns the icon name.
     */
    QString iconName() const;

    /*!
     * \brief Action command.
     *
     * Returns the command to execute this action.
     */
    QString command() const;

private:
    ApplicationActionPrivate *const d_ptr;
};

#endif // APPLICATIONACTION_H
