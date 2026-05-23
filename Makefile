all: build

init:
	git submodule update --init

.PHONY build
build:
	cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release
	cmake --build build/

.PHONY install
install:
	ls

clean:
	rm -rf build/