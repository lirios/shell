#pragma once

#include <QtCore/private/qobject_p.h>

#include "sigwatch.h"

/*!
 * \brief The UnixSignalWatcherPrivate class implements the back-end signal
 * handling for the UnixSignalWatcher.
 *
 * \see http://qt-project.org/doc/qt-5.0/qtdoc/unix-signals.html
 */
class UnixSignalWatcherPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UnixSignalWatcher)

public:
    UnixSignalWatcherPrivate();

    void watchForSignal(int signal);
    static void signalHandler(int signal);

private Q_SLOTS:
    void onNotify(int sockfd);

private:
    static int sockpair[2];
    QSocketNotifier *notifier;
    QList<int> watchedSignals;
};
