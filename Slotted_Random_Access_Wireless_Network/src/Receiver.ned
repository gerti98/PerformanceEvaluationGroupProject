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
simple Receiver
{
    parameters:
        double timeThreshold @unit(s);

        //From the first arrival at the transmitter to the receiver
        @signal[responseTimeSignal](type="double");
        @statistic[responseTime](source="responseTimeSignal"; record=mean);

        //Maintains info of the percentage of the respected time threshold
        @signal[thresholdSignal](type"long");
        @statistic[threshold](source="thresholdSignal"; record=stats);

		@signal[numPacketReceivedSignal](type="long");
		@statistic[numPacketReceived](source="numPacketReceivedSignal"; record=sum);
			
        @display("i=block/rx");
    gates:
        input in;
}
