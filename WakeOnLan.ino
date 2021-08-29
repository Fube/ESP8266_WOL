#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <string.h>

#include "config.h" // Make your own config.h file containing your SSID, PASSWORD, IP, GATEWAY, SUBNET, BOT_TOKEN, and CHANNEL_ID
#include "utils.h"

// Prototypes
void wakeMyPC();
// End of prototypes

WiFiUDP UDP;
WakeOnLan WOL(UDP);
char lastSeen[128], currentId[128];

void setup()
{
    lastSeen[127] = currentId[127] = '\0';
    Serial.begin(9600);
    
    WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.

    WiFi.mode(WIFI_STA);
    WiFi.begin(_SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).

    
    Serial.println("Setup done");
}


void loop()
{
    
    getLatestMessage(BOT_TOKEN, CHANNEL_ID, currentId);

    if(strcmp(lastSeen, currentId) != 0)
    {
      wakeMyPC();
      strcpy(lastSeen, currentId);
      Serial.println("Waking up...");
    }

    delay(5000);
}

void wakeMyPC() {
    WOL.sendMagicPacket(MAC); // Send Wake On Lan packet with the above MAC address. Default to port 9.
}
