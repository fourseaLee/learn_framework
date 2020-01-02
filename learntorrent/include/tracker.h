#ifndef TRACKER_H__
#define TRACKER_H__
#include "util.h"

int StartUdpTracker(libtorrent::address iface = libtorrent::address_v4::any());

int NumUdpAnnounces();

void StopUdpTracker();
#endif
