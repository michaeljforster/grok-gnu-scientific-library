SRC := main.cpp

DEPS := $(SRC)

# Use the time(1) command rather than the shell's builtin time command.
TIME := /usr/bin/time
PROFILE := profile.txt

integral_2D: $(DEPS)
	g++ -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -o $@ $(SRC)

# Usage: make profile xmin=1
profile: integral_2D
	date >> $(PROFILE)
	$(TIME) ./integral_2D $(xmin) >> $(PROFILE) 2>&1

clean:
	rm -f integral_2D *~
