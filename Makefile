CC = cc
OUT = out
TEST = tests
TESTCASES = dictionaries
CFLAGS = -Wall -Werror -pedantic -O3

all: install
.PHONY: clean test 

tests: mix dictionary
	./$(OUT)/mix
	./$(OUT)/dictionary $(TESTCASES)/top-10k-googled-words.txt
	./$(OUT)/dictionary $(TESTCASES)/http-routes.txt
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
