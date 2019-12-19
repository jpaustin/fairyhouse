/*********
 This code created by Jim Nissen for the Fairy House Controller AP. This is a modification of example code from https://randomnerdtutorials.com  
 
 Jim Nissen = jnissen@austin.rr.com

 October 15, 2019
 Fairy House Controller - Used to control four channels for automating the lighting and smoke (chimney) for a Fairy House

 The electronics uses an ESP8266 WeMos Mini D1 Pro board. The pro board has an external WiFi antenna to provide greater coverage area.
 The WeMos D1-Mini port pins = D2 (Smoke), D3 (LED1), D5 (LED2), D6 (LED3) and D4 (Built in LED). Each of the four external outputs are buffered
 by high current solid state switches capable of supporting upto 1A of current each. However the support circuitry is limited so do not load
 the controller any more than 1.3A total current @ 5V output. All outputs are limited to only 5V. LED's must be capable of handling 5V with the 
 use of a dropping resistor. Fairly lights often have the resistor built into the string. If the LED's do not have a resistor in series use a 330 ohm 1/8W
 resistor. 

 The Fairy controller acts like an access point. Once powered on the AP name is FairyHouse. The password is 12345678.

 To use - Connect to the Fairy House Access Point SSID on your phone or tablet.
 Provide the password the first time logging in.

 Open a Web Browser and type in 192.168.4.1 to access the Fairy Controller Web Page. 

 Once the web page is open six buttons are shown. 4 buttons are for the external peripherals (Smoke + 3 LED channels). The fifth button at the bottom
 is a LED test button. The test button turns the built in blue LED on the WeMos board ON or OFF. Always a good idea to test to make sure the phone and 
 controller are communicating properly. The sixth buttun was added late to allow for automatic ON/OFF lighting. Only applies to light channels.

 Try to use only one phone to control the Fairy House. The AP will accept upto 5 simultaneous connections but the control web page is not very sophisticated so 
 it's simpler to use one phone/tablet to control the device. Multiple phones/tablets will work but some of the displayed state of the buttons will not be 
 preoperly reflected on the page if the pages have been cached by your phones browser. Simpler to use a single phone as a result. 

 Automatic lights on at dusk. Added a photo sensor that will be available on the A0 (Analog channel). Will be used to automate the lights 
 at night so they will turn on automatically as dusk. If not selected on the web page this feature will be OFF. Default is to be off.  
  
 Added ability to update the code later with the web-ota code. To program new code requires 192.168.4.1:8080/webota

 Then follow the instuctions and upload the new file. 
  
*/

// Load Wi-Fi library and OTA support
#include <ESP8266WiFi.h>
#include <WebOTA.h>

// Replace with your network credentials
// to create its own wifi network
const char* ssid     = "FairyHouse";
const char* password = "YOUR_PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
// Pins changed for esp8266
String output1State = "off";
String output2State = "off";
String output3State = "off";
String output4State = "off";
String output5State = "off";
String input1State = "off";

// Assign output variables to GPIO pins - ESP8266 Wemos Mini Pro
const int output1 = D1;
const int output2 = D2;
const int output3 = D5;
const int output4 = D6;
const int output5 = D4;

