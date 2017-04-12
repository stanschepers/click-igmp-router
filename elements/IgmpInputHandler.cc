#include "IgmpInputHandler.hh"

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "IgmpMessage.hh"
#include "IgmpMessageManip.hh"
#include "IgmpFilter.hh"

CLICK_DECLS
IgmpInputHandler::IgmpInputHandler()
{
}

IgmpInputHandler::~IgmpInputHandler()
{
}

int IgmpInputHandler::configure(Vector<String> &conf, ErrorHandler *errh)
{
    // Nothing to do here.
    if (cp_va_kparse(conf, this, errh, cpEnd) < 0)
        return -1;
    return 0;
}

void IgmpInputHandler::push_listen(const IPAddress &multicast_address, const IgmpFilterRecord &record)
{
    filter.listen(multicast_address, record);
    click_chatter("sending listen request for multicast address %s", multicast_address.unparse().c_str());

    IgmpV3MembershipReport report;
    report.group_records.push_back(IgmpV3GroupRecord(multicast_address, record, true));

    size_t tailroom = 0;
    size_t packetsize = report.get_size();
    size_t headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket *packet = Packet::make(headroom, 0, packetsize, tailroom);
    if (packet == 0)
        return click_chatter("cannot make packet!");

    auto data_ptr = packet->data();
    report.write(data_ptr);

    // packet->set_dst_ip_anno(to);

    output(0).push(packet);
}

int IgmpInputHandler::join(const String &conf, Element *e, void *, ErrorHandler *errh)
{
    IgmpInputHandler *self = (IgmpInputHandler *)e;
    IPAddress to;
    if (cp_va_kparse(conf, self, errh, "TO", cpkM, cpIPAddress, &to, cpEnd) < 0)
        return -1;

    click_chatter("IGMP join %s", to.unparse().c_str());
    self->push_listen(to, create_igmp_join_record());
    return 0;
}

int IgmpInputHandler::leave(const String &conf, Element *e, void *, ErrorHandler *errh)
{
    IgmpInputHandler *self = (IgmpInputHandler *)e;
    IPAddress to;
    if (cp_va_kparse(conf, self, errh, "TO", cpkM, cpIPAddress, &to, cpEnd) < 0)
        return -1;

    click_chatter("IGMP leave %s", to.unparse().c_str());
    self->push_listen(to, create_igmp_leave_record());
    return 0;
}

void IgmpInputHandler::add_handlers()
{
    add_write_handler("join", &join, (void *)0);
    add_write_handler("leave", &leave, (void *)0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpInputHandler)
