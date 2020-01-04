#include "dht.h"
#include "util.h"
using namespace libtorrent;
struct DhtServer : public BaseServer
{
    udp::socket socket_;
    
    DhtServer()
    : socket_(ios_)
    {
        error_code ec;
        socket_.open(udp::v4(), ec);
        if (ec)
        {
            std::cerr << "Error opening listent DHT socket: " << ec.message() << std::endl;
            return ;
        }

        socket_.bind(udp::endpoint(address_v4::any(),0),ec);
        if (ec)
        {
            std::cerr << "Error binding DHT socket to  port 0: "  << ec.message() << std::endl;
            return ;
        }

        port_ = socket_.local_endpoint(ec).port();
        if (ec)
        {
            std::cerr << "Error getting local endpoint of DHT socket: " << ec.message() << std::endl;
            return;
        }

        std::cout << time_now_string() << ": DHT initialized on port " << port_ << std::endl;

        thread_.reset(new std::thread(std::bind(&DhtServer::threadFun, this)));
    }

    ~DhtServer()
    {
        socket_.cancel();
        socket_.close();
        if (thread_)
            thread_->join();
    }

    static void incomingPacket(error_code  const &ec, size_t bytes_transferred, size_t *ret, error_code*error, bool* done)
    {
        *ret = bytes_transferred;
        *error = ec;
        *done = true;
    }

    void threadFun()
    {
        char buffer[2000];
        for(;;)
        {
            error_code ec;
            udp::endpoint from;
            size_t bytes_transferred;
            bool done = false;

            socket_.async_receive_from(
            boost::asio::buffer(buffer, sizeof(buffer)), from, 0,
            std::bind(&incomingPacket, std::placeholders::_1, std::placeholders::_2, &bytes_transferred, &ec, &done));

            while(!done)
            {
                ios_.poll_one();
                ios_.reset();
            }

            if (ec)
            {
                std::cerr << "Error recieving on EHT socket: " << ec.message() << std::endl;
                return;
            }

            try
            {
                entry msg = bdecode(buffer, buffer + bytes_transferred);
                std::cerr << msg << std::endl;
                requests_++;
            }
            catch (std::exception& e)
            {
                std::cerr << "failed to decode DHT message: " << e.what() << std::endl;
            }
            
        }
    }
};


std::shared_ptr<DhtServer> s_dht;

int StartDht()
{
    s_dht.reset(new DhtServer);
    return s_dht->port();
}

int NumDhtHits()
{
    if (s_dht)
        return s_dht->numHits();
    return 0;
}


void StopDht()
{
    s_dht.reset();
}
