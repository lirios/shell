/*
 * Unix signal watcher for Qt.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2014 Simon Knopp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "sigwatch.h"

#include <QDebug>
#include <QMap>
#include <QSocketNotifier>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include "sigwatch_p.h"

int UnixSignalWatcherPrivate::sockpair[2];

UnixSignalWatcherPrivate::UnixSignalWatcherPrivate(UnixSignalWatcher *qq)
    : q_ptr(qq)
    , notifier(nullptr)
{
}

void UnixSignalWatcherPrivate::initialize()
{
    Q_Q(UnixSignalWatcher);

    // Create socket pair
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair)) {
        qDebug() << "UnixSignalWatcher: socketpair: " << ::strerror(errno);
        return;
    }

    // Create a notifier for the read end of the pair
    notifier = new QSocketNotifier(sockpair[1], QSocketNotifier::Read, q);
    QObject::connect(notifier, SIGNAL(activated(int)), q, SLOT(_q_handleNotify(int)));
    notifier->setEnabled(true);
}

/*!
 * Registers a handler for the given Unix \a signal. The handler will write to
 * a socket pair, the other end of which is connected to a QSocketNotifier.
 * This provides a way to break out of the asynchronous context from which the
 * signal handler is called and back into the Qt event loop.
 */
void UnixSignalWatcherPrivate::watchForSignal(int signal)
{
    if (watchedSignals.contains(signal)) {
        qDebug() << "Already watching for signal" << signal;
        return;
    }

    // Register a sigaction which will write to the socket pair
    struct sigaction sigact;
    sigact.sa_handler = UnixSignalWatcherPrivate::signalHandler;
    ::sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    if (::sigaction(signal, &sigact, NULL)) {
        qDebug() << "UnixSignalWatcher: sigaction: " << ::strerror(errno);
        return;
    }

    watchedSignals.append(signal);
}

/*!
 * Called when a Unix \a signal is received. Write to the socket to wake up the
 * QSocketNotifier.
 */
void UnixSignalWatcherPrivate::signalHandler(int signal)
{
    ssize_t nBytes = ::write(sockpair[0], &signal, sizeof(signal));
    Q_UNUSED(nBytes);
}

/*!
 * Called when the signal handler has written to the socket pair. Emits the Unix
 * signal as a Qt signal.
 */
void UnixSignalWatcherPrivate::_q_handleNotify(int sockfd)
{
    Q_Q(UnixSignalWatcher);

    int signal;
    ssize_t nBytes = ::read(sockfd, &signal, sizeof(signal));
    Q_UNUSED(nBytes);
    qDebug() << "Caught signal:" << ::strsignal(signal);
    emit q->unixSignal(signal);
}

/*!
 * Create a new UnixSignalWatcher as a child of the given \a parent.
 */
UnixSignalWatcher::UnixSignalWatcher(QObject *parent)
    : QObject(parent)
    , d_ptr(new UnixSignalWatcherPrivate(this))
{
    d_func()->initialize();
}

/*!
 * Destroy the UnixSignalWatcher objects.
 */
UnixSignalWatcher::~UnixSignalWatcher()
{
    delete d_ptr;
}

/*!
 * Register a signal handler for the given \a signal.
 *
 * After calling this method you can \c connect() to the unixSignal() Qt signal
 * to be notified when the Unix signal is received.
 */
void UnixSignalWatcher::watchForSignal(int signal)
{
    Q_D(UnixSignalWatcher);
    d->watchForSignal(signal);
}

/*!
 * \fn void UnixSignalWatcher::unixSignal(int signal)
 * Emitted when the given Unix \a signal is received.
 *
 * watchForSignal() must be called for each Unix signal that you want to receive
 * via the unixSignal() Qt signal. If a watcher is watching multiple signals,
 * unixSignal() will be emitted whenever *any* of the watched Unix signals are
 * received, and the \a signal argument can be inspected to find out which one
 * was actually received.
 */

#include "moc_sigwatch.cpp"
