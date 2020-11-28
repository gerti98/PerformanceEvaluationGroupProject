//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SLOTTED_RANDOM_ACCESS_WIRELESS_NETWORK_CHANNEL_H_
#define __SLOTTED_RANDOM_ACCESS_WIRELESS_NETWORK_CHANNEL_H_

#include <omnetpp.h>
#include "PacketMsg_m.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Channel : public cSimpleModule
{
private:
    // signal to record throughput
    simsignal_t throughputSignal_;
    // isCollided_[i] is true if there is a collision on channel i, otherwise is false
    std::vector<bool> isCollided_;
    // packetsOfslot_ contains all the packet for a specific slot
    std::vector<PacketMsg*> packetsOfSlot_;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void scheduleTimeSlot();
    virtual void triggerOthers(std::vector<int> triggeredChannels);
    virtual void findCollisions();
    virtual void transmission();
};

#endif
