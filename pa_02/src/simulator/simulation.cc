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
#include <ns3/config-store.h>

#include "tcpsendapplication.h"


// Networg Topology
//
//             10.0.0.0/8                192.168.178.0/24
// Server --------------------- Router -------------------- Client
//    ( 2 Mbps, 20 ms, Drop Tail )   ( 2 Mbps, 20 ms, Drop Tail )
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("rene-pa_02");

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
    
    uint8_t flags = tcpHeader.GetFlags();
    if (flags & tcpHeader.SYN) {
      *stream->GetStream() << "\tSYN";
    }
    if (flags & tcpHeader.FIN) {
      *stream->GetStream() << "\tFIN";
    }
    if (flags & tcpHeader.RST) {
      *stream->GetStream() << "\tRST";
    }
    if (flags & tcpHeader.CWR) {
      *stream->GetStream() << "\tCWR";
    }
    if (flags & tcpHeader.ECE) {
      *stream->GetStream() << "\tECE";
    }
    if (flags & tcpHeader.PSH) {
      *stream->GetStream() << "\tPSH";
    }
    if (flags & tcpHeader.URG) {
      *stream->GetStream() << "\tURG";
    }
    if (flags & tcpHeader.ACK) {
      *stream->GetStream() << "\tACK";
    }
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
    
    uint8_t flags = tcpHeader.GetFlags();
    if (flags & tcpHeader.SYN) {
      *stream->GetStream() << "\tSYN";
    }
    if (flags & tcpHeader.FIN) {
      *stream->GetStream() << "\tFIN";
    }
    if (flags & tcpHeader.RST) {
      *stream->GetStream() << "\tRST";
    }
    if (flags & tcpHeader.CWR) {
      *stream->GetStream() << "\tCWR";
    }
    if (flags & tcpHeader.ECE) {
      *stream->GetStream() << "\tECE";
    }
    if (flags & tcpHeader.PSH) {
      *stream->GetStream() << "\tPSH";
    }
    if (flags & tcpHeader.URG) {
      *stream->GetStream() << "\tURG";
    }
    if (flags & tcpHeader.ACK) {
      *stream->GetStream() << "\tACK";
    }
    *stream->GetStream() << std::endl;
  }
}

Ptr<Socket> SrcTcpSocket;
void TraceCwSsth(Ptr<OutputStreamWrapper> stream, uint32_t oldVal, uint32_t newVal) {
  UintegerValue ssth;
  SrcTcpSocket->GetAttribute("SlowStartThreshold", ssth);
  *stream->GetStream() << Simulator::Now().GetSeconds() << '\t' 
          << newVal << "\t" << ssth.Get() << std::endl;
}

int
main (int argc, char *argv[])
{
  std::string tcpVersion("NewReno");
  
  CommandLine cmd;
  cmd.AddValue("tcpVersion", "TCP version (Tahoe, Reno, NewReno)", tcpVersion);
  cmd.Parse (argc, argv);
  
  // Options
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue(true));
//  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(2));
  
  if (tcpVersion.compare("Tahoe") != 0 && tcpVersion.compare("Reno") != 0 && tcpVersion.compare("NewReno") != 0) {
    std::cerr << "tcpVersion argument should be Tahoe, Reno or NewReno!" << std::endl;
    exit(EXIT_FAILURE);
  }
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::Tcp" + tcpVersion));
  NS_LOG_INFO("TCP Version set to " << tcpVersion);
    
  // create nodes
  NodeContainer serverRouterNodes;
  serverRouterNodes.Create(2);

  NodeContainer routerClientNodes;
  routerClientNodes.Add(serverRouterNodes.Get(1));
  routerClientNodes.Create(1);

  // create devices and set settings  
  PointToPointHelper deviceSettingsHelper;
  deviceSettingsHelper.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
  deviceSettingsHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  deviceSettingsHelper.SetQueue(DropTailQueue::GetTypeId().GetName());
  
  NetDeviceContainer serverRouterDevices;
  serverRouterDevices = deviceSettingsHelper.Install(serverRouterNodes);
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
  
//  Packet::EnablePrinting();
  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> streamRx = asciiTraceHelper.CreateFileStream ("plotdata/server-trace-rx.data");
  Ptr<OutputStreamWrapper> streamTx = asciiTraceHelper.CreateFileStream ("plotdata/server-trace-tx.data");
  
  serverRouterDevices.Get(0)->TraceConnectWithoutContext("MacRx", MakeBoundCallback(&TraceRx, streamRx));
  serverRouterDevices.Get(0)->TraceConnectWithoutContext("MacTx", MakeBoundCallback(&TraceTx, streamTx));
  
//  LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
  
  uint16_t receivePort = 55000;
  InetSocketAddress receiverAddr(receivePort);
  PacketSinkHelper sinkHelper(TcpSocketFactory::GetTypeId().GetName(), receiverAddr);
  ApplicationContainer sinkApps = sinkHelper.Install (routerClientNodes.Get(1));
    
//  LogComponentEnable("BulkSendApplication", LOG_LEVEL_LOGIC);
  
  InetSocketAddress sinkAddr(routerClinetInterfaces.GetAddress(1), receiverAddr.GetPort());
//  BulkSendHelper packetSendHelper(TcpSocketFactory::GetTypeId().GetName(), sinkAddr);
//  ApplicationContainer srcApps = packetSendHelper.Install(serverRouterNodes.Get(0));
  
  SrcTcpSocket = Socket::CreateSocket (serverRouterNodes.Get (0), TcpSocketFactory::GetTypeId ());
  Ptr<OutputStreamWrapper> streamCwSsth = asciiTraceHelper.CreateFileStream ("plotdata/server-cw-ssth.data");
  SrcTcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&TraceCwSsth, streamCwSsth));
  
  Ptr<TcpSendApplication> app = CreateObject<TcpSendApplication> ();
  DataRateValue dataRate;
  serverRouterDevices.Get(0)->GetAttribute("DataRate", dataRate);
  app->Setup (SrcTcpSocket, sinkAddr, 2000, dataRate.Get());
  
  serverRouterNodes.Get(0)->AddApplication(app);
  app->SetStartTime (Seconds (2.));
  app->SetStopTime (Seconds (9.));

  sinkApps.Start (Seconds (1.0));
  sinkApps.Stop (Seconds (10.0));
//  srcApps.Start(Seconds(2.0));
//  srcApps.Stop(Seconds(9.0));
  
  // dubug attribute values
//  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-attributes.xml"));
//  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
//  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
//  ConfigStore outputConfig;
//  outputConfig.ConfigureDefaults ();
//  outputConfig.ConfigureAttributes ();

  // Output config store to txt format
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-attributes.txt"));
  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("RawText"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
  ConfigStore outputConfig2;
  outputConfig2.ConfigureDefaults ();
  outputConfig2.ConfigureAttributes ();

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
