BIN := integral_2D
SRC := main.cpp

$(BIN): $(SRC)
	g++ -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -o $@ $^

# Usage: make profile
# Usage: make profile xmin=1
# Use the time(1) command rather than the shell's builtin time command.
profile: $(BIN)
	echo $$(date -Iseconds; /usr/bin/time -p ./$(BIN) $(xmin) 2>&1) >> profile.txt 

clean:
	rm -f $(BIN) *~
