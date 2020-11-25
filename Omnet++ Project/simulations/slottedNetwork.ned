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

package slotted_random_access_wireless_network.simulations;

import slotted_random_access_wireless_network.Channel;
import slotted_random_access_wireless_network.Receiver;
import slotted_random_access_wireless_network.Transmitter;


network slottedNetwork{
    parameters:
        int numTransmitters;
        int numChannels;
        
        int bufferSize;
        
        double sendProbability;
        
        double expMean @unit(s);
        
        double timeSlotSize @unit(s);
        
        double thresh @unit(s); 
    submodules:
        transmitters[numTransmitters]: Transmitter {
            parameters:
                numChannels = numChannels;
                bufferSize = bufferSize;
                sendProbability = sendProbability;
                meanInterarrivalTime = expMean;
        }
        receivers[numTransmitters]: Receiver{
         	parameters:   
         	    timeThreshold = thresh;
        }
        ch: Channel {
            parameters:
                numChannels = numChannels;
                timeSlotSize = timeSlotSize;
            gates:
                in_tx[numTransmitters];
                out_tx[numTransmitters];
                out_rx[numTransmitters];
        }
    connections:
        
        for i = 0..numTransmitters-1{
        	transmitters[i].out --> ch.in_tx[i];
        	transmitters[i].in <-- ch.out_tx[i];
        	receivers[i].in <-- ch.out_rx[i];
        }
}