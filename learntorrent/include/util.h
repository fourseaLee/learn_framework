#ifndef UTIL_H__
#define UTIL_H__
//#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/io_service.hpp>
#include <libtorrent/address.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/socket.hpp>
#include <libtorrent/time.hpp>
#include <libtorrent/socket_io.hpp>
#include <atomic>
#include <memory>
#include <functional>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <boost/asio/io_service.hpp>

inline char const* time_now_string()
{
    static const libtorrent::time_point start = libtorrent::clock_type::now();
    static char ret[200];
    int t = int(libtorrent::total_milliseconds(libtorrent::clock_type::now() - start));
    int h = t / 1000 / 60 / 60;
    t -= h * 60 * 60 * 1000;
    int m = t / 1000 / 60;
    t -= m * 60 * 1000;
    int s = t / 1000;
    t -= s * 1000;
    int ms = t;
    std::snprintf(ret, sizeof(ret), "%02d:%02d:%02d.%03d", h, m, s, ms);
    return ret;
}


struct BaseServer
{ 
    libtorrent::io_service ios_;
    std::atomic<int> requests_;   
    int port_; 
    std::shared_ptr<std::thread> thread_;

    BaseServer()
    :requests_(0)
    ,port_(0)
    {
    }

    ~BaseServer()
    {
    }
    int port() const 
    {
        return port_;
    }

    int numHits() const
    {
        return requests_;
    }


};

#endif
