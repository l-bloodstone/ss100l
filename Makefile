CC=gcc

build: main.c
	[[ -d build ]] || mkdir build
	${CC} -g3 -o ./build/ss100l main.c

run: build
	@build/ss100l

clean:
	rm -rf build
