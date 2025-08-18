#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "crypto_lib.h"

// memory buffer struct for curl
struct Memory {
    char *response;
    size_t size;
};

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;
    return realsize;
}

double fetch_btc_price(void) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    double price = -1;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.kraken.com/0/public/Ticker?pair=XBTUSD");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            cJSON *json = cJSON_Parse(chunk.response);
            if (json) {
                cJSON *result = cJSON_GetObjectItemCaseSensitive(json, "result");
                cJSON *xbtusd = cJSON_GetObjectItemCaseSensitive(result, "XXBTZUSD");
                if (xbtusd) {
                    cJSON *c = cJSON_GetObjectItemCaseSensitive(xbtusd, "c");
                    if (c && cJSON_GetArraySize(c) > 0) {
                        cJSON *price_item = cJSON_GetArrayItem(c, 0);
                        price = atof(price_item->valuestring);
                    }
                }
                cJSON_Delete(json);
            }
        } else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    free(chunk.response);
    curl_global_cleanup();
    return price;
}
