/****************************************************************************
**
** Copyright 2021 Lutz Schönemann <https://github.com/schoenemann>
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#include "unixsignalbridge.h"

#include <QSocketNotifier>
#include <QLoggingCategory>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>

static const QLoggingCategory logcat("UnixSignalBridge");

int UnixSignalBridge::unixSignalFd[2] = {-1, -1};

void UnixSignalBridge::unixSignalHandler(int signalNo)
{
    ::write(unixSignalFd[0], &signalNo, sizeof(signalNo));
}

UnixSignalBridge::UnixSignalBridge(QList<int> unixSignals, QObject *parent) : QObject(parent)
{
    if (unixSignalFd[0] != -1 || unixSignalFd[1] != -1) {
        qFatal("only one instance of UnixSignalBridge may exist");
    }

    // create a socket pair to forward signal numbers into Qt code
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, unixSignalFd)) {
        qFatal ("Couldn't create HUP socketpair");
    }
    auto sn = new QSocketNotifier(unixSignalFd[1], QSocketNotifier::Read, this);
    connect(sn, &QSocketNotifier::activated, this, &UnixSignalBridge::propagateSignal);

    // setup unix signal handlers
    struct sigaction sigAction;
    sigAction.sa_handler = unixSignalHandler;
    sigAction.sa_flags = SA_RESTART;

    for (auto sig : unixSignals) {
        if (!::sigaction(sig, &sigAction, 0)) {
            qCCritical(logcat) << "failed to register signal action for signal" << sig;
        }
    }
}

void UnixSignalBridge::propagateSignal()
{
    int signalNo = 0;
    auto s = ::read(unixSignalFd[1], &signalNo, sizeof(signalNo));
    if (static_cast<unsigned long>(s) < sizeof(signalNo)) {
        qCCritical(logcat) << "failed to read signal number from socket";
        return;
    }

    qCDebug(logcat) << "received signal" << signalNo;

    emit receivedSignal(signalNo);
    switch (signalNo) {
    case SIGHUP:
        emit sighup();
        break;
    case SIGINT:
        emit sigint();
        break;
    case SIGQUIT:
        emit sigquit();
        break;
    case SIGABRT:
        emit sigabrt();
        break;
    case SIGUSR1:
        emit sigusr1();
        break;
    case SIGUSR2:
        emit sigusr2();
        break;
    case SIGALRM:
        emit sigalrm();
        break;
    case SIGTERM:
        emit sigterm();
        break;
    }
}
