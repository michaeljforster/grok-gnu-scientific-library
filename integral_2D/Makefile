SRC := main.cpp

DEPS := $(SRC)

integral_2D: $(DEPS)
	g++ -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -o $@ $(SRC)

# Usage: make profile
# Usage: make profile xmin=1
# Use the time(1) command rather than the shell's builtin time command.
profile: integral_2D
	echo $$(date -Iseconds; /usr/bin/time -p ./integral_2D $(xmin) 2>&1) >> profile.txt 

clean:
	rm -f integral_2D *~