// Assign light sensor to analog input pin
const int analogInPin = A0;  
int sensorValue = 0;         // value read from the light sensor
int count = 0;               // Loop count 
int night = 0;               // If night = 0 then it's light out. If night = 1 then dark. 
int mask = 0;                // Indicator for the auto button
int out2 = 0;                // LED1 (OUTPUT2) indicator or pin state. Faster than reading pin
int out3 = 0;                // LED2 (OUTPUT3) indicator or pin state.
int out4 = 0;                // LED3 (OUTPUT4) indicaotr or pin state.

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(output5, OUTPUT);
   
  // Set outputs to high because we are using active NMOS fet switches. A LOW means the switch is OFF.
  // The LED is directly connected to a port pin. A LOW on the port will turn ON the LED. Default to HIGH.
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(output3, LOW);
  digitalWrite(output4, LOW);
  digitalWrite(output5, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  //server.begin();
  
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  webota.handle();

  
  // Add analog read section to determine if the light sensor is LOW enough to turn on the lights automatically
  // Normal analog read tends to take too long and causes WiFi connection issues. 
  // Added count to read ananlog pin every 100K loops. End up beng about once every 1.5 seconds. 
  //
  // Sensor is a simple light dependent resistor (LDR). These sensors are low cost and work well. 
  // See here: http://www.esp8266learning.com/wemos-ldr-example.php
  //
  
  and the  
  if (count == 100000) {
  sensorValue = analogRead(analogInPin);
  Serial.println(sensorValue);
  if (sensorValue <= 90){
    night = 1;                              // Low light - Night set to 1
  } else {
    night = 0;                              // Daylight - Night set to 0
  }
  count = 0;
   
  // Check LED state and mask off if auto mode and day
  if (out2 == 1 && mask == 1 && night == 0) {
    digitalWrite (output2, LOW);
    Serial.println("LED1 is going to auto day off");
  }
  if (out3 == 1 && mask == 1 && night == 0) {
    digitalWrite (output3, LOW);
    Serial.println("LED2 is going to auto day off");
  }
  if (out4 == 1 && mask == 1 && night == 0) {
    digitalWrite (output4, LOW);
    Serial.println("LED3 is going to auto day off");
  }

  //Now see if it should come back on at night
  else if (out2 ==1 && mask == 1 && night == 1) {
   digitalWrite (output2, HIGH);
   Serial.println("LED1 is going to auto day on");
  }
  if (out3 == 1 && mask == 1 && night == 1) {
   digitalWrite (output3, HIGH);
   Serial.println("LED2 is going to auto day on");
  }
  if (out4 == 1 && mask == 1 && night == 1) {
   digitalWrite (output4, HIGH);
   Serial.println("LED3 is going to auto day on");
  } 

  //Now see if auto is off but light buttons still on
  else if (out2 ==1 && mask == 0) {
   digitalWrite (output2, HIGH);
   Serial.println("LED1 is going back to manual ON");
  }
  if (out3 == 1 && mask == 0) {
   digitalWrite (output3, HIGH);
   Serial.println("LED2 is going back to manual ON");

  }
  if (out4 == 1 && mask == 0) {
   digitalWrite (output4, HIGH);
   Serial.println("LED3 is going back to manual ON");
  } 
  
  else
  Serial.print(mask);
  Serial.print(out2);
  Serial.print(out3);
  Serial.print(out4);
  Serial.println(night);
  
  } else {
  count ++;
  }
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                      
            // turns the GPIOs on and off
            //for GPIO5 (D1) or SMOKE
            if (header.indexOf("GET /O5/on") >= 0) 
            {
              Serial.println("SMOKE is on");
              output1State = "on";
              digitalWrite(output1, HIGH);
            } 
            else if (header.indexOf("GET /O5/off") >= 0) 
            {
              Serial.println("SMOKE is off");
              output1State = "off";
              digitalWrite(output1, LOW);
            }

            //for GPIO4 (D2)or LED1
            else if (header.indexOf("GET /O4/on") >= 0)
            {
              Serial.println("LED1 on");
              output2State = "on";
              digitalWrite(output2,HIGH);
              out2 = 1;
            }
            else if (header.indexOf("GET /O4/off") >= 0)
            {
              Serial.println("LED1 off");
              output2State = "off";
              digitalWrite(output2, LOW);
              out2 = 0;
            }
            
          
             //for GPIO14 (D5) or LED2
             else if (header.indexOf("GET /O14/on") >= 0)
            {
              Serial.println("LED2 on");
              output3State = "on";
              digitalWrite(output3, HIGH);
              out3 = 1;
            } 
            else if (header.indexOf("GET /O14/off") >= 0)
            {
              Serial.println("LED2 off");
              output3State = "off";
              digitalWrite(output3, LOW);
              out3 = 0;
            }
            
            //for GPIO12 (D6) or LED3
            else if (header.indexOf("GET /O12/on") >= 0)
            {
              Serial.println("LED3 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
              out4 = 1;
            } 
            else if (header.indexOf("GET /O12/off") >= 0)
            {
              Serial.println("LED3 off");
              output4State = "off";
              digitalWrite(output4, LOW);
              out4 = 0;
            }

            //for GPIO2 (D4 - Built in LED)
            else if (header.indexOf("GET /O2/on") >= 0)
            {
              Serial.println("GPIO O2 on");
              output5State = "on";
              digitalWrite(output5, LOW);
            } 
            else if (header.indexOf("GET /O2/off") >= 0)
            {
              Serial.println("GPIO O2 off");
              output5State = "off";
              digitalWrite(output5, HIGH);
            }

            //for Analog sensor - A0 or Light dependent resistor
            else if (header.indexOf("GET /I1/on") >= 0)
            {
              Serial.println("Automatic I1 on");
              input1State = "on";
              mask = 1;
            } 
            else if (header.indexOf("GET /I1/off") >= 0)
            {
              Serial.println("Automatic I1 off");
              input1State = "off";
              mask = 0;
            }
           

          
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; border-radius: 10px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
                        
            // Web Page Heading
            client.println("<body><h1>Fairy Controller</h1>");
                           
            // Display current state, and ON/OFF buttons for GPIO 5 (D1)  
            client.println("<p>Smoke Machine - " + output1State + "</p>");
            // If the output1State is off, it displays the ON button       
            if (output1State=="off") {
              client.println("<p><a href=\"/O5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/O5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
                      
            // Display current state, and ON/OFF buttons for GPIO 4 (D2)  
            client.println("<p>LED 1 - " + output2State + "</p>");
            // If the output2State is off, it displays the ON button       
            if (output2State=="off") {
              client.println("<p><a href=\"/O4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/O4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 14 (D5) 
            client.println("<p>LED 2 - " + output3State + "</p>");
            // If the output3State is off, it displays the ON button       
            if (output3State=="off") {
              client.println("<p><a href=\"/O14/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/O14/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 12 (D6)  
            client.println("<p>LED 3 - " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/O12/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/O12/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 2 (D4)  
            client.println("<p>TEST LED - " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/O2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/O2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for Automatic ON/OFF Analog (A0)  
            client.println("<p>Lights - Auto on at dusk - " + input1State + "</p>");
            // If the input1State is off, it displays the ON button       
            if (input1State=="off") {
              client.println("<p><a href=\"/I1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/I1/off\"><button class=\"button button2\">OFF</button></a></p>");
            }


          client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
