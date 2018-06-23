/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H

#include <QDBusContext>
#include <QDBusMessage>
#include <QImage>
#include <QLoggingCategory>
#include <QQmlParserStatus>

QT_BEGIN_NAMESPACE

class QQuickItem;
class QWaylandCompositor;

QT_END_NAMESPACE

class ScreenshooterAdaptor;
class Screenshot;

Q_DECLARE_LOGGING_CATEGORY(lcScreenshooter)

class Screenshooter : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor WRITE setCompositor NOTIFY compositorChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit Screenshooter(QObject *parent = nullptr);
    ~Screenshooter();

    QWaylandCompositor *compositor() const;
    void setCompositor(QWaylandCompositor *compositor);

protected:
    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void compositorChanged();
    void captureRequested(Screenshot *screenshot);

private:
    friend class ScreenshooterAdaptor;

    QWaylandCompositor *m_compositor = nullptr;
    ScreenshooterAdaptor *m_adaptor = nullptr;
};

class ScreenshooterAdaptor : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.Screenshooter")
public:
    explicit ScreenshooterAdaptor(QObject *parent = nullptr);
    ~ScreenshooterAdaptor();

    Q_SCRIPTABLE QByteArray captureScreens(bool overlayCursor);
    Q_SCRIPTABLE QByteArray captureActiveWindow(bool overlayCursor,
                                                bool windowBorder);
    Q_SCRIPTABLE QByteArray captureWindow(bool overlayCursor,
                                          bool windowBorder);
    Q_SCRIPTABLE QByteArray captureArea();

private:
    Screenshooter *m_parent = nullptr;
};

class Screenshot : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CaptureType captureType READ captureType CONSTANT)
    Q_PROPERTY(bool overlayCursor READ overlayCursor CONSTANT)
    Q_PROPERTY(bool keepWindowBorder READ keepWindowBorder CONSTANT)
public:
    enum CaptureType {
        CaptureScreens,
        CaptureActiveWindow,
        CaptureWindow,
        CaptureArea
    };
    Q_ENUM(CaptureType)

    CaptureType captureType() const;
    bool overlayCursor() const;
    bool keepWindowBorder() const;

    QImage grabItem(QQuickItem *item);

    Q_INVOKABLE void grabCursorItem(QQuickItem *item);

    Q_INVOKABLE void grabScreens();
    Q_INVOKABLE void grabSurfaceItem(QQuickItem *item);

    Q_INVOKABLE void save();

private:
    friend class ScreenshooterAdaptor;

    explicit Screenshot(CaptureType captureType, const QDBusMessage &reply);

    void composeCursorImage(QImage &finalImage, const QPointF &position, const QImage &cursorImage);

    CaptureType m_captureType;
    QDBusMessage m_reply;
    QWaylandCompositor *m_compositor = nullptr;
    bool m_overlayCursor = false;
    bool m_windowBorder = false;
    QPointF m_cursorPosition;
    QImage m_cursorImage;
    QImage m_finalImage;
};

#endif // SCREENSHOTER_H
