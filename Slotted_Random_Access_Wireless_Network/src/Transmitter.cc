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
    numPacketCreatedSignal_ = registerSignal("numPacketCreatedSignal");
    numPacketOnBufferSignal_ = registerSignal("numPacketOnBufferSignal");

    meanPacketSignal_ = registerSignal("meanPacketSignal");
    olgertiMeanPacketSignal_ = registerSignal("olgertiMeanPacketSignal");

    meanPktInBuffer_ = 0;

    olgertiLastSimtime_ = 0;
    olgertiMeanPktInBuffer_ = 0;

    /*
     * buffer related variable initialization
     */
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
        this->updateBufferCount();
        handleArrivedPacket(msg);
        computeModuleStatistics();
    }
    else
    {
        handleChannelPacket(msg);
        delete(msg);
    }
}

/*
 * Handle the arrival of a new packet and store it into
 * the buffer.
 */
void Transmitter::handleArrivedPacket(cMessage* msg){
    PacketMsg* pkt = check_and_cast<PacketMsg*>(msg);
    int id = this->getId();

    //Register creation of the packet
    emit(numPacketCreatedSignal_, 1);


    EV << "TX-" << id << ": arrival packet inserted into the buffer " << endl;
    pkt->setIdTransmitter(id);
    buffer.push(pkt);

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
    if(backoffTime > 0 && par("isBackoff"))
    {
        backoffTime--;
        EV << "TX-" << id << ": Backoff remained: " << backoffTime << endl;
    }
    else
    {
        /*
         * when a NACK is received a collision occurred, so the transmitter
         * double his back off time from the previous collision and start counting
         */
        if(strcmp(msg->getName(), "NACK") == 0 && par("isBackoff"))
        {
            maxBackoffTime *= 2;
            backoffTime = intuniform(1, maxBackoffTime, 1);
            EV << "TX-" << id << ": NACK received, back-off time = " << backoffTime << endl;
        }
        else
        {
            /*
             * when an ACK is received there was no collision and the previous packet
             * can be removed from the buffer and the back-off time can be cleared
             */
            if(strcmp(msg->getName(), "ACK") == 0)
            {
                /*
                 * Computation of the mean number of packet in the queue (here sum, division in finish())
                 */
                simtime_t howManyTimeInQueue = simTime() - buffer.front()->getCreationTime();
                meanPktInBuffer_ = meanPktInBuffer_ + buffer.size()*(howManyTimeInQueue.dbl());

                this->updateBufferCount();
                delete(buffer.front());
                buffer.pop();

                maxBackoffTime = 2;
                EV << "TX-" << id << ": ACK received " << endl;
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
                EV << "TX-" << id << ": packet sent, waiting for answer " << endl;
            }
        }
    }
}

void Transmitter::computeModuleStatistics(){
    emit(numPacketOnBufferSignal_, (long)buffer.size());
}


void Transmitter::updateBufferCount(){
    double warmup = getSimulation()->getWarmupPeriod().dbl();

    if(simTime().dbl() >= warmup){
        // Handle start of count in case of warmup between
        // current simtime and last simtime registered
        if(olgertiLastSimtime_ < warmup)
            olgertiLastSimtime_ = warmup;

        olgertiMeanPktInBuffer_ = olgertiMeanPktInBuffer_ + buffer.size()*(simTime().dbl()-olgertiLastSimtime_.dbl());
        EV << "Buffer size:  " << buffer.size() << ", duration: " << (simTime().dbl()-olgertiLastSimtime_.dbl()) << ", sum: " << olgertiMeanPktInBuffer_ << endl;
        olgertiLastSimtime_ = simTime();
    }
}

void Transmitter::finish(){
    // Retrieve warmup period
    double warmup = getSimulation()->getWarmupPeriod().dbl();
    // Retrieve simulation time limit
    cConfigOption simTimeConfig("sim-time-limit", true,cConfigOption::Type::CFG_DOUBLE, "s", "300", "");
    double simLimit = getSimulation()->getEnvir()->getConfig()->getAsDouble(&simTimeConfig);
    // Computation of simulation duration
    long simDuration = simLimit-warmup;


    //Last update
    this->updateBufferCount();

    // Computation of the mean number of packet in the buffer
    double toEmit = meanPktInBuffer_/simDuration;
    double olgertiToEmit = olgertiMeanPktInBuffer_/simDuration;

    // Emission of the value
    EV <<  toEmit << " warmup: " << warmup << "simLimiti: " << simLimit << " simdur: " << simDuration << " meanPkt: " << meanPktInBuffer_ << endl;
    emit(meanPacketSignal_, toEmit);
    emit(olgertiMeanPacketSignal_, olgertiToEmit);

    while(buffer.empty() == false){
        delete(buffer.front());
        buffer.pop();
    }
}

