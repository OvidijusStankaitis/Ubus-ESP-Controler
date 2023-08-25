#ifndef _UBUS_TLS_H_
#define _UBUS_TLS_H_

#include <libubus.h>

struct device {
    char port[64];
    int vid;
    int pid;
};

extern struct blob_buf b;

void init_ubus_methods(struct ubus_context *ctx);

#endif
