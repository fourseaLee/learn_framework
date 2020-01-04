#include "tracker.h"
#include "util.h"
using namespace libtorrent;
struct UdpTracker:public BaseServer
{
    std::atomic<int> udp_announces_;
    libtorrent::udp::socket socket_;
    bool abort_;

    void onUdpReceive(error_code const& ec, size_t bytes_transferred, udp::endpoint* from, char* buffer, int size)
    {
        if (ec)
        {
            std::cerr << time_now_string() << ": UDP tracker , read failed : " << ec.message() << std::endl;
            return ;
        }

        if (bytes_transferred < 16)
        {
            std::cerr << time_now_string() << ": UDP message too short (from: "/* << print_endpoint(from)*/ <<") \n";
            return ;
        }

        if (abort_)
        {
            return ;
        }

        std::cout << time_now_string() << ": UDP message " << int(bytes_transferred) << "bytes \n";
        char* ptr = buffer;
        detail::read_uint64(ptr);
        boost::uint32_t const action = detail::read_uint32(ptr);
        boost::uint32_t const transaction_id  = detail::read_uint32(ptr);

        error_code e;
        switch (action)
        {
            case 0:
                std::cerr << time_now_string() << ": UDP connect from " /*<< print_endpoint(*from) */<< std::endl;
                ptr = buffer;
                detail::write_uint32(0,ptr);
                detail::write_uint32(transaction_id, ptr);
                detail::write_uint64(10, ptr);
                socket_.send_to(boost::asio::buffer(buffer,16), *from, 0, e);
                if (e)
                    std::cerr << time_now_string() << ": UDP send_to failed. ERROR: " << e.message() << std::endl;
                else
                    std::cout << time_now_string() << ": UDP sent response to: "/* << print_endpoint(from)*/ << std::endl;
                break;
            case 1:
                ++udp_announces_;
                std::cerr << time_now_string() << ": UDP announce  [" << int(udp_announces_) << "]" << std::endl;
                ptr = buffer;
                detail::write_uint32(1, ptr);
                detail::write_uint32(transaction_id, ptr);
                detail::write_uint32(1800, ptr);
                detail::write_uint32(1, ptr);
                detail::write_uint32(1, ptr);

                if (from->address().is_v6())
                {
                    detail::write_uint32(0,ptr);
                    detail::write_uint32(0,ptr);
                    detail::write_uint32(0,ptr);
                    detail::write_uint8(1,ptr);
                    detail::write_uint8(3,ptr);
                    detail::write_uint8(3,ptr);
                    detail::write_uint8(7,ptr);
                    detail::write_uint16(1337,ptr);
                }
                else
                {
                    detail::write_uint8(1,ptr);
                    detail::write_uint8(3,ptr);
                    detail::write_uint8(3,ptr);
                    detail::write_uint8(7,ptr);
                    detail::write_uint16(1337,ptr); 
                }

                socket_.send_to(boost::asio::buffer(buffer,ptr - buffer), *from, 0, e);
                if (e)
                    std::cerr << time_now_string() << ": UDP send_to failed, ERROR: " << e.message() << std::endl;
                else
                    std::cerr << time_now_string() << ": UDP sent response: " /*<< print_endpoint(from)*/ << std::endl;
            case 2:
                std::cerr << time_now_string() << ": UDP scrape (ignored) \n" ;
                break;
            default:
                std::cerr << time_now_string() << "UDP unknow message: " << action << std::endl;
                break;

        }
        socket_.async_receive_from(boost::asio::buffer(buffer, size),*from, 0
                                   ,std::bind(&UdpTracker::onUdpReceive, this, std::placeholders::_1, std::placeholders::_2, from, buffer, size));
    }

    UdpTracker(address iface)
    :udp_announces_(0)
    ,socket_(ios_)
    //,port_(0)
    ,abort_(false)
    {
        port_ = 0;
        error_code ec;
        socket_.open(iface.is_v4() ? udp::v4() : udp::v6(), ec);

        if(ec)
        {
            std::cerr << "UDP error opening listen tracker socket: " << ec.message() << std::endl;
            return;
        }

        socket_.bind(udp::endpoint(iface, 0), ec);
        if(ec)
        {
            std::cerr << "UDP error binding UDP tracker socket to port 0: " << ec.message() << std::endl;
            return;
        }
        
        port_ = socket_.local_endpoint(ec).port();
        if(ec)
        {
            std::cerr << "UDP error getting local endpoint of UDP tracker socket: " << ec.message() << std::endl;
            return;
        }

        std::cout << time_now_string() << ": UDP tracker initialized on port " << port_ << std::endl;
        thread_.reset(new std::thread(std::bind(&UdpTracker::threadFun, this)));
    }

    void stop()
    {
        std::cout << time_now_string() << "UDP tracker, stop \n";
        abort_ = true;
        socket_.cancel();
        socket_.close();
    }

    ~UdpTracker()
    {
        ios_.post(std::bind(&UdpTracker::stop, this));
        if(thread_)
            thread_->join();
    }

    static void incomingPacket(error_code const& ec, size_t bytes_transferred, size_t* ret, error_code* error, bool* done)
    {
        *ret = bytes_transferred;
        *error = ec;
        *done = true;
    }


    void threadFun()
    {
        char buffer[2000];
        error_code ec;
        udp::endpoint from;
        socket_.async_receive_from(
            boost::asio::buffer(buffer,sizeof(buffer)), from, 0,
            std::bind(&UdpTracker::onUdpReceive, this, std::placeholders::_1,std::placeholders::_2, &from, &buffer[0], sizeof(buffer)));
        ios_.run(ec);
        if (ec)
        {
            std::cerr << "UDP Error running UDP tracker services: " << ec.message() << std::endl;
            return;
        }
        
        std::cout << "UDP exiting UDP tracker thread \n" ;
    }
};

std::shared_ptr<UdpTracker> s_udp_tracker;

int StartUdpTracker(address iface)
{
    s_udp_tracker.reset(new UdpTracker(iface));
    return s_udp_tracker->port();
}

int NumUdpAnnounces()
{
    if(s_udp_tracker)
        return s_udp_tracker->numHits();
    return 0;
}

void StopUdpTracker()
{
    s_udp_tracker.reset();
}
