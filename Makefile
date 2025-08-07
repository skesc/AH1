CC = cc
OUT = out
TEST = tests
TESTCASES = dictionaries
CFLAGS = -Wall -Werror -pedantic -O3

all: install
.PHONY: clean test test_top10k test_http_routes test_one_million

tests: test_mix test_top10k test_http_routes test_one_million

# Testcases
test_mix: mix
	./$(OUT)/$^

test_top10k: dictionary 
	./$(OUT)/dictionary $(TESTCASES)/top-10k-googled-words.txt

test_http_routes: dictionary 
	./$(OUT)/dictionary $(TESTCASES)/http-routes.txt

test_one_million: dictionary
	./$(OUT)/dictionary $(TESTCASES)/one-million-words.txt


mix: $(TEST)/mix.c
	mkdir -p $(OUT)/
	$(CC) -D__AH1_DEBUG__ $(CFLAGS) -o $(OUT)/$@ $^ hash.c

dictionary: $(TEST)/dictionary.c
	mkdir -p $(OUT)/
	$(CC) -D__AH1_DEBUG__ $(CFLAGS) -o $(OUT)/$@ $^ hash.c

install: libAH1.so
	cp ./hash.h /usr/include/AH1.h
	cp ./libAH1.so /usr/lib

libAH1.so: hash.c
	$(CC) $(CFLAGS) -o $@ -shared -fPIC $^

clean:
	rm -f libAH1.so
	rm -rf $(OUT)/
