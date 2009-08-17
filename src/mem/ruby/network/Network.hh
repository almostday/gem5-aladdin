
/*
 * Copyright (c) 1999-2008 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Network.hh
 *
 * Description: The Network class is the base class for classes that
 * implement the interconnection network between components
 * (processor/cache components and memory/directory components).  The
 * interconnection network as described here is not a physical
 * network, but a programming concept used to implement all
 * communication between components.  Thus parts of this 'network'
 * will model the on-chip connections between cache controllers and
 * directory controllers as well as the links between chip and network
 * switches.
 *
 * $Id$
 * */

#ifndef NETWORK_H
#define NETWORK_H

#include "mem/ruby/common/Global.hh"
#include "mem/ruby/system/NodeID.hh"
#include "mem/protocol/MessageSizeType.hh"
#include "mem/ruby/system/System.hh"

class NetDest;
class MessageBuffer;
class Throttle;
class Topology;

class Network {
public:
  // Constructors
  Network(const string & name);
  virtual void init(const vector<string> & argv);

  // Destructor
  virtual ~Network() {}

  // Public Methods
  int getBufferSize() { return m_buffer_size; }
  int getNumberOfVirtualNetworks() { return m_virtual_networks; }
  int getEndpointBandwidth() { return m_endpoint_bandwidth; }
  bool getAdaptiveRouting() {return m_adaptive_routing; }
  int getLinkLatency() { return m_link_latency; }
  int MessageSizeType_to_int(MessageSizeType size_type);


  // returns the queue requested for the given component
  virtual MessageBuffer* getToNetQueue(NodeID id, bool ordered, int netNumber) = 0;
  virtual MessageBuffer* getFromNetQueue(NodeID id, bool ordered, int netNumber) = 0;
  virtual const Vector<Throttle*>* getThrottles(NodeID id) const { return NULL; }

  virtual int getNumNodes() {return 1;}

  virtual void makeOutLink(SwitchID src, NodeID dest, const NetDest& routing_table_entry, int link_latency, int link_weight, int bw_multiplier, bool isReconfiguration) = 0;
  virtual void makeInLink(SwitchID src, NodeID dest, const NetDest& routing_table_entry, int link_latency, int bw_multiplier, bool isReconfiguration) = 0;
  virtual void makeInternalLink(SwitchID src, NodeID dest, const NetDest& routing_table_entry, int link_latency, int link_weight, int bw_multiplier, bool isReconfiguration) = 0;

  virtual void reset() = 0;

  virtual void printStats(ostream& out) const = 0;
  virtual void clearStats() = 0;
  virtual void printConfig(ostream& out) const = 0;
  virtual void print(ostream& out) const = 0;

protected:

  // Private Methods
  // Private copy constructor and assignment operator
  Network(const Network& obj);
  Network& operator=(const Network& obj);

  // Data Members (m_ prefix)
protected:
  const string m_name;
  int m_nodes;
  int m_virtual_networks;
  int m_buffer_size;
  int m_endpoint_bandwidth;
  Topology* m_topology_ptr;
  bool m_adaptive_routing;
  int m_link_latency;
  int m_control_msg_size;
  int m_data_msg_size;
};

// Output operator declaration
ostream& operator<<(ostream& out, const Network& obj);

// ******************* Definitions *******************

// Output operator definition
extern inline
ostream& operator<<(ostream& out, const Network& obj)
{
  obj.print(out);
  out << flush;
  return out;
}

#endif //NETWORK_H
