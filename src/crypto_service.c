#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <microhttpd.h>
#include <prom.h>
#include "crypto_lib.h"

#define PORT 8080
static int keep_running = 1;

// --- Prometheus metric ---
static prom_counter_t *req_counter;
static prom_gauge_t *btc_gauge;

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

    // --- /metrics endpoint ---
    if (strcmp(url, "/metrics") == 0) {
        const char *metrics = prom_collector_registry_bridge(PROM_COLLECTOR_REGISTRY_DEFAULT);

        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(metrics),
                                                                        (void *)metrics,
                                                                        MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(response, "Content-Type", "text/plain; version=0.0.4");

        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    }

    // --- Root "/" endpoint for BTC price ---
    double price = fetch_btc_price();
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "{\"BTC_USD\": %.2f}", price);

    // Update metrics
    prom_counter_inc(req_counter, NULL);        // total request counter
    prom_gauge_set(btc_gauge, price, NULL);     // current BTC price

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(buffer),
                                                                    (void *)buffer,
                                                                    MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(response, "Content-Type", "application/json");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

#ifndef TEST
int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // --- Initialize Prometheus default registry ---
    if (prom_collector_registry_default_init() != 0) {
        fprintf(stderr, "Failed to init Prometheus registry\n");
        return 1;
    }

    // Register metrics
    req_counter = prom_counter_new("crypto_requests_total",
                                   "Total number of / requests served", 0, NULL);
    btc_gauge = prom_gauge_new("crypto_btc_usd",
                               "Current BTC/USD price", 0, NULL);

    prom_collector_registry_register_metric(req_counter);
    prom_collector_registry_register_metric(btc_gauge);

    // Start HTTP daemon (serves both / and /metrics)
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);

    if (!daemon) {
        fprintf(stderr, "Failed to start HTTP server\n");
        return 1;
    }

    printf("Server running on:\n");
    printf("  API      → http://localhost:%d/\n", PORT);
    printf("  Metrics  → http://localhost:%d/metrics\n", PORT);

    while (keep_running) {
        sleep(1);
    }

    MHD_stop_daemon(daemon);
    printf("Server stopped.\n");
    return 0;
}
#endif
