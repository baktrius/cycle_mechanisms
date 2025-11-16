.PHONY: all tests clean
.PRECIOUS: data/%

all: tests

tests: main
	bash -c "time ./auto_test"

ref/%: main
	@echo preparing $@ ...
	@./main $(subst _, ,$(notdir $@)) > $@

mai%: mai%.cpp lib.h Makefile
	g++ -o $@ $< -g -std=c++23 -Wall -O3

mai%_dbg: mai%.cpp lib.h Makefile
	g++ -o $@ $< -g -std=c++23 -Wall -Werror -O0

# Build WebAssembly module and JS loader together (portable across make versions)
build_wasm.stamp: main.cpp lib.h Makefile
	emcc main.cpp -std=c++26 -o main.js -s MODULARIZE=1 -s 'EXPORT_NAME="createModule"' -O3
	touch $@

main.js main.wasm: build_wasm.stamp

# Convenience target: build and place artifacts into front/public/wasm/
.PHONY: wasm
wasm: main.js main.wasm
	mkdir -p front/public/wasm
	cp -f main.js main.wasm front/public/wasm/
clean:
	rm -f main main_dbg front/public/wasm/main.js front/public/wasm/main.wasm main.js main.wasm build_wasm.stamp
