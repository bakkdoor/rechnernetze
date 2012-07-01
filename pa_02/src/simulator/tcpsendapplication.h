
#ifndef TCPSENDAPPLICATION_H
#define	TCPSENDAPPLICATION_H

namespace ns3 {
  
  class TcpSendApplication : public Application 
  {
  public:

    TcpSendApplication ();
    virtual ~TcpSendApplication();

    void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate);

  private:
    virtual void StartApplication (void);  
    virtual void StopApplication (void);

    void ScheduleTx (void);
    void SendPacket (void);

    Ptr<Socket>     m_socket;
    Address         m_peer;
    uint32_t        m_packetSize;
    uint32_t        m_nPackets;
    DataRate        m_dataRate;
    EventId         m_sendEvent;
    bool            m_running;
  };
  
}

#endif	/* TCPSENDAPPLICATION_H */

