all: 
	meson setup builddir
	ninja -C builddir

unit:
	ninja -C builddir check_unit

func:
	ninja -C builddir check

check: func unit

clean:
	rm -r builddir

.PHONY: clean unit all check func
