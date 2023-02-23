#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>
#include "pldm.h"

int main(int argc, char *argv[]) {
	sd_bus_error error = SD_BUS_ERROR_NULL;
	sd_bus_message *m = NULL;
	sd_bus_message *reply = NULL;
	sd_bus *bus = NULL;

	uint8_t *array;
	size_t array_size;
	int r;
	uint8_t instance_id = 0;
	pldm_get_tid_request_t get_tid_req;
	pldm_get_tid_response_t get_tid_res;

	get_tid_req.pldm_header.instance_id = instance_id | PLDM_HEADER_REQUEST_MASK;
	get_tid_req.pldm_header.pldm_type = PLDM_MESSAGE_TYPE_CONTROL_DISCOVERY;
	get_tid_req.pldm_header.pldm_command_code = PLDM_CONTROL_DISCOVERY_COMMAND_GET_TID;

	array = (uint8_t *)&get_tid_req;
	array_size = sizeof(get_tid_req);

	/* Connect to the system bus */
	r = sd_bus_open_user(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to user bus: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_message_new_method_call(bus,
										&m,
										"openbmc.PLDM",				/* service to contact */
										"/openbmc/PLDM",				/* object path */
										"openbmc.PLDM.pldm_rsp",		/* interface name */
										"GetTID");						/* method name */;
	if (r < 0) {
		fprintf(stderr, "Failed to create method call: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_message_append_array(m, 'y', array, array_size);
	if (r < 0) {
		fprintf(stderr, "Failed to append array: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_call(bus, m, -1, &error, &reply);
	if (r < 0) {
		fprintf(stderr, "Failed to send message: %s\n", error.message);
		goto finish;
	}

	r = sd_bus_message_read_array(reply, 'y', (const void **)&array, &array_size);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
		return r;
	}

	printf("Receive the response, array size = %ld.\n", array_size);

	get_tid_res.pldm_header.instance_id = ((pldm_get_tid_response_t *)array)->pldm_header.instance_id;
	get_tid_res.pldm_header.pldm_type = ((pldm_get_tid_response_t *)array)->pldm_header.pldm_type;
	get_tid_res.pldm_header.pldm_command_code = ((pldm_get_tid_response_t *)array)->pldm_header.pldm_command_code;
	get_tid_res.pldm_response_header.pldm_completion_code = ((pldm_get_tid_response_t *)array)->pldm_response_header.pldm_completion_code;
	get_tid_res.tid = ((pldm_get_tid_response_t *)array)->tid;

	printf("PLDM pldm_completion_code = %d.\n", get_tid_res.pldm_response_header.pldm_completion_code);
	printf("PLDM the responder's tid = %d.\n", get_tid_res.tid);

	if (get_tid_res.pldm_response_header.pldm_completion_code == PLDM_BASE_CODE_SUCCESS) {
		printf("PLDM GETTID Successfly!\n");
	}

finish:
	sd_bus_error_free(&error);
	sd_bus_message_unref(m);
	sd_bus_unref(bus);

	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
