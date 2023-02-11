# sd-bus-priactice
sd-bus - A lightweight D-Bus IPC client library
See [here](https://www.freedesktop.org/software/dbus/) for more information about D-Bus IPC.

## How to use sd-bus
These APIs provided in sd-bus.h are implemented as a shared library, which can be compiled and linked to with the libsystemd pkg-config file.

1. #include <systemd/sd-bus.h>
2. pkg-config --cflags --libs libsystemd

## sd-bus demos
