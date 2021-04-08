# Unix Signal Bridge for Qt applications

## Usage

```c++
int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    if (QCoreApplication::applicationPid() == 1) {
        // when the application is running in a docker container
        // it's process ID is `1` and we have to handle the Unix
        // signals in our application
        auto unixSignals = new UnixSignalBridge({SIGINT, SIGQUIT, SIGABRT, SIGTERM}, &a);
        QObject::connect(unixSignals, &UnixSignalBridge::sigint, &a, &QCoreApplication::quit);
        QObject::connect(unixSignals, &UnixSignalBridge::sigquit, &a, &QCoreApplication::quit);
        QObject::connect(unixSignals, &UnixSignalBridge::sigabrt, &a, &QCoreApplication::quit);
        QObject::connect(unixSignals, &UnixSignalBridge::sigterm, &a, &QCoreApplication::quit);
    }


    // ...


    return a.exec();
}

```

## License

MIT
