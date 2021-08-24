// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QQuickItem>

class AbstractTitleBar : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(QQuickItem *dragTarget READ dragTarget WRITE setDragTarget NOTIFY dragTargetChanged)
    Q_PROPERTY(bool moving READ isMoving NOTIFY movingChanged)
public:
    AbstractTitleBar(QQuickItem *parent = nullptr);

    QColor color() const;
    void setColor(const QColor &color);

    QQuickItem *dragTarget() const;
    void setDragTarget(QQuickItem *dragTarget);

    bool isMoving() const;

Q_SIGNALS:
    void colorChanged();
    void dragTargetChanged();
    void movingChanged();
    void activationRequested();
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();
    void windowMenuRequested(const QPointF &position);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private:
    QColor m_color = Qt::transparent;
    QQuickItem *m_dragTarget = nullptr;
    bool m_isMoving = false;
    bool m_initialized = false;
    QPointF m_initialOffset;
};

#endif // TITLEBAR_H
