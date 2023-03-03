# sd-bus-priactice
sd-bus - A lightweight D-Bus IPC client library
See [here](https://www.freedesktop.org/software/dbus/) for more information about D-Bus IPC.

## How to use sd-bus
These APIs provided in sd-bus.h are implemented as a shared library, which can be compiled and linked to with the libsystemd pkg-config file.

1. #include <systemd/sd-bus.h>
2. pkg-config --cflags --libs libsystemd

## sd-bus demos
### From 0pointer
[Calculator](0pointer/Calculator.c)

[Queue service job](0pointer/Queue_service_job.c)

/* Creater a client to communicate with Calculator service */
[Calculator client](0pointer/Calculator_Multiply.c)

### From Linux Manual
[Print unit path](Linux_manual/print-unit-path.c)

[add object vatable example](Linux_manual/add_object_vtable_example.c)

### my examples
[hello world service](myexample/hello_world_service.c)

[hello world client](myexample/hello_world_client.c)

[pldm get tid example](myexample/pldm)
