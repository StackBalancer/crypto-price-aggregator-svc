#include <assert.h>
#include <stdio.h>
#include "../src/crypto_lib.h"

int main() {
    printf("Running unit test: fetch_btc_price...\n");
    double price = fetch_btc_price();
    assert(price > 0 && "Price should be positive");
    printf("✔ Test passed, BTC/USD price = %.2f\n", price);
    return 0;
}
