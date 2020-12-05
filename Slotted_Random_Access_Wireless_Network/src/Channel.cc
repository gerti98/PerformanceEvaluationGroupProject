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

#include "Channel.h"
#include "PacketMsg_m.h"

Define_Module(Channel);

void Channel::initialize()
{
    throughputSignal_ = registerSignal("throughputSignal");


    // Array Initialization
    int numOfChannels = getAncestorPar("numChannels");

    for(int i=0; i<numOfChannels; i++)
        isCollided_.push_back(false);

    scheduleTimeSlot();

}

void Channel::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage())
    {
        EV << "Received self message" << endl;

        /* If it's a self message then it means that it is
         * the trigger of the start of the slot.
         * So I have to control all the packet in
         * order to understand in which channels there
         * are collisions*/
        findCollisions();

        /* Now the channel transmit to the receivers the ACK for the
         * packets that don't collide and send a NACK
         * for the packets that collide.
         * It transmits to the receiver the non-collided packets */
        transmission();

        // Delete the trigger message
        delete(msg);

        // Schedule the next time slot
        scheduleTimeSlot();
    }
    else
    {
        /* If it's not a self message then it means that
         * this is a packet.
         * So I have to push it in the vector of packets
         * for this slot*/
        PacketMsg* newPkt = check_and_cast<PacketMsg*>(msg);
        newPkt->setIndexTx(newPkt->getArrivalGate()->getIndex());
        EV << "Packet from tx " << newPkt->getIdTransmitter() << " arrived" << endl;
        packetsOfSlot_.push_back(newPkt);
    }
}


/* Control every packet and set isCollided[i] to true
 * if there is a collision in the channel i*/
void Channel::findCollisions()
{
    for(int i = 0; i<(int)packetsOfSlot_.size(); i++)
    {
        int ch = packetsOfSlot_[i]->getIdChannel();
        for(int j = i+1; j<(int)packetsOfSlot_.size(); j++)
        {
            if(ch==(packetsOfSlot_[j]->getIdChannel())){
                isCollided_[ch] = true;
                EV << "Collision at Channel " << ch << endl;
            }

        }
    }
}

/* Transmit ACK or NACK to the transmitters
 * and non-collided packets to the receivers*/
void Channel::transmission()
{
    std::vector<int> triggeredTx;
    int packetSent = 0;
    for(int i = 0; i<(int)packetsOfSlot_.size(); i++)
    {

        int idTx = packetsOfSlot_[i]->getIdTransmitter();
        int indexTx = packetsOfSlot_[i]->getIndexTx();

        // Store the index of the triggered tx
        triggeredTx.push_back(indexTx);

        if(isCollided_[packetsOfSlot_[i]->getIdChannel()])
        {
            // A collision occur for this packet
            // send a NACK to the transmitter
            EV << "NACK sended to Transmitter " << idTx<< endl;
            cMessage* nack = new cMessage("NACK");
            send(nack,"out_tx",indexTx);

            delete(packetsOfSlot_[i]);
        }
        else
        {
            // No collision
            // Send a ACK to the transmitter
            EV << "ACK sended to Transmitter " << idTx<< endl;
            cMessage* ack = new cMessage("ACK");
            send(ack,"out_tx",indexTx);

            // Send the pkt to the receiver
            //int idRx = packetsOfSlot_[i]->getIdReceiver();
            //Reused indexTx to get the id of the port
            EV << "Sended packet to Receiver " << indexTx<< endl;
            send(packetsOfSlot_[i],"out_rx",indexTx);

            //Count packet sent to the receiver
            packetSent++;
        }
    }

    // Emit throughput (num channel with packets sent / total channels)
    EV << "Channel: sent " << packetSent << " packets" << endl;

    if(isCollided_.size() > 0)
        emit(throughputSignal_, ((packetSent*1.0)/isCollided_.size()));
    else
        emit(throughputSignal_, 0);



    /* Trigger the tx which don't transmit a packet in the previous timeslot.
     * So the ones that are not been triggered yet*/
    triggerOthers(triggeredTx);


    // Reset vectors
    /* The following instruction it's equivalent
     * to packetsOfSlot_.clear() but force the
     * reallocation of the vector. This ensures that
     * the vector capacity will change (to 0)*/
    std::vector<PacketMsg*>().swap(packetsOfSlot_);

    for(int i = 0; i<(int)isCollided_.size(); i++)
        isCollided_[i] = false;
}

/* It triggers the transmitters that have not been triggered yet */
void Channel::triggerOthers(std::vector<int> triggeredTx)
{
    int numTx = getAncestorPar("numTransmitters");
    int cnt = 0;
    for(int i = 0; i<numTx; i++)
    {
        for(int j = 0; j<(int)triggeredTx.size(); j++)
        {
            if(i!=triggeredTx[j])
                cnt++;
        }

        /* If the i-th transmitter is not in the vector triggeredTx it means
         * that the i-th transmitter has not been triggered.
         * i-th tx is not in the vector if i!=triggeredTx[j] for any valid i,j
         * and so if the previous relation is respected n time with n equal to the
         * size of triggeredTx*/
        if(cnt==(int)triggeredTx.size())
        {
            cMessage* trigger = new cMessage("TRIGGER");
            EV << "Trigger sent to tx at gate" << i << endl;
            send(trigger,"out_tx",i);
        }
        cnt = 0;
    }
}


/* Send a self message in order to trigger the start
 * of the time slot */
void Channel::scheduleTimeSlot()
{
    simtime_t timeSlot = par("timeSlotSize");
    cMessage* timeSlotTrigger = new cMessage("timeSlotTrigger");

    /* Set the schedule priority in a way that allow us to
     * handle this message when the channel has received
     * all the packets of the transmitters. So I put
     * priority 1 (0 is the highest priority)*/
    //timeSlotTrigger->setSchedulingPriority(1);
    /* Not needed because the channel trigger txs
     * and control their packet in the at the end of this timeSlot
     * (so the beginning of the next)*/


    /*Schedule the sending*/
    scheduleAt(simTime()+timeSlot,timeSlotTrigger);
}


/**
 * Delete every packet left in the Channel
 */
void Channel::finish(){
    for(PacketMsg* p: packetsOfSlot_){
        delete(p);
    }
    std::vector<PacketMsg*>().swap(packetsOfSlot_);
}




