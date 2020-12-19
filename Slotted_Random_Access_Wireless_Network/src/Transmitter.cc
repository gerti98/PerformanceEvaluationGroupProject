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
    numPacketDiscardedSignal_ = registerSignal("numPacketDiscardedSignal");
    numPacketCreatedSignal_ = registerSignal("numPacketCreatedSignal");

    /*
     * buffer related variable initialization
     */

    bufferMaxSize = par("bufferSize");

    buffer = std::queue<PacketMsg*>();

    /*
     * back-off related variable initialization
     */
    maxBackoffTime = 2;
    backoffTime = 0;

    /*
     * retrieve the parameter from the .ned file
     */
    sendProbability = par("sendProbability");
    meanInterarrivalTime = par("meanInterarrivalTime");
    numChannels = par("numChannels");

    scheduleNextPacket();
}

void Transmitter::scheduleNextPacket(){
    simtime_t interArrivalTime = exponential(meanInterarrivalTime,0);
    if(par("deterministicInterarrivalTime"))
        interArrivalTime = meanInterarrivalTime;

    simtime_t arrivalTime = interArrivalTime + simTime();
    PacketMsg* pkt = new PacketMsg("Packet");
    pkt->setCreationTime(arrivalTime);

    scheduleAt(arrivalTime, pkt);
}

void Transmitter::handleMessage(cMessage *msg)
{
    /*
     * if the message come from self then it's a message that declare the
     * arrival of a new packet
     */
    if(msg->isSelfMessage())
    {
        handleArrivedPacket(msg);

    }
    else
    {
        handleChannelPacket(msg);
        delete(msg);
    }
}

/*
 * Handle the arrival of a new packet by trying to store it into
 * the buffer. If not possible, the packet is discarded;
 */
void Transmitter::handleArrivedPacket(cMessage* msg){
    PacketMsg* pkt = check_and_cast<PacketMsg*>(msg);
    int id = this->getId();

    //Register creation of the packet
    emit(numPacketCreatedSignal_, 1);

    /*
     * if the buffer is full then the packets is discard
     */
    if(buffer.size() == bufferMaxSize)
    {
        EV << "transmitter " << id << ": arrival packet discarded because the buffer is full" << endl;
        emit(numPacketDiscardedSignal_, 1);
        delete(msg);
    }
    else
    {
        EV << "transmitter " << id << ": arrival packet inserted into the buffer " << endl;
        pkt->setIdTransmitter(id);
        buffer.push(pkt);
    }

    scheduleNextPacket();
}

/**
 * Handle the eventual acknowledgement from the Channel. In case of collision
 * starts the backoff period
 */
void Transmitter::handleChannelPacket(cMessage* msg){
    int id = this->getId();
    /*
     * if the back off time is greater than 0 then the transmitter must
     * do nothing and wait
     */
    if(backoffTime > 0)
    {
        backoffTime--;
    }
    else
    {
        /*
         * when a NACK is received a collision occurred, so the transmitter
         * double his back off time from the previous collision and start counting
         */
        if(strcmp(msg->getName(), "NACK") == 0)
        {
            maxBackoffTime *= 2;
            backoffTime = intuniform(1, maxBackoffTime, 1);
            EV << "transmitter " << id << ": NACK received, back-off time = " << backoffTime << endl;
        }
        else
        {
            /*
             * when an ACK is received there was no collision and the previous packet
             * can be removed from the buffer and the back-off time can be cleared
             */
            if(strcmp(msg->getName(), "ACK") == 0)
            {
                delete(buffer.front());
                buffer.pop();

                maxBackoffTime = 2;
                EV << "transmitter " << id << ": ACK received " << endl;
            }

            /*
             * if an ACK or a TRIGGER packet is received the transmitter check if
             * the buffer is empty and if not it starts with the Bernoullian experiment
             */
            if(buffer.empty() == false && uniform(0.0, 1.0, 3) < sendProbability)
            {
                PacketMsg* pkt = buffer.front();
                pkt->setIdChannel(intuniform(0, numChannels - 1, 2));
                send(pkt->dup(), "out");
                EV << "transmitter " << id << ": packet sent, waiting for answer " << endl;
            }
        }
    }
}


void Transmitter::finish(){
    while(buffer.empty() == false){
        delete(buffer.front());
        buffer.pop();
    }
}

