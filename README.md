# Wireless Slotted Random Access Protocol Simulation

Project carried out as part of the *Performance Evaluation of Computer Systems and Networks* Exam.

**Project Specifics**:

In a *slotted random-access network*, **N** couples transmitter-receiver share the same communication
medium, which consists of **C** separate channels. Multiple attempts to use the same channel in the
same slot by different transmissions will lead to collision, hence no receiver listening on that
channel will be able to decode the message. 
Assume that each of the **N** transmitters generate packets according to an exponential inter-arrival
distribution, and picks its channel at random on every new transmission. Before sending a packet, it
keeps extracting a value from a Bernoullian RV with success probability **p** on every slot, until it
achieves success. Then it transmits the packet and starts over. If a collision occurs, then the
transmitter backs off for a random number of slots (see later), and then starts over the whole
Bernoullian experiment.
The number of back-off slots is extracted as U(1, 2^(x+1)), where x is the number of collisions
experienced by the packet being transmitted.

**Project work**:
* Definition of Key Performance Indicators (Throughput and Response Time) and Factors (Number of Couples Tx-Rx, Numbers of Channels, Send Probability, Mean Inter-arrival time, 
* Modeling of the system through C++ (*OMNet++ Framework*)
* Verification of the above system through simplified cases 
* Experiment Design, Simulation and Data Analysis with *MS Excel* for gathering insights of factors influence on KPIs

Group Members: Tommaso Burlon, Francesco Iemma, Olgerti Xhanej

[![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fgerti98%2FPerformanceEvaluationGroupProject&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false)](https://hits.seeyoufarm.com)
