#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// ++++++++++ Neo-Pixel ***************
#define PIN D5
#define NUMPIXELS 15
#define DELAYVAL 100
int R=0;
int G=0;
int B=0;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// ++++++++++ WebServer ++++++++++++
ESP8266WebServer server ( 80 );

//functions
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void httpIndex();


// ++++++++++ Wifi und MQTT Broker +++++++++++++++++
#define MQTT_SERVER "192.168.**.**"
#define SSID "******"
#define PASSWD "*********"

// +++++++++++++ OTA ++++++++++++++++++++++++++++++++
// upload_port und flags müssen evtl. in der platformio.ini freigeschaltet 
#define HOSTNAME "OTA_LED_MONITOR"
#define OTA_PASSWD "Picasso"

// MQTT Subscripe and push topics
#define inTopic_1 "test/for_esp" // originally like this: const char* inTopic ...
#define outTopic_1 "test/from_esp" // originally like this: const char* outTopic...
#define inTopic_2 "test2/from_esp"

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());  // Wofür das denn?

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} // End of function setup_wifi

void callback(char* topic, byte* payload, unsigned int length) {
// wird für die MQTT Kommunitkation benötigt.
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("length: ");
  Serial.println(length);
  for (unsigned int i = 0; i < length; i++)
  { 
    Serial.print((char)payload[i]);
  }
  Serial.println("");

  if((char)payload[3]=='A')
    {
    // +++++++++++++++ Neo-Pixel
    // mit einer 3stelligen Zahl kann über mqtt Farbe und Helligkeit gesteuert werden.
    // ++++++ ROT ++++++++
    switch ((char)payload[0])
    {
    case '0':
    R = 0;
    break;
    case '1':
    R = 25;
    break;
      case '2':
        R = 50;
        break;
      case '3':
        R = 75;
        break;
      case '4':
        R = 100;
        break;
      case '5':
        R = 125;
        break;
      case '6':
        R = 150;
        break;
      case '7':
        R = 175;
        break;
      case '8':
        R = 200;
        break;
      case '9':
        R = 255;
        break;
      default:
        R = 0;
    }
    
    // // ++++++ GRUEN +++++
    switch ((char)payload[1])
    {
      case '0':
        G = 0;
        break;
      case '1':
        G = 25;
        break;
      case '2':
        G = 50;
        break;
      case '3':
        G = 75;
        break;
      case '4':
        G = 100;
        break;
      case '5':
        G = 125;
        break;
      case '6':
        G = 150;
        break;
      case '7':
        G = 175;
        break;
      case '8':
        G = 200;
        break;
      case '9':
        G = 255;
        break;
      default:
        G = 0;
    }

    // +++++++++ BLAU ++++++++
    switch ((char)payload[2])
    {
      case '0':
        B = 0;
        break;
      case '1':
        B = 25;
        break;
      case '2':
        B = 50;
        break;
      case '3':
        B = 75;
        break;
      case '4':
        B = 100;
        break;
      case '5':
        B = 125;
        break;
      case '6':
        B = 150;
        break;
      case '7':
        B = 175;
        break;
      case '8':
        B = 200;
        break;
      case '9':
        B = 255;
        break;
      default:
        B = 0;
    }
  }
} //End of function Callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"***", "***")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      
      // ... and resubscribe
      client.subscribe(inTopic_1);
      // client.subscribe(inTopic_2); // ++++++++++++++++++++++++++ TEST
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} // End of function reconnect

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);

  server.on("/", httpIndex);
  server.begin();
  Serial.println ( "HTTP server started" );

  //+++++++++++ neo pixel +++++++++++++++
  pixels.begin();  
  // +++++++++++ end neo pixel +++++++++++++++++++++++

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWD);
  ArduinoOTA.begin();
}

void loop() {
  if (!client.connected()) 
  {
    reconnect();
  }
    client.loop();
    server.handleClient();
    // ArduinoOTA.handle();
     

  // +++++++++++++++ Neo-Pixel  
  /* pixels.clear(); // Set all pixel colors to 'off' 
  The first NeoPixel in a strand is #0, second is 1, all the way up to the count of pixels minus one.
  pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255 */
  
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...  
  pixels.setPixelColor(i, pixels.Color(R, G, B));
  pixels.show();   // Send the updated pixel colors to the hardware
  //delay(DELAYVAL); // Pause before next pass through loop
  }
} //End of void loop

void httpIndex()
{
  server.send(200, "text/plain", "Hier könnte Ihre Werbung stehen");
}

