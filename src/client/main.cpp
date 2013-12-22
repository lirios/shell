/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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

#include <QtCore/QDebug>
#include <QtWidgets/QApplication>

#include "hawaiishell.h"
#include "notificationsdaemon.h"
#include "config.h"
#include "gitsha1.h"

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cxxabi.h>
#include <signal.h>

#include <iostream>

static void printBacktrace()
{
    const size_t max_depth = 100;
    void *stack_addrs[max_depth];

    size_t stack_depth = backtrace(stack_addrs, max_depth);

    char **messages = backtrace_symbols(stack_addrs, stack_depth);

    // skip first stack frame (points here)
    for (size_t i = 1; i < stack_depth && messages != NULL; i++) {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p) {
            if (*p == '(') {
                mangled_name = p;
            } else if (*p == '+') {
                offset_begin = p;
            } else if (*p == ')') {
                offset_end = p;
                break;
            }
        }

        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end &&
                mangled_name < offset_begin) {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';

            int status;
            char *real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

            // if demangling is successful, output the demangled function name
            if (status == 0) {
                std::cerr << "[bt]: (" << i << ") " << messages[i] << " : "
                          << real_name << "+" << offset_begin << offset_end
                          << std::endl;

            }
            // otherwise, output the mangled function name
            else {
                std::cerr << "[bt]: (" << i << ") " << messages[i] << " : "
                          << mangled_name << "+" << offset_begin << offset_end
                          << std::endl;
            }
            free(real_name);
        }
        // otherwise, print the whole line
        else {
            std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
        }
    }
    std::cerr << std::endl;

    free(messages);
}

static void deathSignal(int signum)
{
    // Print the stack trace when a segmentation fault occurs
    printBacktrace();
    signal(signum, SIG_DFL);
    _exit(3);
}

int main(int argc, char *argv[])
{
    // Handle a segmentation fault nicely
    signal(SIGSEGV, deathSignal);

    QApplication app(argc, argv);
    app.setApplicationName("Hawaii Shell");
    app.setApplicationVersion(HAWAII_SHELL_VERSION_STRING);
    app.setOrganizationDomain("hawaii.org");
    app.setOrganizationName("Hawaii");

    // Print version information
    qDebug() << qPrintable(QStringLiteral("== Hawaii Shell v%1 ==\n").arg(HAWAII_SHELL_VERSION_STRING))
             << "** http://www.maui-project.org\n"
             << "** Bug reports to: https://github.com/mauios/hawaii-shell/issues\n"
             << qPrintable(QStringLiteral("** Build: %1-%2")
                           .arg(HAWAII_SHELL_VERSION_STRING).arg(GIT_REV));

    // Create the notification daemon
    (void)NotificationsDaemon::instance();

    // Create the main shell object
    (void)HawaiiShell::instance();

    return app.exec();
}
