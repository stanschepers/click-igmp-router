#ifndef IGMP_INPUT_HANDLER
#define IGMP_INPUT_HANDLER

#include <click/config.h>
#include <click/element.hh>
#include "IgmpFilter.hh"

CLICK_DECLS

class IgmpInputHandler;

class IgmpInputHandler : public Element
{
public:
  IgmpInputHandler();
  ~IgmpInputHandler();

  // Description of ports:
  //
  //     Input:
  //         0. Incoming IP packets which are filtered based on their source
  //            address.
  //
  //     Output:
  //         0. Generated IGMP packets.
  //
  //         1. Incoming IP packets which have been filtered based on their
  //            source address.
  //
  //         2. Incoming IP packets which were filtered out. They are not intended
  //            for the current host.

  const char *class_name() const { return "IgmpInputHandler"; }
  const char *port_count() const { return "1/3"; }
  const char *processing() const { return PUSH; }

  int configure(Vector<String> &, ErrorHandler *);

  static int join(const String &conf, Element *e, void *thunk, ErrorHandler *errh);
  static int leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh);

  void add_handlers();

  void push(int port, Packet *packet);

  void push_listen(const IPAddress &multicast_address, const IgmpFilterRecord &record);

private:
  IgmpFilter filter;
};

CLICK_ENDDECLS
#endif