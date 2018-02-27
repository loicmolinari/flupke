# Quicken

Started as a dump of the interesting features developed for Ubuntu Touch, **Quicken** delivers additional libraries, plugins and tools for Qt Quick. Here's a quick summary of these.

## QuickenMetrics library

QuickenMetrics is a library to track and show real-time performance metrics of Qt Quick applications. The metrics can be displayed per windows, each QQuickWindow having different values, or logged to a file.

For now, there are 3 types of metrics:

- Window metrics, with an id, a geometry and a state.
- Frame metrics, with a window id, a frame number and various values like sync, render and swap times.
- Process metrics, with the virtually allocated memory size, the Resident Set Size, CPU usage and the thread count.

Here is a shot showing the metrics overlay on a QQuickWindow:

![metrics logging image](https://raw.githubusercontent.com/wiki/loicmolinari/quicken/web/quicken-win.png)

And another one showing logging to the standard output:

![metrics overlay image](https://raw.githubusercontent.com/wiki/loicmolinari/quicken/web/quicken-term.png)

## qmlscene extensions

This is Qt's qmlscene, extended with new options:

```
$ qmlscene-quicken --help

 Quicken options:
  --metrics-overlay ................. Enable the metrics overlay on each QQuickWindows.
  --metrics-logging <device> ........ Enable metrics logging. <device> is a file or 'stdout' (an empty
    ................................. <device> means 'stdout').
  --metrics-logging-filter <filter> . Filter logged metrics. <filter> is a list of metrics types (either
    ................................. 'window', 'frame', 'process' or 'generic') separated by commas
    ................................. (for example: 'window' or 'window,process').
  --continuous-updates .............. Continuously update the main window.
  --quit-after-frame-count <count> .. Quit after <count> frames rendered on the main window.
```

Note how `--continuous-updates` and `--quit-after-frame-count` can be used in conjonction with performance metrics logging in order to measure average timings across several frames and get precise rendering times for instance. Such values can be useful in regression tests for instance.

## Supported platforms

Only tested on Linux and Qt 5.10.1 for now. Theoretically builds with Qt 5.6.0. Planning to add Windows support.

Help appreciated!

## Build instructions

Build with:

```
$ cd $SOURCE_DIRECTORY
$ qmake
$ make
```

Install with:

```
$ make install INSTALL_ROOT=$INSTALL_DIRECTORY
```

The Quicken repository provides a script allowing to test the uninstalled compiled binaries:

```
$ qmake
$ ./scripts/shell.sh
[Quicken] $ make
[Quicken] $ ./bin/qmlscene-quicken --metrics-overlay tests/Test1.qml
```
