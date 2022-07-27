BIN := integral_2D
SRC := main.cpp

$(BIN): $(SRC)
	g++ -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -o $@ $^

clean:
	rm -f $(BIN) *~
