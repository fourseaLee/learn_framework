#include "testingsetup.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(test_io,TestingSetup)

BOOST_AUTO_TEST_CASE(cpp_test)
{
	std::cout << "This is C++  io test!" <<  std::endl;
	std::cout << "begin -------------------" << std::endl;


	std::cout << "end ---------------------" << std::endl;
}

BOOST_AUTO_TEST_CASE(c_test)
{
	printf("This is C io test!\n");
	printf("begin ------------------------\n");
	printf("end --------------------------\n");

}

BOOST_AUTO_TEST_CASE(make_test)
{
	std::cout << "This is make test!" << std::endl;
	std::cout << "begin ---------------------------" << std::endl;
	std::cout << "end -----------------------------" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
