#include "util.h"
#include <libtorrent/enum_net.hpp>
using namespace libtorrent;
using namespace lt;
int main(int argc, char*argv[])
{
    boost::asio::io_service ios;
    error_code ec;
    address def_gw ;// = get_default_gateway(ios, "", false, ec);
    if (ec)
    {
        std::cout << ec.message() << std::endl;
        return 1;
    }

    return 0;
}
