/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QtCore/QObject>

class ApplicationAction;
class ApplicationInfoPrivate;

class ApplicationInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appId READ appId CONSTANT)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(bool focused READ isFocused NOTIFY focusedChanged)
    Q_PROPERTY(QList<ApplicationAction *> actions READ actions CONSTANT)
    Q_ENUMS(State)
    Q_DECLARE_PRIVATE(ApplicationInfo)
public:
    enum State {
        //! The application is not running
        NotRunning,
        //! The application was launched and it's starting up.
        Starting,
        //! The application is currently running.
        Running,
        //! The application is in the background and it was suspended by
        //! the system to save resources.
        Suspended,
        //! The application is in the background and its activity was
        //! stopped by the system in order to save resources, acts as
        //! being closed but the state is saved on disk and can be
        //! restored.
        Stopped
    };

    ApplicationInfo(const QString &appId, QObject *parent = 0);
    ~ApplicationInfo();

    /*!
     * \brief Application state.
     *
     * Holds the current state of the application.
     */
    State state() const;

    /*!
     * \brief Application identifier.
     *
     * Holds the application identifier according to the Freedesktop
     * Desktop Entry specification.
     */
    QString appId() const;

    /*!
     * \brief Desktop entry file name.
     *
     * Returns the desktop entry file name.
     */
    QString fileName() const;

    /*!
     * \brief Application name.
     *
     * Returns the application name localized to current language.
     * This information is retrieved from the desktop entry.
     */
    QString name() const;

    /*!
     * \brief Application description.
     *
     * Returns the application description localized to current language.
     * This information is retrieved from the desktop entry.
     */
    QString comment() const;

    /*!
     * \brief Application icon.
     *
     * Returns the application main icon from the desktop entry or
     * a custom icon that is later chosen by the application itself
     * in order to indicate the current activity better.
     */
    QString iconName() const;

    /*!
     * \brief Application focus state.
     *
     * Returns whether the application is currently focused or not.
     */
    bool isFocused() const;

    /*!
     * \brief Actions.
     *
     * Holds the list of actions for the quicklist.
     */
    QList<ApplicationAction *> actions() const;

Q_SIGNALS:
    void stateChanged();
    void nameChanged();
    void commentChanged();
    void iconNameChanged();
    void focusedChanged();

private:
    ApplicationInfoPrivate *const d_ptr;
};

#endif // APPLICATIONINFO_H
