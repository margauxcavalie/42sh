all: 
	meson setup builddir
	ninja -C builddir

unit: all
	ninja -C builddir check_unit

func: all
	ninja -C builddir check

check: func unit

clean:
	rm -r builddir

.PHONY: clean unit all check func
