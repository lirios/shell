// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WINDOWMASK_H
#define WINDOWMASK_H

#include <QQmlParserStatus>
#include <QWindow>

class WindowMask : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit WindowMask(QObject *parent = nullptr);

    QWindow *window() const;
    void setWindow(QWindow *window);

    Q_INVOKABLE void addRect(const QRect &rect);
    Q_INVOKABLE void removeRect(const QRect &rect);

protected:
    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void windowChanged(QWindow *window);

private:
    bool m_initialized = false;
    QWindow *m_window = nullptr;
    QRegion m_region;
};

#endif // WINDOWMASK_H
