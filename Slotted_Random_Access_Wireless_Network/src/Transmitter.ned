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

package slotted_random_access_wireless_network;

//
// TODO auto-generated module
//
simple Transmitter
{
    parameters:
        double meanInterarrivalTime @unit(s) = default(1s);
		
        int numChannels = default(1);
        
        //Probability of sending a packet in the current timeslot
        double sendProbability = default(0.5);
		
        bool deterministicInterarrivalTime = default(false);
        bool isBackoff = default(true);
        
        @signal[numPacketCreatedSignal](type="long");
		@statistic[numPacketCreated](source="numPacketCreatedSignal"; record=sum);
        
        @signal[numPacketOnBufferSignal](type="long");
		@statistic[numPacketOnBuffer](source="numPacketOnBufferSignal"; record=vector, stats);
        
        @signal[meanPacketSignal](type="double");
        @statistic[meanPacket](source="meanPacketSignal";record=sum);
        
        @display("i=block/tx");
    gates:
        output out;
        input in;

}
