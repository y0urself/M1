all:
	cd Server ; make
	cd Client ; make

clean:
	cd Server ; make clean
	cd Client ; make clean

.phony: all clean