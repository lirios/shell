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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtScript/QScriptValueIterator>

#include "appinterface.h"
#include "desktopview.h"
#include "panelview.h"
#include "scriptengine.h"
#include "shellmanager.h"

ScriptEngine::ScriptEngine(QObject *parent)
    : QScriptEngine(parent)
{
    AppInterface *appInterface = new AppInterface(this);
    connect(appInterface, SIGNAL(print(QString)),
            this, SIGNAL(print(QString)));
    m_self = newQObject(appInterface, QScriptEngine::QtOwnership,
                        QScriptEngine::ExcludeSuperClassProperties |
                        QScriptEngine::ExcludeSuperClassMethods);
    setupEngine();

    connect(this, &QScriptEngine::signalHandlerException,
            this, &ScriptEngine::exception);
}

bool ScriptEngine::evaluateScript(const QString &script, const QString &fileName)
{
    qDebug() << "Executing" << fileName;

    evaluate(script, fileName);

    if (hasUncaughtException()) {
        QString error = tr("Error: %1 at line %2\n\nBacktrace:\n%3")
                .arg(uncaughtException().toString())
                .arg(QString::number(uncaughtExceptionLineNumber()))
                .arg(uncaughtExceptionBacktrace().join("\n"));
        Q_EMIT printError(error);
        return false;
    }

    return true;
}

ScriptEngine *ScriptEngine::envFor(QScriptEngine *engine)
{
    QObject *object = engine->globalObject().toQObject();
    AppInterface *interface = qobject_cast<AppInterface *>(object);
    ScriptEngine *env = qobject_cast<ScriptEngine *>(interface->parent());
    return env;
}

AppInterface *ScriptEngine::interfaceFor(QScriptEngine *engine)
{
    QObject *object = engine->globalObject().toQObject();
    return qobject_cast<AppInterface *>(object);
}

ShellUi *ScriptEngine::uiFor(QScriptEngine *engine)
{
    QObject *object = engine->globalObject().toQObject();
    AppInterface *interface = qobject_cast<AppInterface *>(object);
    ScriptEngine *env = qobject_cast<ScriptEngine *>(interface->parent());
    return qobject_cast<ShellUi *>(env->parent());
}

void ScriptEngine::setupEngine()
{
    QScriptValueIterator it(globalObject());
    while (it.hasNext()) {
        it.next();
        if (it.name() != "print")
            m_self.setProperty(it.name(), it.value());
    }

    m_self.setProperty("Panel", newFunction(ScriptEngine::newPanel, newObject()));
    m_self.setProperty("panels", newFunction(ScriptEngine::panels));

    setGlobalObject(m_self);
}

QScriptValue ScriptEngine::wrapWithScriptOwnership(QObject *object)
{
    QScriptValue value = newQObject(object, QScriptEngine::ScriptOwnership,
                                    QScriptEngine::ExcludeSuperClassProperties |
                                    QScriptEngine::ExcludeSuperClassMethods);
    return value;
}

QScriptValue ScriptEngine::wrapWithQtOwnership(QObject *object)
{
    QScriptValue value = newQObject(object, QScriptEngine::QtOwnership,
                                    QScriptEngine::ExcludeSuperClassProperties |
                                    QScriptEngine::ExcludeSuperClassMethods);
    return value;
}

QScriptValue ScriptEngine::newPanel(QScriptContext *context, QScriptEngine *engine)
{
    QScreen *screen = nullptr;

    if (context->argumentCount() > 0) {
        int desiredScreen = context->argument(0).toInt32();
        int screenNum = qBound<int>(0, desiredScreen, QGuiApplication::screens().size());
        screen = QGuiApplication::screens().at(screenNum);
    } else {
        screen = QGuiApplication::primaryScreen();
    }

    ScriptEngine *env = envFor(engine);
    PanelView *view = new PanelView(ShellManager::instance()->corona(), screen);
    view->show();

    QScriptValue value = env->wrapWithQtOwnership(view);
    return value;
}

QScriptValue ScriptEngine::panels(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(context);

    QScriptValue value = engine->newArray();
    value.setProperty("length", 0);
    return value;
}

void ScriptEngine::exception(const QScriptValue &value)
{
    Q_EMIT printError(value.toString());
}

#include "moc_scriptengine.cpp"
