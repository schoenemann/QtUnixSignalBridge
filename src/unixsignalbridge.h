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

#ifndef UNIXSIGNALBRIDGE_H
#define UNIXSIGNALBRIDGE_H

#include <QObject>

class UnixSignalBridge : public QObject
{
    Q_OBJECT
    static int unixSignalFd[2];
    static void unixSignalHandler(int signalNo);

public:
    explicit UnixSignalBridge(QList<int> unixSignals, QObject *parent = nullptr);

private slots:
    void propagateSignal();

signals:
    void receivedSignal(int signalNo);

    void sighup();
    void sigint();
    void sigquit();
    void sigabrt();
    void sigusr1();
    void sigusr2();
    void sigalrm();
    void sigterm();

};

#endif // UNIXSIGNALBRIDGE_H
