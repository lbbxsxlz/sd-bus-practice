#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

int main(int argc, char *argv[])
{
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;
	sd_bus *bus = NULL;
	const char *reply;
	int r;

	/* Connect to the system bus */
	r = sd_bus_open_user(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to ssssion bus: %s\n", strerror(-r));
		goto finish;
	}

	fprintf(stderr, "connect to system bus: %s\n", strerror(-r));
	/* Issue the method call and store the respons message in m */
	r = sd_bus_call_method(bus,
				"org.desktop.lbbxsxlz",				/* service to contact */
				"/org/desktop/lbbxsxlz",			/* object path */
				"org.desktop.lbbxsxlz.Chat",			/* interface name */
				"Chat",						/* method name */
				&error,						/* object to return error in */
				&m,						/* return message on success */
				"s",						/* input signature */
				"Hi");
	if (r < 0) {
		fprintf(stderr, "Failed to issue method call: %s\n", error.message);
		goto finish;
	}

	/* Parse the response message */
	r = sd_bus_message_read(m, "s", &reply);
	if (r < 0) {
		fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
		goto finish;
	}

	printf("%s", reply);

finish:
	sd_bus_error_free(&error);
	sd_bus_message_unref(m);
	sd_bus_unref(bus);

	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
