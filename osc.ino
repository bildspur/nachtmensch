IPAddress broadcastIP(255, 255, 255, 255);
const unsigned int outPort = 9000;

void setupOSC()
{
  // send reset reason boradcast
  sendRestartReason();
}

void sendRestartReason()
{
  Serial.print("Restart reason: ");
  Serial.println(ESP.getResetReason());

  rst_info *resetInfo = ESP.getResetInfoPtr();

  OSCMessage msg(concatStr("/nachtmensch/status/", deviceName));

  // add software version
  msg.add(version);

  // add reason
  msg.add(resetInfo->reason);

  // add extended information

  //exccause
  msg.add(resetInfo->exccause);

  //epc1
  msg.add(resetInfo->epc1);

  //epc2
  msg.add(resetInfo->epc2);

  //epc3
  msg.add(resetInfo->epc3);

  //excvaddr
  msg.add(resetInfo->excvaddr);

  //depc
  msg.add(resetInfo->depc);

  ESP.wdtFeed();

  Udp.beginPacket(broadcastIP, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
}

void routeOSCMessage(OSCMessage &msg)
{
  msg.route("/nachtmensch/warning", warningReceived);
}

void warningReceived(OSCMessage &msg, int addrOffset)
{
  Serial.println("received warning!");
}
