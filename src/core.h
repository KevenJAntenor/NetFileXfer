#ifndef INF3173_TP1_FILEOPS_H
#define INF3173_TP1_FILEOPS_H

#include <inttypes.h>

#include "xfer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum xfer_srv_status { XFER_SRV_INIT, XFER_SRV_READY, XFER_SRV_STOP };

typedef int (*xfer_status_cb)(int status);

struct xfer_server_state;

int xfer_client(const char *ipaddr, uint16_t port, xfer_function handler,
                void *args);
int xfer_server(const char *ipaddr, uint16_t port, xfer_function handler,
                void *args, xfer_status_cb st);

int xfer_server_init(struct xfer_server_state **state, const char *ipaddr,
                     uint16_t port, xfer_function cb, void *args);
int xfer_server_start(struct xfer_server_state *state);
int xfer_server_stop(struct xfer_server_state *state);
int xfer_server_once(void *args);

#ifdef __cplusplus
}
#endif

#endif
