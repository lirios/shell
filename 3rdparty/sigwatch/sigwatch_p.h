#pragma once

#include "sigwatch.h"

/*!
 * \brief The UnixSignalWatcherPrivate class implements the back-end signal
 * handling for the UnixSignalWatcher.
 *
 * \see http://qt-project.org/doc/qt-5.0/qtdoc/unix-signals.html
 */
class UnixSignalWatcherPrivate
{
    Q_DECLARE_PUBLIC(UnixSignalWatcher)
public:
    UnixSignalWatcherPrivate(UnixSignalWatcher *qq);

    void initialize();

    void watchForSignal(int signal);
    static void signalHandler(int signal);

    void _q_handleNotify(int sockfd);

protected:
    UnixSignalWatcher *q_ptr;

private:
    static int sockpair[2];
    QSocketNotifier *notifier;
    QList<int> watchedSignals;
};
