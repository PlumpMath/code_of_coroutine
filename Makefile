all:
	g++ -Wall -O3 infinite.cc -o twist.run
	g++ -Wall -O3 c_implemented_yield_and_send.cc -o yield.run
clean:
	rm -rf *.run
