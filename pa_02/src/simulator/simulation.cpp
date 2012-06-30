#include <ns3/drop-tail-queue.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv4-interface-container.h>
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/csma-module.h>
#include <ns3/internet-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/applications-module.h>
#include <ns3/ipv4-global-routing-helper.h>
#include <ns3/application-container.h>
#include <ns3/packet.h>


// Networg Topology
//
//             10.0.0.0/8                192.168.178.0/24
// Server --------------------- Router -------------------- Client
//    ( 2 Mbps, 20 ms, Drop Tail )   ( 2 Mbps, 20 ms, Drop Tail )
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("rene-pa_02");

//void TraceTRxN(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface) 
//{ 
////  *stream->GetStream() << Simulator::Now().GetSeconds() << '\t';
////  packet->Print(*stream->GetStream());
////  *stream->GetStream() << std::endl;
//  
//  Packet p(*packet);
//  Ipv4Header iph;
//  p.PeekHeader(iph);
//  p.RemoveHeader(iph);
//  
//  TcpHeader tcpHeader;
//  if (p.PeekHeader(tcpHeader))
//  {
//    *stream->GetStream() << Simulator::Now().GetSeconds() << '\t';
////    tcpHeader.Print(*stream->GetStream());
//    *stream->GetStream() << (uint32_t)tcpHeader.GetSequenceNumber().GetValue();
//    *stream->GetStream() << '\t';
//    *stream->GetStream() << (uint32_t)tcpHeader.GetAckNumber().GetValue();
//    *stream->GetStream() << std::endl;
//  }
//}

void TraceRx(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> packet) 
{
//  *stream->GetStream() << Simulator::Now().GetSeconds() << '\t';
//  packet->Print(*stream->GetStream());
//  *stream->GetStream() << std::endl;
  
  Packet p(*packet);
  Ipv4Header ipHeader;
  p.PeekHeader(ipHeader);
  p.RemoveHeader(ipHeader);
  
  TcpHeader tcpHeader;
  if (p.PeekHeader(tcpHeader))
  {
    *stream->GetStream() << Simulator::Now().GetSeconds() << '\t';
    *stream->GetStream() << (uint32_t)tcpHeader.GetAckNumber().GetValue();
    *stream->GetStream() << std::endl;
  }
}

void TraceTx(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> packet) 
{
//  *stream->GetStream() << Simulator::Now().GetSeconds() << '\t';
//  packet->Print(*stream->GetStream());
//  *stream->GetStream() << std::endl;
  
  Packet p(*packet);
  
  PppHeader pppHeader;
  p.PeekHeader(pppHeader);
  p.RemoveHeader(pppHeader);
  
  Ipv4Header ipHeader;
  p.PeekHeader(ipHeader);
  p.RemoveHeader(ipHeader);
  
  TcpHeader tcpHeader;
  if (p.PeekHeader(tcpHeader))
  {
    *stream->GetStream() << Simulator::Now().GetSeconds() << '\t';
    *stream->GetStream() << (uint32_t)tcpHeader.GetSequenceNumber().GetValue();
    *stream->GetStream() << std::endl;
  }
}

void RWNDCallback(uint32_t oldRWND, uint32_t newRWND) {
  std::cout << "old rwnd: " << oldRWND << " new rwnd: " << newRWND << std::endl;
}

