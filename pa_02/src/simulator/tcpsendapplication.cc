#include <ns3/core-module.h>
#include <ns3/network-module.h>

#include "tcpsendapplication.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpSendApplication");

TcpSendApplication::TcpSendApplication ()
  : m_socket (0), 
    m_peer (), 
    m_packetSize (0), 
    m_nPackets (0), 
    m_dataRate (0), 
    m_sendEvent (), 
    m_running (false)
{
}

TcpSendApplication::~TcpSendApplication()
{
  m_socket = 0;
}

void
TcpSendApplication::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_dataRate = dataRate;
}

void
TcpSendApplication::StartApplication (void)
{
  m_running = true;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void 
TcpSendApplication::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void 
TcpSendApplication::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);
  ScheduleTx ();
}

void 
TcpSendApplication::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &TcpSendApplication::SendPacket, this);
    }
}