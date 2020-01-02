#ifndef PEER_H__
#define PEER_H__
#include "util.h"

//return the port the peer is running on
int StartPeer();

//the number of incoming connections to this peer
int NumPeerHits();

void stop_peer();
#endif
