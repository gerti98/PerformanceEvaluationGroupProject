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

#include "Transmitter.h"

Define_Module(Transmitter);

void Transmitter::initialize()
{
    bufferSize = par("bufferSize");
    overflowPercentageSignal_ = registerSignal("overflowPercentageSignal");
    scheduleNextPacket();
}

void Transmitter::scheduleNextPacket(){
    double mean = par("meanInterarrivalTime");
    simtime_t arrivalTime = exponential(mean) + simTime();
    PacketMsg* pkt = new PacketMsg("Packet");
    pkt->setArrivalTime(arrivalTime);

    //TODO handle channel choice
    //pkt->setIdChannel(var);
    scheduleAt(arrivalTime, pkt);
}


void Transmitter::scheduleNextTimeSlot(){
    //Obtain clock parameter

    //scheduleAt()

    // TODO - Generated method body
}

void Transmitter::handleMessage(cMessage *msg)
{

    // TODO - Generated method body
    if(msg->isSelfMessage()){
        //handleArrivedPacket(msg);

    } else {
        //handleChannelMsg(msg);
    }
}

/*
 * Handle the arrival of a new packet by trying to store it into
 * the buffer. If not possible, the packet is discarded;
 */
void Transmitter::handleArrivedPacket(cMessage* msg){
    PacketMsg* pkt = check_and_cast<PacketMsg*>(msg);

    // TODO - body
}

/**
 * Handle the eventual acknowledgement from the Channel. In case of collision
 * starts the backoff period
 */
void Transmitter::handleChannelMsg(cMessage* msg){
    // TODO - Generated method body
}

void Transmitter::handleTimeSlotMsg(cMessage* msg){
    // TODO - Generated method body
}
