// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INPUTREGION_H
#define INPUTREGION_H

#include <QQmlListProperty>
#include <QQmlParserStatus>
#include <QWindow>

class Rect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
public:
    Rect(QObject *parent = nullptr);

    qreal x() const;
    void setX(qreal x);

    qreal y() const;
    void setY(qreal y);

    qreal width() const;
    void setWidth(qreal width);

    qreal height() const;
    void setHeight(qreal height);

    QRectF rect() const;

Q_SIGNALS:
    void xChanged(qreal x);
    void yChanged(qreal y);
    void widthChanged(qreal width);
    void heightChanged(qreal height);

private:
    QRectF m_rect;
};

class InputRegion : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(QQmlListProperty<Rect> rects READ rects)
    Q_INTERFACES(QQmlParserStatus)
public:
    InputRegion(QObject *parent = nullptr);

    QWindow *window() const;
    void setWindow(QWindow *window);

    QQmlListProperty<Rect> rects();

protected:
    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void windowChanged(QWindow *window);

private:
    bool m_initialized = false;
    QWindow *m_window = nullptr;
    QList<Rect *> m_rects;

    void setInputRegion();

    static void appendRect(QQmlListProperty<Rect> *list, Rect *r);
    static int rectsCount(QQmlListProperty<Rect> *list);
    static Rect *rectAt(QQmlListProperty<Rect> *list, int index);
    static void clearRects(QQmlListProperty<Rect> *list);
    static void replaceRect(QQmlListProperty<Rect> *list, int index, Rect *r);
    static void removeLastRect(QQmlListProperty<Rect> *list);
};

#endif // INPUTREGION_H
