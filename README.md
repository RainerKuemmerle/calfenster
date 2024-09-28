# calfenster

[![CI](https://github.com/RainerKuemmerle/calfenster/actions/workflows/ci.yml/badge.svg)](https://github.com/RainerKuemmerle/calfenster/actions/workflows/ci.yml) ![GitHub License](https://img.shields.io/github/license/RainerKuemmerle/calfenster) ![GitHub top language](https://img.shields.io/github/languages/top/RainerKuemmerle/calfenster) [![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)

calfenster displays a simple calendar widget. It's primary use case is integration into a taskbar launcher to quickly see a calendar widget. The widget shown is Qt's [QCalendarWidget](https://doc.qt.io/qt-6/qcalendarwidget.html).

Currently, calfenster is only tested on Linux but might run also on other platforms.

Look & feel can be adjusted by Qt theme, for example, using qt5ct / qt6ct depending on the Qt version of choice.

| Light Theme | Dark Theme |
| ------------- | ------------- |
| <img src="doc/calfenster-light.png"/> | <img src="doc/calfenster-dark.png"/> |

## Usage

```
$ ./calfenster -h
Usage: ./calfenster [options] command
A simple calender widget.

Options:
  -h, --help  Displays help on commandline options.
  --help-all  Displays help including Qt specific options.

Arguments:
  command     prev | next: Show previous or next month
```

`calfenster` upon starting shows _today_ as the highlighted date. Launching a second time will terminate the previously started instance by default. If valid commands are specified, `calfenster` will interact with the previously started instance.

## Configuration

calfenster can be configured using a configuration file. The file is located in `~/.config/calfenster/calfenster.conf`. If not found, fallback to `/etc/xdg/` instead of `$HOME`. See [QSettings](https://doc.qt.io/qt-6/qsettings.html#locations-where-application-settings-are-stored) for further and platform specific details. If none found a default configuration will be stored.

### Example

Below an example configuration file featuring the default settings.

```INI
[General]
customize_window=true
frameless_window=true
locale=de_DE
skip_task_bar=true
window_no_shadow=false
window_position=mouse
window_stays_on_bottom=false
window_stays_on_top=true
```

### Options

* `customize_window=<boolean>` defaults to `true`.
  Controls whether the window has the default window title hints.
* `frameless_window=<boolean>` defaults to `true`.
  If activated produces a frameless window.
* `locale=<str>` defaults to `""`, i.e., the system's default locale.
  The entry `locale` allows to override the environment locale. It also determines calendar related settings such as the first day of the week.
* `skip_task_bar=<boolean>`  defaults to `true`.
  Asks the window manager to skip showing the window in the taskbar.
* `window_no_shadow=<boolean>`  defaults to `false`.
  If supported deactivate the window's shadow.
* `window_position=<str>`, defaults to `mouse`.
  If `mouse` positions the window next to the mouse cursor.
* `window_stays_on_bottom=<boolean>`  defaults to `false`.
  The window shall stay on bottom of all windows.
* `window_stays_on_top=<boolean>`  defaults to `true`.
  The window shall stay on top of all windows.

## Compilation

`calfenster` only depends on Qt either in version 5 or 6. In particular, we require the modules core, gui, and network.

On Debian/Ubuntu you can install the required Qt development files with
```
# Qt6
sudo apt install qt6-base-dev qt6-tools-dev
# Qt5
sudo apt install qtbase5-dev qtbase5-dev-tools
```

Building follows the standard pattern of a CMake based project. For example, on Linux with the default Makefile generator.
```
mkdir build
cd build
cmake ..
make
```

You can use `cmake -DCALFENSTER_QTVERSION=5` to use Qt5 instead of the default Qt6.

## Acknowledgements

The functionality of `calfenster` is heavily inspired by [gsimplecal](https://github.com/dmedvinsky/gsimplecal). Basically when I started this project, I wanted a rather similar behavior but a slightly different look&feel.
