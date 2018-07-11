#include "testingsetup.h"
#include <boost/test/unit_test.hpp>
#include <iostream>
BOOST_FIXTURE_TEST_SUITE(test_for_test,TestingSetup)

BOOST_AUTO_TEST_CASE(test1)
{
    bool unmutatedMutated = true;
	std::cout << "this is test_for_test" << std::endl;
    BOOST_CHECK(unmutatedMutated == true);

}

BOOST_AUTO_TEST_CASE(test2)
{
	std::cout << "this is test2" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
