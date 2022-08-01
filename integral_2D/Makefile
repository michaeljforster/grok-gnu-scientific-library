BIN := integral_2D

# Source files
SRC := main.cpp
SRC += quad2d.cpp

# Source and header files
DEPS := $(SRC)
DEPS += quad2d.h

# See https://www.gnu.org/software/gsl/doc/html/usage.html

$(BIN): $(DEPS)
	g++ -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -o $@ $(SRC)

clean:
	rm -f $(BIN) *~
