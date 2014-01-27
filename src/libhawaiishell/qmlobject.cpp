/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2+$
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlIncubator>

#include "qmlobject.h"
#include "qmlobjectincubationcontroller_p.h"

namespace Hawaii {

namespace Shell {

/*
 * QmlObjectIncubator
 */

class QmlObjectIncubator : public QQmlIncubator
{
public:
    QVariantHash m_initialProperties;

protected:
    virtual void setInitialState(QObject *object)
    {
        QHashIterator<QString, QVariant> i(m_initialProperties);
        while (i.hasNext()) {
            i.next();
            object->setProperty(i.key().toLatin1().data(), i.value());
        }
    }
};

/*
 * QmlObjectPrivate
 */

class QmlObjectPrivate
{
public:
    QmlObjectPrivate(QmlObject *parent)
        : q(parent)
        , engine(0)
        , component(0)
        , delay(false)
    {
        executionEndTimer = new QTimer(q);
        executionEndTimer->setInterval(0);
        executionEndTimer->setSingleShot(true);
        QObject::connect(executionEndTimer, SIGNAL(timeout()),
                         q, SLOT(_q_scheduleExecutionEnd()));
    }

    ~QmlObjectPrivate()
    {
        delete root.data();
    }

    void errorPrint(QQmlComponent *component);
    void execute(const QUrl &source);
    void _q_scheduleExecutionEnd();
    void minimumWidthChanged();
    void minimumHeightChanged();
    void maximumWidthChanged();
    void maximumHeightChanged();
    void preferredWidthChanged();
    void preferredHeightChanged();

    QmlObject *q;

    QUrl source;
    QQmlEngine* engine;
    QmlObjectIncubator incubator;
    QQmlComponent* component;
    QPointer<QObject> root;
    QTimer *executionEndTimer;
    bool delay : 1;
};

void QmlObjectPrivate::errorPrint(QQmlComponent *component)
{
    QString errorStr = QStringLiteral("Error loading QML file.\n");
    if(component->isError()){
        QList<QQmlError> errors = component->errors();
        foreach (const QQmlError &error, errors) {
            errorStr += (error.line()>0?QString(QString::number(error.line()) + QLatin1String(": ")):QLatin1String(""))
                    + error.description() + QLatin1Char('\n');
        }
    }
    qWarning() << component->url().toString() << '\n' << errorStr;
}

void QmlObjectPrivate::execute(const QUrl &source)
{
    if (source.isEmpty())
        return;

    delete component;
    component = new QQmlComponent(engine, q);

    delete root.data();

    component->loadUrl(source);

    if (delay)
        executionEndTimer->start(0);
    else
        _q_scheduleExecutionEnd();
}

void QmlObjectPrivate::_q_scheduleExecutionEnd()
{
    if (component->isReady() || component->isError())
        q->completeInitialization();
    else
        QObject::connect(component, SIGNAL(statusChanged(QQmlComponent::Status)),
                         q, SLOT(completeInitialization()));
}

/*
 * QmlObject
 */

QmlObject::QmlObject(QObject *parent)
    : QObject(parent)
    , d_ptr(new QmlObjectPrivate(this))
{
    Q_D(QmlObject);
    d->engine = new QQmlEngine(this);
    d->engine->setIncubationController(new QmlObjectIncubationController(nullptr));
}

QmlObject::~QmlObject()
{
    delete d_ptr;
}

void QmlObject::setSource(const QUrl &source)
{
    Q_D(QmlObject);
    d->source = source;
    d->execute(source);
}

QUrl QmlObject::source() const
{
    Q_D(const QmlObject);
    return d->source;
}

void QmlObject::setInitializationDelayed(const bool delay)
{
    Q_D(QmlObject);
    d->delay = delay;
}

bool QmlObject::isInitializationDelayed() const
{
    Q_D(const QmlObject);
    return d->delay;
}

QQmlEngine *QmlObject::engine()
{
    Q_D(QmlObject);
    return d->engine;
}

QObject *QmlObject::rootObject() const
{
    Q_D(const QmlObject);
    return d->root.data();
}

QQmlComponent *QmlObject::mainComponent() const
{
    Q_D(const QmlObject);
    return d->component;
}

void QmlObject::completeInitialization(const QVariantHash &initialProperties)
{
    Q_D(QmlObject);

    d->executionEndTimer->stop();

    if (d->root)
        return;

    if (d->component->status() != QQmlComponent::Ready || d->component->isError()) {
        d->errorPrint(d->component);
        return;
    }

    d->incubator.m_initialProperties = initialProperties;
    d->component->create(d->incubator);

    while (!d->incubator.isReady() && d->incubator.status() != QQmlIncubator::Error)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    d->root = d->incubator.object();

    if (!d->root)
        d->errorPrint(d->component);

    emit finished();
}

QObject *QmlObject::createObjectFromSource(const QUrl &source, const QVariantHash &initialProperties)
{
    Q_D(QmlObject);

    QQmlComponent *component = new QQmlComponent(d->engine, this);
    component->loadUrl(source);
    d->incubator.m_initialProperties = initialProperties;
    component->create(d->incubator, d->engine->rootContext());
    while (!d->incubator.isReady() && d->incubator.status() != QQmlIncubator::Error)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    QObject *object = d->incubator.object();

    if (!component->isError() && d->root && object) {
        component->setParent(object);
        object->setProperty("parent", QVariant::fromValue(d->root.data()));
        return object;
    } else {
        d->errorPrint(component);
        delete component;
        delete object;
        return 0;
    }
}

} // namespace Shell

} // namespace Hawaii

#include "moc_qmlobject.cpp"
