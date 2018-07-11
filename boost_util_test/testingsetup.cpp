#define BOOST_TEST_MODULE boost_test_test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include "testingsetup.h"
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <string>
#include <iostream>


BasicTestingSetup::BasicTestingSetup()
{

}

BasicTestingSetup::~BasicTestingSetup()
{

}


TestingSetup::TestingSetup()
{

}

TestingSetup::~TestingSetup()
{

}
void Shutdown(void *parg) { exit(0); }
void StartShutdown() { exit(0); }
bool ShutdownRequested() { return false; }
using namespace   boost::program_options;

struct StartupShutdown
{
    StartupShutdown()
    {
//
//        boost::program_options::options_description optDef("Options");
//        optDef.add_options()("testhelp", "program options information")
//                ("log_level", "set boost logging (all, test_suite, message, warning, error, ...)")
//                ("log_bitcoin", boost::program_options::value<std::string>()->required(), "test logging destination (console, none)");
//        boost::program_options::variables_map opts;
//        boost::program_options::store(boost::program_options::parse_command_line
//            (boost::unit_test::framework::master_test_suite().argc,
//            boost::unit_test::framework::master_test_suite().argv, optDef),opts);
//
        options_description optDef("Options");
               optDef.add_options()("testhelp", "program options information")(
                   "log_level", "set boost logging (all, test_suite, message, warning, error, ...)")(
                   "log_bitcoin", value<std::string>()->required(), "test logging destination (console, none)");
               variables_map opts;
               store(parse_command_line(boost::unit_test::framework::master_test_suite().argc,
                         boost::unit_test::framework::master_test_suite().argv, optDef),
                   opts);


        if (opts.count("testhelp"))
        {
            std::cout << optDef << std::endl;
            exit(0);
        }

        if (opts.count("log_bitcoin"))
        {
            std::string s = opts["log_bitcoin"].as<std::string>();
        }
    }

    ~StartupShutdown()
    {

    }
};

BOOST_GLOBAL_FIXTURE(StartupShutdown);
