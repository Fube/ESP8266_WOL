#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <string.h>

int * parseIP(char *ip);

#include "config.h" // Make your own config.h file containing your SSID, PASSWORD, IP, GATEWAY, and SUBNET

// Prototypes
void handle_OnConnect();
void wakeMyPC();
int* parseIP(char *ip);
// End of prototypes

WiFiUDP UDP;
WakeOnLan WOL(UDP);

int* IP_ARRAY = parseIP(IP);
int* GATEWAY_ARRAY = parseIP(GATEWAY);
int* SUBNET_ARRAY = parseIP(SUBNET);

IPAddress local_ip(IP_ARRAY[0], IP_ARRAY[1], IP_ARRAY[2], IP_ARRAY[3]);
IPAddress gateway(GATEWAY_ARRAY[0], GATEWAY_ARRAY[1], GATEWAY_ARRAY[2], GATEWAY_ARRAY[3]);
IPAddress subnet(SUBNET_ARRAY[0], SUBNET_ARRAY[1], SUBNET_ARRAY[2], SUBNET_ARRAY[3]);

ESP8266WebServer server(PORT);

void setup()
{
    Serial.begin(9600);
    
    WiFi.softAP(SSID, PASSWORD);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", handle_OnConnect);
    server.onNotFound(handle_OnConnect);
    
    WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    free(IP_ARRAY);
    free(GATEWAY_ARRAY);
    free(SUBNET_ARRAY);

    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
    
    server.begin();
    Serial.println("HTTP server started");
}


void loop()
{
  server.handleClient();
}


void handle_OnConnect() {
  wakeMyPC();
  Serial.println("SENDING WOL");
  server.send(200, "text/html", "SENT WOL");
}

void wakeMyPC() {
    WOL.sendMagicPacket(MAC); // Send Wake On Lan packet with the above MAC address. Default to port 9.
}

int *parseIP(char *ip)
{
    int i, *toRet;
    toRet = (int*)malloc(sizeof(int) * 4);
    
    sscanf(ip, "%d.%d.%d.%d", toRet,toRet+1,toRet+2,toRet+3);

    return toRet;
}
