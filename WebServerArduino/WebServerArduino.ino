#include <WiFiNINA.h>
#include <ArduinoJson.h>

char ssid[] = "BELL297";             //  your network SSID (name) between the " "
char pass[] = "E2D4A674112F";      // your network password between the " "
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

WiFiClient client = server.available();

int ledPinR = 10;
int ledPinG= 6;
int ledPinB = 5;

void setup() {
  Serial.begin(9600);
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  while (!Serial);
  
  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();

}

void loop() {
  client = server.available();

  if (client) {
    printWEB();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB() {

  String newLine="";

  if (client) {                             // if you get a client,
    String currentLine = "{";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
          if (c == '{') {    // if you got anything else but a carriage return character,
            while(c){
              if(c=='}') break;
              else{
                c=client.read();
                currentLine += c;
              }
            }
          }

        if(c=='}') break;
      }
    }
    
    currentLine+="}}";
    Serial.println(currentLine);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");

    newLine=currentLine;
  }

  DynamicJsonDocument doc(100);

  deserializeJson(doc, newLine);

  long red=doc["params"]["rgbVal"]["r"];
  long green=doc["params"]["rgbVal"]["g"];
  long blue=doc["params"]["rgbVal"]["b"];

  analogWrite(ledPinR, red);
  analogWrite(ledPinG, green);
  analogWrite(ledPinB, blue);
}