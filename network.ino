#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <IPAddress.h>

#define DEVICE_IDENTIFIER "nachtmensch"
#define OSC_LOCAL_PORT 8000

#define MAX_SCAN_COUNT 3

#define CONNECTION_WAIT_TIMES 5000

#define CONNECTION_ERROR_WAIT_TIME 5000

// wifi credentials
const char *ssid = "nachtmensch";
const char *password = "";

char deviceName[25];

int connectionWaitTimes = 0;

WiFiMode_t wifiMode = WIFI_STA;

WiFiUDP Udp;

void setupNetwork()
{
  setupDeviceName();
  initWiFi();

  setupUDPServer();
  setupMDNS();

  // clean up led state
  ledStopBlink();
  ledOFF();
}

void setupDeviceName()
{
  char *mac = "00:00:00:00:00:00N";
  WiFi.macAddress().toCharArray(mac, 18);

  char hex[5];
  sprintf(hex, "%x", ESP.getChipId());
  strcpy(deviceName, concatStr(DEVICE_IDENTIFIER, "-", concatStr(generateName(mac), "-", hex)));

  Serial.print("Device Name: ");
  Serial.println(deviceName);
}

void initWiFi()
{
  WiFi.hostname(deviceName);

  // check if wifi is already available
  if (wifiExists())
  {
    initSTA();
    setupSTA();
  }
  else
  {
    initAP();
  }

  printNetworkInformation();
}

boolean wifiExists()
{
  Serial.println("check if wifi exists...");
  ledBlink();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wait(100);

  int scanCount = 0;
  boolean isWifiAvaialable = false;

  while (!isWifiAvaialable && scanCount < MAX_SCAN_COUNT)
  {
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i)
    {
      if (WiFi.SSID(i).equals(ssid))
        isWifiAvaialable = true;
    }

    if (!isWifiAvaialable)
    {
      Serial.print(scanCount + 1);
      Serial.println(" -> nope.");
      wait(1000);
    }
    else
    {
      Serial.print(ssid);
      Serial.println(" found!");
    }

    scanCount++;
  }

  ledOFF();
  return isWifiAvaialable;
}

void initAP()
{
  Serial.println("init AP mode...");

  wifiMode = WIFI_AP;
  WiFi.mode(wifiMode);
  WiFi.softAP(ssid, password);
}

void initSTA()
{
  Serial.println("init STA mode...");

  wifiMode = WIFI_STA;
  WiFi.mode(wifiMode);
  WiFi.begin(ssid, password);
}

void setupSTA()
{
  Serial.print("trying to connect...");
  ledBlink();

  // wait till wifi is connected
  while (WiFi.status() != WL_CONNECTED) {
    connectionWaitTimes++;

    // reset wifi
    if (connectionWaitTimes > CONNECTION_WAIT_TIMES)
    {
      Serial.println();
      Serial.println("re-init wifi...");
      initSTA();
      connectionWaitTimes = 0;
    }

    wait();
  }

  Serial.println();
}

void printNetworkInformation()
{
  // print out relevant information
  Serial.print("WiFi Mode: ");
  Serial.println(wifiMode);

  Serial.print("WiFi State: ");
  Serial.println(WiFi.status());

  Serial.print("Local IP address: ");
  Serial.println(getIPAddress());

  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
}

void setupMDNS()
{
  if (!MDNS.begin(deviceName)) {
    Serial.println("Error setting up MDNS responder!");
    ledError();
  }

  // Add service to MDNS-SD
  MDNS.addService("osc", "udp", OSC_LOCAL_PORT);
  MDNS.addServiceTxt("osc", "udp", "mac", WiFi.macAddress());
}

void setupUDPServer()
{
  Udp.begin(OSC_LOCAL_PORT);
}

String getIPAddress()
{
  if (wifiMode == WIFI_AP)
    return WiFi.softAPIP().toString();
  else
    return WiFi.localIP().toString();
}

void loopNetwork()
{
  loopUdp();

  // check for connection loss
  if (wifiMode == WIFI_STA && WiFi.status() != WL_CONNECTED)
  {
    ledError();
    Serial.println("lost connection...");
    wait(CONNECTION_ERROR_WAIT_TIME);

    initWiFi();
  }
}

void loopUdp()
{
  OSCMessage msg;
  int size;
  if ((size = Udp.parsePacket()) > 0) {
    while (size--)
      msg.fill(Udp.read());

    if (!msg.hasError()) {

      // feed because of watchdog
      ESP.wdtFeed();

      ledON();
      routeOSCMessage(msg);
      ledOFF();
    }
  }
}

