/* Create a WiFi access point and provide a web server on it. */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//define o IP fixo
IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

const char *ssid = "SSID";
const char *password = "senha";
const int relePin = D1; // an LED is connected to NodeMCU pin D1 (ESP8266 GPIO5) via a 1K Ohm resistor
int releState = LOW;
long intervalo = 500;
bool toggle = 0; //
bool ledState = HIGH;

char releText[80];// texto botão

//--------------------------------------------------------------

void blinkLED(void)// pisca led para indicar a placa esta funcioando
{
  static unsigned long ult_tempo = 0;
  int tempo = millis();
  if (tempo - ult_tempo > 5000) {
    ledState = LOW; // liga led
    ult_tempo = tempo;
  } else if (tempo - ult_tempo > 90) {
    ledState = HIGH;//desliga led
  }
  digitalWrite(LED_BUILTIN, ledState);
}



void toggleRele(int Pin) // alterna o status do rele (função com problema)
{
  digitalWrite(Pin, HIGH);
  delay(400);
  digitalWrite(Pin, LOW);
}
//******************html****************
  const char MAIN_page[] PROGMEM = R"=====(
<html>
  <head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <meta charset='utf-8'/>
    <title>Portão</title>
    <style>
       
      header { position: fixed;  left: 0;  top: 0;   width: 100%; Color: #ffffff; background-color: #7422c7; font-weight: bold; font-family: Arial, Helvetica, Sans-Serif; font-size: 250%;}
      body { background-color: #ffffff; text-align: center;  Color: #000000; }
      button { background:linear-gradient(to bottom, #892ce6 5%, #892ce6 100%); background-color:#892ce6; margin-top: 20%; border-radius:16px; border:3px solid #5c0fa8;  cursor:pointer; color:#ffffff; font-family:Arial; font-size:28px; padding:3% 15%; text-decoration:none;}
      button:hover { background:linear-gradient(to bottom, #476e9e 5%, #6b0a61 100%);background-color:#892ce6;}
      footer {Color: #ffffff; background-color: #7422c7;   position: fixed;  left: 0;  bottom: 0;   width: 100%;}
     </style>
  </head>
  <header>Jaboticaba control</header>
  <body>
   <form action='/' method='POST'><button name='rele'>Abre/fecha</button></form>
   </body>
   <footer>Tiago Batista 03/2020 versão: beta<footer>
</html>
)====="; 

//--------------------------------------------------------------

void handleRele() { // pagina principal
   String h = MAIN_page;
  if ( server.hasArg("rele") ) {
    digitalWrite (LED_BUILTIN, ledState); //turn the built in LED on pin DO of NodeMCU on
    toggleRele(relePin);
     digitalWrite ( LED_BUILTIN, !ledState );
     server.send (200, "text/html", h);
  }
  server.send (200, "text/html", h);
}


void handleNotFound() { // pagina de erro
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, ledState ); //turn the built in LED on pin DO of NodeMCU off
}

void setup() {
  pinMode ( relePin, OUTPUT );
  pinMode ( LED_BUILTIN, OUTPUT );
  digitalWrite ( relePin, 0 );

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  //  WiFi.mode(WIFI_AP_STA);
  //  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);// ip fixo
  /* You can remove the password parameter if you want the AP to be open. */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(F("*WifiRTC: Iniciando UDP"));

  server.on ("/", handleRele);
  server.on ("rele", handleRele);

  server.onNotFound ( handleNotFound );

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  blinkLED(); // pisca o led
  server.handleClient();
  yield();
}
