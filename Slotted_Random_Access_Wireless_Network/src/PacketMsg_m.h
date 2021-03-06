//
// Generated file, do not edit! Created by nedtool 5.6 from PacketMsg.msg.
//

#ifndef __PACKETMSG_M_H
#define __PACKETMSG_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>PacketMsg.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet PacketMsg
 * {
 *     //Channel choosed for the current transmission, may change in case of collision
 *     int idChannel;
 * 
 *     // id of the transmitter of the message
 *     int idTransmitter;
 * 
 *     // id of the channel gate at which the message is arrived
 *     int idGate;
 * 
 *     //Time in which the packet arrives for the first time at the Transmitter
 *     simtime_t creationTime;
 * }
 * </pre>
 */
class PacketMsg : public ::omnetpp::cPacket
{
  protected:
    int idChannel;
    int idTransmitter;
    int idGate;
    ::omnetpp::simtime_t creationTime;

  private:
    void copy(const PacketMsg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PacketMsg&);

  public:
    PacketMsg(const char *name=nullptr, short kind=0);
    PacketMsg(const PacketMsg& other);
    virtual ~PacketMsg();
    PacketMsg& operator=(const PacketMsg& other);
    virtual PacketMsg *dup() const override {return new PacketMsg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getIdChannel() const;
    virtual void setIdChannel(int idChannel);
    virtual int getIdTransmitter() const;
    virtual void setIdTransmitter(int idTransmitter);
    virtual int getIdGate() const;
    virtual void setIdGate(int idGate);
    virtual ::omnetpp::simtime_t getCreationTime() const;
    virtual void setCreationTime(::omnetpp::simtime_t creationTime);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const PacketMsg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, PacketMsg& obj) {obj.parsimUnpack(b);}


#endif // ifndef __PACKETMSG_M_H

