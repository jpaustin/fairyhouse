# fairyhouse
An ESP8266 access point (AP) type web server that controls 4 independent outputs

This controller was initially desgined for my sisters hobbies. She likes to create miniature dioramas that appear to be Fairy Houses. As she added more and more details to these outdoor creations she started to add lights and a smoke generator to the intricate facades. I created this controller to allow her to turn on or off the various lights or smoke generator from a web page that is being served from the controller. This is hosted on an ESP8266 controller. I used the Arduino development system to write and test the code. 

In order to fully understnd the controller you have to understand how it's powered. The fairy controller is powered from some landscape lighting that was available outdoors. Landscape lighting varies but this version was a 14V AC transformer that only comes on at night. In order to power an ESP8266 controller (3.3V/5V DC) from this source you have to be a bit creative. Recall power is only available at night and there was a desire to power the fairy house in the day as well. Yes a 120V AC line could have been run outside to the tree in question but that would involve an electrician or an ugly extension cord running across the yard. A decision was made to use the existing low voltage AC power for the landscape lights. The 14V AC power can work but you need to rectify the AC and convert it to a form of DC voltage. A 5A bridge rectifier was used and a 1000uF smoothing capacitor was employed on the DC side. This dirty DC power was run into a DC to DC converter like this:  https://www.amazon.com/Converter-Regulator-Adapter-Reducer-Electronics/dp/B07Q5W1BG3/ref=sr_1_1?keywords=12V+to+5V+potted+egulator&qid=1576704803&sr=8-1-spell  The output of this DC/DC regulator is 5V and is very clean. Perfect for a project except for the fact that in the day time the power will go away! To overcome that a UPS type circuit is emplyed. 

14VAC ----Bridge Rectifier and CAP to GND-----12/24V to 5V Regulator -----Powerboost 1000 -------ESP8266 Controller
3.7V 10,000maH LiPo Battery ----------------------------------------------------/

An Adafruit Power Boost 1000 https://www.adafruit.com/product/2465 is the UPS like circuit. This is a strange little board that does multiple things. If you have input power available the output will be supplied to your project and the LiPo battery will be charged at the same time. If power goes away the Adafruit device will switch (glitch free) to the battery to power the load. The output is 5.2V @ 1.4A maximum so that needs to be kept in mind if you load the Fairy Controller with lots of loads. 
                                              
The controller is an ESP8266 WeMos Mini Pro board: https://www.amazon.com/External-Antenna-Connector-ESP-8266EX-Development/dp/B07T1D6G6Z/ref=sr_1_11?keywords=wemos+mini+pro&qid=1576705909&sr=8-11 This board has a very large 16Mb of program memory and the board has an external WiFi antenna connector option. If you use the external antenna make sure you modify the board by moving the small surface mount jumper. 

The code is written so that the ESP8266 creates an access point. Once powered ON you can connect to this WiFi device with your phone or a local computer. See the detailed operating instructions included.  


The code is called 4_CH_AP (4 channel access point). It serves the built in web page and allows for control of the hardware. The code is written to allow for over the air (OTA) updates later on. If you desire to update the code then follow the link shown in the program and supply the binary compiled file to load. 

The hardware, besides the power listed above, includes four power transistors (NMOS protected switches) to drive the loads. The ESP8266 can only drive a few milliamps of current. The drivers will supply the loads with a path to GND. NMOS switches are not used on the positive rail but for the ground side of a circuit. Two pins per channel are provided. +5V and the switched GND via the transitor. Do not drive the LED's directly from the port pins or you can damage the conttroller. The smoke machine requires almost 1A of current alone so there is no other way to use that but employ a power transistor. 

Update - Uploaded a simple schematic.



