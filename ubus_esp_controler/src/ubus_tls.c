#include "ubus_tls.h"
#include "tls.h"
#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>

struct blob_buf b;

static int ubus_method_devices(struct ubus_context *ctx, struct ubus_object *obj,
                            struct ubus_request_data *req, const char *method,
                            struct blob_attr *msg);
static int ubus_method_on(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg);
static int ubus_method_off(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg);

const struct ubus_method ubus_methods[] = {
    { .name = "devices", .handler = ubus_method_devices },
    { .name = "on", .handler = ubus_method_on },
    { .name = "off", .handler = ubus_method_off },
};

struct ubus_object_type device_object_type =
    UBUS_OBJECT_TYPE("esp_device", ubus_methods);

struct ubus_object device_object = {
    .name = "esp_device",
    .type = &device_object_type,
    .methods = ubus_methods,
    .n_methods = ARRAY_SIZE(ubus_methods),
};

void init_ubus_methods(struct ubus_context *ctx) {
    ubus_add_object(ctx, &device_object);
}


static int ubus_method_devices(struct ubus_context *ctx, struct ubus_object *obj,
                            struct ubus_request_data *req, const char *method,
                            struct blob_attr *msg) {
    struct sp_port **ports;
    int i;
    blob_buf_init(&b, 0);

    if (get_esp_devices(&ports) < 0) {
        blobmsg_add_string(&b, "error", "Failed to get device list");
    } else {
        void *array = blobmsg_open_array(&b, "devices");
        
        for (i = 0; ports[i]; i++) {
            struct device dev;
            strcpy(dev.port, sp_get_port_name(ports[i]));
            // Removed the incorrect line
            sp_get_port_usb_vid_pid(ports[i], &dev.vid, &dev.pid);  

            void *table = blobmsg_open_table(&b, NULL);
            blobmsg_add_string(&b, "port", dev.port);
            blobmsg_add_u32(&b, "vendor id", dev.vid);
            blobmsg_add_u32(&b, "product id", dev.pid);
            blobmsg_close_table(&b, table);
        }
        
        blobmsg_close_array(&b, array);
        sp_free_port_list(ports);
    }

    ubus_send_reply(ctx, req, b.head);
    return 0;
}

static int send_command_to_device(const char *port_name, const char *action, int pin) {
    char command[64];
    snprintf(command, sizeof(command), "{\"action\": \"%s\", \"pin\": %d}", action, pin);
    return send_esp_command(port_name, command);
}

const struct blobmsg_policy ubus_method_policy[] = {
    { .name = "port", .type = BLOBMSG_TYPE_STRING },
    { .name = "pin", .type = BLOBMSG_TYPE_INT32 },
};

static int ubus_method_on(struct ubus_context *ctx, struct ubus_object *obj,
                       struct ubus_request_data *req, const char *method,
                       struct blob_attr *msg) {
    struct blob_attr *tb[2];
    const char *port_name;
    int pin;
    
    blobmsg_parse(ubus_method_policy, ARRAY_SIZE(ubus_method_policy), tb, blob_data(msg), blob_len(msg));
    if (!tb[0] || !tb[1]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }
    
    port_name = blobmsg_get_string(tb[0]);
    pin = blobmsg_get_u32(tb[1]);
    
    if (send_command_to_device(port_name, "on", pin) < 0) {
        blobmsg_add_string(&b, "error", "Failed to send ON command");
    } else {
        blobmsg_add_string(&b, "result", "Successfully sent ON command");
    }

    ubus_send_reply(ctx, req, b.head);
    return 0;
}

static int ubus_method_off(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg) {
    struct blob_attr *tb[2];
    const char *port_name;
    int pin;
    
    blobmsg_parse(ubus_method_policy, ARRAY_SIZE(ubus_method_policy), tb, blob_data(msg), blob_len(msg));
    if (!tb[0] || !tb[1]) {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }
    
    port_name = blobmsg_get_string(tb[0]);
    pin = blobmsg_get_u32(tb[1]);
    
    if (send_command_to_device(port_name, "off", pin) < 0) {
        blobmsg_add_string(&b, "error", "Failed to send OFF command");
    } else {
        blobmsg_add_string(&b, "result", "Successfully sent OFF command");
    }

    ubus_send_reply(ctx, req, b.head);
    return 0;
}

