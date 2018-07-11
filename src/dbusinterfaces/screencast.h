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

#ifndef SCREENCAST_H
#define SCREENCAST_H

#include <QObject>
#include <QLoggingCategory>
#include <QQmlParserStatus>

QT_BEGIN_NAMESPACE

class QWaylandCompositor;
class QScreen;

QT_END_NAMESPACE

class ScreenCastAdaptor;
#ifdef SCREENCAST_ENABLED
class ScreenCastStream;
#endif

Q_DECLARE_LOGGING_CATEGORY(lcScreenCast)

class ScreenCast : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor WRITE setCompositor NOTIFY compositorChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit ScreenCast(QObject *parent = nullptr);
    ~ScreenCast();

    QWaylandCompositor *compositor() const;
    void setCompositor(QWaylandCompositor *compositor);

    bool createStream(QScreen *screen);

Q_SIGNALS:
    void compositorChanged();

protected:
#ifdef SCREENCAST_ENABLED
    bool eventFilter(QObject *obj, QEvent *event) override;
#endif

    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void streamReady(quint32 nodeId);
    void startStreaming();
    void stopStreaming();

private:
    ScreenCastAdaptor *m_adaptor = nullptr;
#ifdef SCREENCAST_ENABLED
    ScreenCastStream *m_stream = nullptr;
#endif
    QWaylandCompositor *m_compositor = nullptr;
    bool m_streamingEnabled = false;
};

#endif // SCREENCAST_H
