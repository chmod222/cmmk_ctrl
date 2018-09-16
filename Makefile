all: libcmmk cmmk_ctrl

.PHONY: libcmmk

libcmmk:
	make -C libcmmk

cmmk_ctrl:
	test -d build || mkdir build
	cd build; qmake PREFIX=$(PREFIX) ..
	make -j -C build

install:
	make -C build install
