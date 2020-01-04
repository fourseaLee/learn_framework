#include "peer.h"
#include <boost/predef/other/endian.h>
using namespace libtorrent;

struct PeerServer: public BaseServer
{
    tcp::acceptor acceptor_;
    PeerServer()
    : acceptor_(ios_)
    {
        error_code ec;
        acceptor_.open(tcp::v4(), ec);
        if (ec)
        {
            std::cerr << "PEER Error opening peer listen socket: \n" << ec.message() << std::endl;
            return;
        }

        acceptor_.bind(tcp::endpoint(address_v4::any(), 0), ec);
        if (ec)
        {
            std::cerr << "PEER Error binding peer socket to port 0: \n" << ec.message() << std::endl;
            return;
        }

        port_ = acceptor_.local_endpoint(ec).port();
        if (ec)
        {
            std::cerr << "PEER Error getting local endpoint of peer socket: \n" << ec.message() << std::endl;
            return;
        }

        acceptor_.listen(10, ec);
        if (ec)
        {
            std::cerr << "PEER Error listening on peer socket: \n" << ec.message() << std::endl;
        }

        std::cout << time_now_string() << ": PEER peer initialized on port " << port_ << std::endl;
        thread_.reset(new std::thread(std::bind(&PeerServer::threadFun, this)));
    }

    ~PeerServer()
    {
        error_code ignore;
        acceptor_.cancel(ignore);
        acceptor_.close(ignore);
        if (thread_)
            thread_->join();
    }

    static void newConnect(error_code const& ec , error_code* ret, bool* done)
    {
        *ret = ec;
        *done = true;
    }

    void threadFun()
    {
        for(;;)
        {
            error_code ec;
            tcp::endpoint from;
            tcp::socket socket(ios_);
            std::condition_variable cond;
            bool done = false;

            acceptor_.async_accept(socket, from, std::bind(&newConnect, std::placeholders::_1, &ec, &done));
            while(!done)
            {
                ios_.poll_one();
                ios_.reset();

            }

            if (ec == boost::asio::error::operation_aborted ||
                ec == boost::asio::error::bad_descriptor)
                return ;

            if (ec)
            {
                std::cerr << "PEER Error accepting connection on peer socket: " << ec.message() << std::endl;
            }

            std::cout << time_now_string() << ": PEER incoming peer connection" << std::endl;
            ++requests_;
            socket.close(ec);
        }
    }
};


std::shared_ptr<PeerServer> s_peer;
int StartPeer()
{
    s_peer.reset(new PeerServer);
    return s_peer->port();
}


int NumPeerHits()
{
    if (s_peer)
        return s_peer->numHits();

     return 0;
}

void StopPeer()
{
    s_peer.reset();
}
