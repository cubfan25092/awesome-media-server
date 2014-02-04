CXXFLAGS=-O2 -std=c++11
LDFLAGS=-lpthread -lboost_locale
CXX=g++

all: obj

tests: writer_obj
	$(CXX) $(CXXFLAGS) -c tests/writer_tests.cpp

	$(CXX) $(LDFLAGS) writer_tests.o writer.o -o tests/writer.tests

obj: writer_obj

writer_obj:
	$(CXX) $(CXXFLAGS) -c writer.cpp

clean:
	rm *.o
	
clean_tests: clean
	rm tests/writer.tests
	rm tests/*.txt