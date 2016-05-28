CPPC=g++
CPPFLAGS=-Wall -lwiringPi -std=gnu++11

all: clean wind_tx23

wind_tx23:
	$(CPPC) $(CPPFLAGS) Wind_TX23.cpp -o Wind_TX23

clean:
	rm -f Wind_TX23


