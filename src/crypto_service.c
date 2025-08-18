#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <microhttpd.h>
#include "crypto_lib.h"

#define PORT 8080
static int keep_running = 1;

void handle_signal(int sig) {
    keep_running = 0;
}

static enum MHD_Result answer_to_connection(void *cls,
                                struct MHD_Connection *connection,
                                const char *url,
                                const char *method,
                                const char *version,
                                const char *upload_data,
                                size_t *upload_data_size,
                                void **con_cls) {
    if (strcmp(method, "GET") != 0)
        return MHD_NO;

    double price = fetch_btc_price();
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "{\"BTC_USD\": %.2f}", price);

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(buffer),
                                                                    (void *)buffer,
                                                                    MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

#ifndef TEST
int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);

    if (!daemon) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    printf("Server running on http://localhost:%d/ (Ctrl+C or docker stop to exit)\n", PORT);

    while (keep_running) {
        sleep(1);
    }

    MHD_stop_daemon(daemon);
    printf("Server stopped.\n");
    return 0;
}
#endif
