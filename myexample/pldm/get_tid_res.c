#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include "pldm.h"

sd_bus *bus = NULL;

static int method_GetTID(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
	uint8_t *array;
	size_t array_size;
	int r = 0;
	sd_bus_message *reply = NULL;

	/* Read the parameters */
	r = sd_bus_message_read_array(m, 'y', (const void **)&array, &array_size);
	if (r < 0) {
		fprintf(stderr, "Failed to read the message: %s\n", strerror(-r));
		return r;
	}

	if (array_size < sizeof(pldm_get_tid_request_t)) {
		fprintf(stderr, "The invalid PLDM request size!\n");
		return -1;
	}

	pldm_get_tid_request_t get_tid_req;
	get_tid_req.pldm_header.instance_id = ((pldm_message_header_t *)array)->instance_id;
	get_tid_req.pldm_header.pldm_type = ((pldm_message_header_t *)array)->pldm_type;
	get_tid_req.pldm_header.pldm_command_code = ((pldm_message_header_t *)array)->pldm_command_code;

	printf("Receive the request, array_size = %ld\n", array_size);
	printf("PLDM instance_id = %d\n", get_tid_req.pldm_header.instance_id);
	printf("PLDM pldm_type = %d\n", get_tid_req.pldm_header.pldm_type);
	printf("PLDM pldm_command_code = %d\n", get_tid_req.pldm_header.pldm_command_code);

	pldm_get_tid_response_t get_tid_res;
	get_tid_res.pldm_header.instance_id = get_tid_req.pldm_header.instance_id & PLDM_HEADER_INSTANCE_ID_MASK;
	get_tid_res.pldm_header.pldm_type = get_tid_req.pldm_header.pldm_type;
	get_tid_res.pldm_header.pldm_command_code = get_tid_req.pldm_header.pldm_command_code;

	if (get_tid_res.pldm_header.pldm_type != PLDM_MESSAGE_TYPE_CONTROL_DISCOVERY
			|| get_tid_req.pldm_header.pldm_command_code != PLDM_CONTROL_DISCOVERY_COMMAND_GET_TID) {
		get_tid_res.pldm_response_header.pldm_completion_code = PLDM_BASE_CODE_ERROR;
	} else {
		get_tid_res.pldm_response_header.pldm_completion_code = PLDM_BASE_CODE_SUCCESS;
	}

	get_tid_res.tid = 1;

	array = (uint8_t *)&get_tid_res;
	array_size = sizeof(get_tid_res);

	r = sd_bus_message_new_method_return(m, &reply);
	if (r < 0) {
		fprintf(stderr, "Create the new replay fail: %s\n", strerror(-r));
		return r;
	}

	r = sd_bus_message_append_array(reply, 'y', array, array_size);
	if (r < 0) {
		fprintf(stderr, "Failed to append array: %s\n", strerror(-r));
		return r;
	}

	//return sd_bus_message_send(reply);
	return sd_bus_send(bus, reply, NULL);
}

/* The vtable of our little object, implements the interface */
static const sd_bus_vtable pldm_resp_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("GetTID", "ay", "ay", method_GetTID, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};

int main(int argc, char *argv[]) {
	sd_bus_slot *slot = NULL;
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
					"/openbmc/PLDM",  /* object path */
					"openbmc.PLDM.pldm_rsp",   /* interface name */
					pldm_resp_vtable,
					NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to issue method call: %s\n", strerror(-r));
		goto finish;
	}

	/* Take a well-known service name so that clients can find us */
	r = sd_bus_request_name(bus, "bmc.MCTP.PLDM", 0);
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
