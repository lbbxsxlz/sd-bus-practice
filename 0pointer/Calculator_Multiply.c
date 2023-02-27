#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

int main(int argc, char *argv[]) {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    const char *path;
    int answer;
    int r, x, y;

    if (argc <= 2 )
    {
          fprintf(stderr, "please input x y to multiply \n");
          return -1;
    }
   
    x=atoi(argv[1]);
    y=atoi(argv[2]);

    /* Connect to the session bus */
    r = sd_bus_open_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to user bus: %s\n", strerror(-r));
        goto finish;
    }

   /* Issue the method call and store the respons message in m */
    r = sd_bus_call_method(bus,
                        "net.poettering.Calculator",           /* service to contact */
                        "/net/poettering/Calculator",          /* object path */
                        "net.poettering.Calculator",           /* interface name */
                        "Multiply",                            /* method name */
                        &error,                                /* object to return error in */
                        &m,                                    /* return message on success */
                        "xx",                                  /* input signature */
                        x,                                     /* first argument */
                        y);                                    /* second argument */
    if (r < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
        goto finish;
    }

    /* Parse the response message */
    r = sd_bus_message_read(m, "x", &answer);
    if (r < 0) {
        fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
        goto finish;
    }

    printf("Answer is: %d.\n", answer);

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
