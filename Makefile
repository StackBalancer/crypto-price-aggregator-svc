CC = gcc
CFLAGS = -Wall -I/usr/include/cjson
LIBS = -lcurl -lmicrohttpd -lcjson

SRC = src/crypto_lib.c src/crypto_service.c
TEST_SRC = tests/test_crypto_svc.c
BIN = crypto_service
TEST_BIN = test_crypto_svc

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(LIBS)

run: $(BIN)
	./$(BIN)

test: $(SRC) $(TEST_SRC)
	$(CC) $(TEST_SRC) $(SRC) -o $(TEST_BIN) $(CFLAGS) $(LIBS)
	./$(TEST_BIN)

static-check:
	cppcheck --force --enable=all --inconclusive \
	-I/usr/include/cjson \
	-I/usr/include/x86_64-linux-gnu/curl \
	--suppress=missingIncludeSystem \
	src/

clean:
	rm -f $(BIN) $(TEST_BIN)
