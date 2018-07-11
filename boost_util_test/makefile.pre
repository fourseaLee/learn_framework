SRC=./testingsetup.cpp  ./test_for_test.cpp  ./test_io.cpp
INCLUDE= -I./  
LIB=  -lc -lrt -lcurl -lpthread -lboost_unit_test_framework -lboost_program_options -lboost_system
APP= boost_test
CFLAG=-std=c++11
DEBUG=-g
boost_test:
	g++ $(CFLAG) $(DEBUG) $(SRC) $(INCLUDE) -o $(APP) $(LIB)  

clean:
	rm -rf $(APP)
