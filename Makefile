CC = clang
OUT = out
TEST = tests
TESTCASES = dictionaries
CFLAGS = -Wall -Werror -pedantic -O3 -march=native -flto -funroll-loops -fstrict-aliasing -fomit-frame-pointer -fno-exceptions

all: install
.PHONY: clean test 

repl: $(TEST)/repl.c pihash.c
	mkdir -p $(OUT)
	$(CC) $(CFLAGS) -o $(OUT)/$@ $^
	@echo "REPL generated in" $(OUT) "folder."

tests: test_mix test_top10k test_mit10k test_100k 

# Testcases
test_mix: mix
	./$(OUT)/$^

test_top10k: dictionary 
	./$(OUT)/dictionary $(TESTCASES)/top-10k-googled-words.txt

test_mit10k: dictionary
	./$(OUT)/dictionary $(TESTCASES)/mit-1000.txt

test_100k: dictionary
	./$(OUT)/dictionary $(TESTCASES)/ignis-100k.txt

test_wordlist: dictionary 
	./$(OUT)/dictionary $(TESTCASES)/wordlist.txt

# Will include when dictionary test is mor optimized
# test_million: dictionary
# 	./$(OUT)/dictionary $(TESTCASES)/million.txt

mix: $(TEST)/mix.c pihash.c
	mkdir -p $(OUT)/
	$(CC) -D__AH1_DEBUG__ $(CFLAGS) -o $(OUT)/$@ $^

dictionary: $(TEST)/dictionary.c pihash.c
	mkdir -p $(OUT)/
	$(CC) -D__AH1_DEBUG__ $(CFLAGS) -o $(OUT)/$@ $^

install: libpihash.so
	cp ./hash.h /usr/include/AH1.h
	cp ./libAH1.so /usr/lib

libAH1.so: hash.c
	$(CC) $(CFLAGS) -o $@ -shared -fPIC $^

clean:
	rm -f libAH1.so
	rm -rf $(OUT)/
