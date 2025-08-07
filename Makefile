CC = cc
OUT = out
TEST = tests
TESTCASES = dictionaries
CFLAGS = -Wall -Werror -pedantic -O3

all: install
.PHONY: clean test 

repl: $(TEST)/repl.c
	mkdir -p $(OUT)
	$(CC) -lAH1 $(CFLAGS) -o $(OUT)/$@ $^
	@echo "REPL generated in" $(OUT) "folder."

tests: test_mix test_top10k test_mit10k test_wordlist # test_one_million

# Testcases
test_mix: mix
	./$(OUT)/$^

test_top10k: dictionary 
	./$(OUT)/dictionary $(TESTCASES)/top-10k-googled-words.txt

test_mit10k: dictionary
	./$(OUT)/dictionary $(TESTCASES)/mit-1000.txt

test_wordlist: dictionary 
	./$(OUT)/dictionary $(TESTCASES)/wordlist.txt

# test_one_million: dictionary
# 	./$(OUT)/dictionary $(TESTCASES)/one-million-words.txt


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
