/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  David Edmundson <david@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef PENDINGSESSION_H
#define PENDINGSESSION_H

#include <QObject>
#include <QSharedPointer>

#include "enhancedqdbusabstractinterface.h"

#include "qlogind_export.h"

/**
 * PendingInterface is a KJob like API for loading interfaces
 * when the interface is loaded all initial properties are loaded
 *
 * PendingInterface will delete itself when finished
 */

class QLOGIND_EXPORT PendingInterfaceInternal: public QObject
{
    Q_OBJECT

//     QDBusError error() const;
//     void setError(QDBusError);

Q_SIGNALS:
    void finished(PendingInterfaceInternal*);

protected Q_SLOTS:
    void loadFinished();

protected:
    PendingInterfaceInternal();
    void loadInterface(QSharedPointer< EnhancedQDBusAbstractInterface > interface);
};

// moc can't handle Template classes
// so it's split to mkae moc happy

//template stuff is inline as otherwise it's hard to compile implementations

template <class T> class QLOGIND_EXPORT PendingInterface : public PendingInterfaceInternal
{
public:
    bool isValid() const
    {
        return !m_interface.isNull();
    };

    QSharedPointer<T> interface() const
    {
        return m_interface;
    };

    void setInterface(const QSharedPointer<T> &interface)
    {
        m_interface = interface;
        loadInterface(m_interface);
    };

private:
    QSharedPointer<T> m_interface;
};


/**
 * Takes a list of of PendingInterface classes and waits for them all to be ready.
 * All interfaces from each PendingInterface are then exposed as m_interfaces
 */
template <class T> class QLOGIND_EXPORT PendingInterfaces : public PendingInterfaceInternal
{
public:
    QList<QSharedPointer<T> > interfaces() const
    {
        return m_interfaces;
    };

    void setPendingItems(QList<PendingInterface<T>*> pendingItems)
    {
        m_pendingInterfacesCount = 0;

        foreach(auto pendingItem, pendingItems) {
            m_pendingInterfacesCount++;
            connect(pendingItem, &PendingInterfaceInternal::finished, this, [=]() {
                m_pendingInterfacesCount--;
                m_interfaces << pendingItem->interface();
                if (m_pendingInterfacesCount == 0) {
                    loadFinished();
                }
            });
        }
        if (m_pendingInterfacesCount == 0) {
            loadFinished();
        }
    };


private:
    QList<QSharedPointer<T> > m_interfaces;
    int m_pendingInterfacesCount;
};



//templates don't really work here, as you can't mix QObject and templates nicely
//given we only have 4 classes, just copy and paste

/*
class PendingSession : public PendingInterface
{
    Q_OBJECT
    SessionPtr session() const
    {
        return qSharedPointerCast<Session>(m_interface);
    }

    void emitFinished() {
        emit finished(this);
    }
Q_SIGNALS:
    void finished(PendingSession*);
};


*/

#endif // PENDINGSESSION_H