int
main (int argc, char *argv[])
{
  // Options
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
//  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpTahoe"));
//  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpReno"));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    
  // create nodes
  NodeContainer serverRouterNodes;
  serverRouterNodes.Create(2);

  NodeContainer routerClientNodes;
  routerClientNodes.Add(serverRouterNodes.Get(1));
  routerClientNodes.Create(1);

  // create devices and set settings
//  CsmaHelper deviceSettingsHelper;
//  deviceSettingsHelper.SetChannelAttribute("DataRate", StringValue("2Mbps"));
  
  PointToPointHelper deviceSettingsHelper;
  deviceSettingsHelper.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
  
  deviceSettingsHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  deviceSettingsHelper.SetQueue(DropTailQueue::GetTypeId().GetName());

  NetDeviceContainer serverRouterDevices;
  serverRouterDevices = deviceSettingsHelper.Install(serverRouterNodes);

  // on greater networks use CsmaHelper to install devices
  NetDeviceContainer routerClientDevices;
  routerClientDevices = deviceSettingsHelper.Install(routerClientNodes);

  // create internet stack
  InternetStackHelper stack;
  stack.Install(serverRouterNodes.Get(0));
  stack.Install(routerClientNodes);

  // assign ip adresses
  Ipv4AddressHelper adressHelper;
  adressHelper.SetBase("10.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer serverRouterInterfaces;
  serverRouterInterfaces = adressHelper.Assign(serverRouterDevices);

  adressHelper.SetBase("192.168.178.0", "255.255.255.0");
  Ipv4InterfaceContainer routerClinetInterfaces;
  routerClinetInterfaces = adressHelper.Assign(routerClientDevices);

  // create routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // create pcap files for each device
  deviceSettingsHelper.EnablePcapAll("trace");
  
  // error model
  Ptr<ReceiveListErrorModel> errorModel = CreateObject<ReceiveListErrorModel>();
  std::list<uint32_t> lostPackets = errorModel->GetList();
  // select packets to be loose
  lostPackets.push_back(11);
  lostPackets.push_back(16);
  lostPackets.push_back(22);
  
  errorModel->SetList(lostPackets);
  // set receive error model on server interface
  serverRouterDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));
  
  Packet::EnablePrinting();
  
  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> streamRx = asciiTraceHelper.CreateFileStream ("plotdata/server-trace-rx.data");
//  Config::ConnectWithoutContext("/NodeList/0/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback(&TraceTRxN, streamRx));
//  
  Ptr<OutputStreamWrapper> streamTx = asciiTraceHelper.CreateFileStream ("plotdata/server-trace-tx.data");
//  Config::ConnectWithoutContext("/NodeList/0/$ns3::Ipv4L3Protocol/Tx", MakeBoundCallback(&TraceTRxN, streamTx));
  
  serverRouterDevices.Get(0)->TraceConnectWithoutContext("MacRx", MakeBoundCallback(&TraceRx, streamRx));
  serverRouterDevices.Get(0)->TraceConnectWithoutContext("MacTx", MakeBoundCallback(&TraceTx, streamTx));
  
  ///////////////////////////////////////////////////////////////////////////

  // test with ping application
  // client ping server 5 times with one second intervall
//  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
//  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
//  
//  UdpEchoServerHelper echoServer (9);
//
//  ApplicationContainer serverApps = echoServer.Install (serverRouterNodes.Get (0));
//  serverApps.Start (Seconds (1.0));
//  serverApps.Stop (Seconds (10.0));
//  
//  UdpEchoClientHelper echoClient (serverRouterInterfaces.GetAddress (0), 9);
//  echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
//  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
//  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
//  
//  ApplicationContainer clientApps = echoClient.Install (routerClientNodes.Get (1));
//  clientApps.Start (Seconds (2.0));
//  clientApps.Stop (Seconds (10.0));
  
  ////////////////////
  
//  LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
  
  uint16_t receivePort = 55000;
  InetSocketAddress receiveSock(receivePort);
  PacketSinkHelper sinkHelper(TcpSocketFactory::GetTypeId().GetName(), receiveSock);
  ApplicationContainer sinkApps = sinkHelper.Install (routerClientNodes.Get(1));
    
//  LogComponentEnable("BulkSendApplication", LOG_LEVEL_LOGIC);
  
  InetSocketAddress sendSock(routerClinetInterfaces.GetAddress(1), receiveSock.GetPort());
  BulkSendHelper packetSendHelper(TcpSocketFactory::GetTypeId().GetName(), sendSock);
  ApplicationContainer srcApps = packetSendHelper.Install(serverRouterNodes.Get(0));
  
  sinkApps.Start (Seconds (1.0));
  sinkApps.Stop (Seconds (10.0));
  srcApps.Start(Seconds(2.0));
  srcApps.Stop(Seconds(9.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
