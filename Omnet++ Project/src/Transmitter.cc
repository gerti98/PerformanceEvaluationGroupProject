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
    overflowPercentageSignal_ = registerSignal("overflowPercentageSignal");

    /*
     * buffer related variable initialization
     */

    bufferSize = par("bufferSize");
    bufferSize++;

    buffer = std::vector<PacketMsg*>(bufferSize);
    start_idx = 0;
    end_idx = 0;

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
    simtime_t arrivalTime = exponential(meanInterarrivalTime) + simTime();
    PacketMsg* pkt = new PacketMsg("Packet");
    pkt->setArrivalTime(arrivalTime);

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
    }
}

/*
 * Handle the arrival of a new packet by trying to store it into
 * the buffer. If not possible, the packet is discarded;
 */
void Transmitter::handleArrivedPacket(cMessage* msg){
    PacketMsg* pkt = check_and_cast<PacketMsg*>(msg);
    int id = this->getId();
    /*
     * if the buffer is full then the packets is discard
     */
    if((end_idx + 1) % bufferSize == start_idx)
    {
        EV << "transmitter " << id << ": arrival packet discarded because the buffer is full" << endl;
        emit(overflowPercentageSignal_, 1);
    }
    else
    {
        EV << "transmitter " << id << ": arrival packet inserted into the buffer " << endl;
        buffer[end_idx] = pkt;
        end_idx = (end_idx + 1) % bufferSize;
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
            backoffTime = intuniform(1, maxBackoffTime);
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
                start_idx = (start_idx + 1) % bufferSize;
                maxBackoffTime = 2;
                EV << "transmitter " << id << ": ACK received " << endl;
            }

            /*
             * if an ACK or a TRIGGER packet is received the transmitter check if
             * the buffer is empty and if not it starts with the Bernoullian experiment
             */
            if(start_idx != end_idx && uniform(0.0, 1.0) < sendProbability)
            {
                buffer[start_idx]->setIdChannel(intuniform(0, numChannels));
                send(buffer[start_idx], "out");
                EV << "transmitter " << id << ": packet sent, waiting for answer " << endl;
            }
        }
    }
}

