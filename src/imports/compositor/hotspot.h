// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HOTSPOT_H
#define HOTSPOT_H

#include <QElapsedTimer>
#include <QQuickItem>

class HotSpot : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Qt::Corner corner READ corner WRITE setCorner NOTIFY cornerChanged)
    Q_PROPERTY(quint64 threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(quint64 pushTime READ pushTime WRITE setPushTime NOTIFY pushTimeChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoveredChanged)
public:
    explicit HotSpot(QQuickItem *parent = nullptr);

    Qt::Corner corner() const;
    void setCorner(Qt::Corner corner);

    quint64 threshold() const;
    void setThreshold(quint64 threshold);

    quint64 pushTime() const;
    void setPushTime(quint64 time);

    bool hovered() const;

Q_SIGNALS:
    void cornerChanged();
    void thresholdChanged();
    void pushTimeChanged();
    void hoveredChanged();
    void triggered();

protected:
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    Qt::Corner m_corner;
    quint64 m_threshold;
    quint64 m_pushTime;
    quint64 m_lastTime;
    quint64 m_entered;
    QElapsedTimer m_timer;
    bool m_hovered;
};

#endif // HOTSPOT_H
