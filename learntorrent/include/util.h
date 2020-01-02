#ifndef UTIL_H__
#define UTIL_H__
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/io_service.hpp>
#include <libtorrent/address.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/socket.hpp>
#include <libtorrent/aux_/time.hpp>
#include <boost/detail/atomic_count.hpp>
#include <memory>
#include <functional>
#include <iostream>

struct BaseServer
{ 
    libtorrent::io_service ios_;
    boost::detail::atomic_count requests_;   
    int port_; 
    std::shared_ptr<libtorrent::thread> thread_;
    
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
