#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-bus.h>

static int method_chat(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	const char* input;
	int r;

	/* Read the parameters */
	r = sd_bus_message_read(m, "s", &input);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
		return r;
	}
		
	if (strncmp(input, "Hi", 2) == 0) {
		return sd_bus_reply_method_return(m, "s", "Hi, Nice to meet you!\n");
	} else if (strncmp(input, "who", 3) == 0) {
		return sd_bus_reply_method_return(m, "s", "I am a simple Dbus service !\n");
	}

	/* Reply with the response */
	return sd_bus_reply_method_return(m, "s", "Hello World!\n");
}



/* The vtable of our little object, implements the net.poettering.Calculator interface */
static const sd_bus_vtable chat_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("Chat", "s", "s", method_chat, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};

int main(int argc, char *argv[])
{
	sd_bus_slot *slot = NULL;
	sd_bus *bus = NULL;
	int r;
	
	/* Connect to the user bus this time */
	r = sd_bus_open_user(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to session bus: %s\n", strerror(-r));
		goto finish;
	}
	
	/* Install the object */
	r = sd_bus_add_object_vtable(bus,
					&slot,
					"/org/desktop/lbbxsxlz",	/* object path */
					"org.desktop.lbbxsxlz.Chat",	/* interface name */
					chat_vtable,
					NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to issue method call: %s\n", strerror(-r));
		goto finish;
	}
	
	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(bus, "org.desktop.lbbxsxlz", 0);
	if (r < 0) {
		fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-r));
		goto finish;
	}
	
	for (;;) {
		/* Process requests */
		r = sd_bus_process(bus, NULL);
		if (r < 0) {
			fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
			goto finish;
		}
		if (r > 0) /* we processed a request, try to process another one, right-away */
			continue;
	
		/* Wait for the next request to process */
		r = sd_bus_wait(bus, (uint64_t) -1);
		if (r < 0) {
			fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
			goto finish;
		}
	}

finish:
	sd_bus_slot_unref(slot);
	sd_bus_unref(bus);
	
	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
