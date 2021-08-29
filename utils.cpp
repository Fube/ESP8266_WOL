#include "utils.h"

const char* host = "discord.com";
const int httpsPort = 443; //ssl port
boolean isSetUp = false;
WiFiClientSecure client;

X509List cert(CERT);

void setUp() {
    
    if(isSetUp)return;
    
    client.setNoDelay(1);
    
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    
    client.setTrustAnchors(&cert);
    
    isSetUp = true;
}

void apiRequest(String &response, String apiEndPoint, String botToken){
    setUp();
    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      return;
    }
    
    // Send get request to client
    client.print(String("GET ") + apiEndPoint + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Accept: */*\r\n" +
                  "Authorization:Bot " + botToken + "\r\n" +
                  "User-Agent:ESP8266_WOL (https://github.com/Fube/ESP8266_WOL, v0.1)\r\n" +
                  "Content-Type: application/json\r\n" + 
                  "Connection: close\r\n\r\n");
  
    // Get body
    String received = "";
    while(client.connected()){
      String line = client.readStringUntil('\r');
      if(line.length() == 1){ // Check for blank line between headers and body
        break;
      }
    }
    
    client.readStringUntil('\r'); // Idk why there's a number above the body but we shall ignore it
    response = client.readStringUntil('\r');
}

//gets the lastest message from the bot
String getLatestMessage(String botToken, String channelId, char * destId){

    //get channel info
    String payload;
    String getchannelinfo = "/api/channels/" + channelId;
    apiRequest(payload, getchannelinfo, botToken);
    
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    String lastMessageId = doc["last_message_id"];
    
    String getlastmessage = "/api/channels/" + channelId + "/messages/" + lastMessageId;
    apiRequest(payload, getlastmessage, botToken);
    deserializeJson(doc, payload);
    
    String content = doc["content"];
    auto id = doc["id"].as<const char*>();
    strcpy(destId, id);

    return content;
}
