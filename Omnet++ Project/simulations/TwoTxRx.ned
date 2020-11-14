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


network TwoTxRx
{
    @display("bgb=520,352");
    submodules:
        channel1: Channel {
            @display("p=263,163");
        }
        transmitter: Transmitter {
            @display("p=69,83");
        }
        transmitter1: Transmitter {
            @display("p=62,221");
        }
        receiver: Receiver {
            @display("p=407,77");
        }
        receiver1: Receiver {
            @display("p=384,218");
        }
    connections:
        transmitter.out --> channel1.in_tx++;
        channel1.out_tx++ --> transmitter.in;
        transmitter1.out --> channel1.in_tx++;
        channel1.out_tx++ --> transmitter1.in;
        channel1.out_rx++ --> receiver.in;
        channel1.out_rx++ --> receiver1.in;
}
