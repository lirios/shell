// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef QUICKOUTPUT_H
#define QUICKOUTPUT_H

#include <QWaylandQuickOutput>

#include "screenmodel.h"

class QuickOutput : public QWaylandQuickOutput
{
    Q_OBJECT
    Q_PROPERTY(ScreenItem *screen READ screen WRITE setScreen NOTIFY screenChanged)
public:
    explicit QuickOutput();

    ScreenItem *screen() const;
    void setScreen(ScreenItem *screen);

Q_SIGNALS:
    void screenChanged(ScreenItem *screen);

protected:
    void componentComplete() override;

private:
    bool m_initialized = false;
    ScreenItem *m_screen = nullptr;

    void addModes();
    void addScreenMode(ScreenMode *mode);

private Q_SLOTS:
    void handleCurrentModeChanged(const QSize &resolution, int refreshRate);
};

#endif // QUICKOUTPUT_H
