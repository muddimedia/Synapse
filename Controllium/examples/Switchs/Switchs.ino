/*************************************************************************************************************************
    DEVELOPED BY:   ARTUR FERREIRA MOREIRA
    DATE:           MAY, 13 OF 2017
    EMAIL:          artur31415926@gmail.com
    WEBPAGE:        http://daedalusstone.com/
    INSTAGRAM:      https://www.instagram.com/artur31415/
    GOOGLEPLAY:     https://play.google.com/store/apps/developer?id=Synapse
    YOUTUBE:        https://www.youtube.com/channel/UC6blOB30re0J-Oiksqaob1g/videos
    GITHUB:         https://github.com/artur31415
    TWITTER:        https://twitter.com/artur31415
    LINKEDIN:       https://www.linkedin.com/in/artur31415

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
[ENG-USA]
      This library was developed to work with the Android app "Controllium", with which,
    you can control your ESP8266 projects using your Android device.
      Through the library you can attach any of the UI elements to the Arduino hardware pins
    accordingly to theirs functionalities.
      You can use widgets to send and receive data from the ESP8266 using you Android Device.
      Look at the examples to understand how the library works.

      Any Ideas or criticism, email me.

    Have fun!
[PT-BR]
      Esta biblioteca foi desenvolvida para trabalhar com o aplicativo Android "Controllium", com o qual,
    Você pode controlar seus projetos de ESP8266 usando seu dispositivo Android.
      Através da biblioteca você pode anexar qualquer um dos elementos UI para os pinos de hardware arduino
    De acordo com s  uas funcionalidades.
      Voce pode usar widgets para enviar e receber dados do ESP8266 usando o seu dispositivo Android.
    Veja os exemplos para entender como funciona a biblioteca.

      Quaisquer idéias ou críticas, email me.

    Divirta-se!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                    Switchs                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
[ENG-USA]
    In this example the switchs that are on the controller, on the Controllium Android app, are used to control some of
    the ESP8266's pins

[PT-BR]
    Neste exemplo os switchs que estão no controle, no aplicativo Android Controllium, são usados para controlar alguns
    dos pinos do ESP8266.
**************************************************************************************************************************/


#include <Controllium.h>

#include <ESP8266WiFi.h>

//Wifi SSID and Pass
const char* ROUTER_SSID = "YOUR_SSID";
const char* ROUTER_PASS = "YOUR_PASS";

//Soft AP SSID and Pass
char* SOFT_AP_SSID = "Controllium";
char* SOFT_AP_PASS = "0123456789";

//ESP8266 DIGITAL PINS
int D[] = {16, 5, 4, 0, 2, 14, 12, 13, 15};

//variable used to control the blinking of the "alive" led
long ledBlinkLastTime = 0;


//Controllium object
Controllium controllium("ESP_Switchs");//Name of this device, if you have more than one connected, provide a different name to each

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                        UTILS FUNCTIONS BEGIN                                                  /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Jusy a function to blink a pin
void blink(int pin)
{
  for(int i = 0; i < 2; ++i)
  {
    digitalWrite(pin, LOW);
    delay(50);
    digitalWrite(pin, HIGH);
    delay(50);
  }

  digitalWrite(pin, LOW);
  delay(100);
}

//Setup Soft AP
void setupAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  randomSeed(analogRead(A0) + analogRead(A0));
  sprintf(SOFT_AP_SSID, "%s_%04d", SOFT_AP_SSID, random(1023));

  WiFi.softAP(SOFT_AP_SSID, SOFT_AP_PASS, 6);
}

//Try to connects to WIFI
//If it fails, returns false
//Otherwise, returns true
bool connectToWifi()
{
  WiFi.begin(ROUTER_SSID, ROUTER_PASS);

  // Wait for connect to AP
  Serial.print("\n\nConnecting to ");

  Serial.println(ROUTER_SSID);

  int tries=0;

  while (WiFi.status() != WL_CONNECTED)
   {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30)
    {
      break;
    }
  }

  Serial.println();

  if (tries > 30)
  {
    Serial.println("Failed To Connect, initializing the Soft AP!\n\n");
    return false;
  }

  return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                        UTILS FUNCTIONS BEGIN                                                  /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  //Init the serial for debug
  Serial.begin(115200);

  //Init the ESP pins
  for(int i = 0; i < 9; ++i)
  {
    pinMode(D[i], OUTPUT);
    digitalWrite(D[i], LOW);
  }

  if(connectToWifi())//Try to connect to WIFI
  {
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    digitalWrite(D[1], HIGH);// Pin to show that it is connected to wifi
  }
  else//Otherwise, setup a soft AP
  {
    setupAP();

    Serial.println("Soft AP On!");

    Serial.print("\nSSID = ");
    Serial.println(SOFT_AP_SSID);
    Serial.print("PASS = ");
    Serial.println(SOFT_AP_PASS);

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    digitalWrite(D[2], HIGH);// Pin to show that it is NOT connected to wifi
  }

  //Begin server
  Serial.print("\nUdp server started at port ");
  Serial.println(UDP_DEFAULT_PORT);
  controllium.BeginUdpServer(UDP_DEFAULT_PORT);

  //Instantiate Controllium Devices and add them to the controllium object
  ControlliumDevice switch0("SWITCH_0", DEVICE_TYPE_BUTTON, "0");
  ControlliumDevice switch1("SWITCH_1", DEVICE_TYPE_BUTTON, "0");
  ControlliumDevice switch2("SWITCH_2", DEVICE_TYPE_BUTTON, "0");
  ControlliumDevice switch3("SWITCH_3", DEVICE_TYPE_BUTTON, "0");

  controllium.AddDevice(switch0);
  controllium.AddDevice(switch1);
  controllium.AddDevice(switch2);
  controllium.AddDevice(switch3);

  Serial.println("Waiting for clients to connect...");
}

void loop()
{
  //Just a blinking led to show that the device is alive
  if(millis() - ledBlinkLastTime > 2000)
  {
    ledBlinkLastTime = millis();
    blink(D[3]);
  }

  if(controllium.Update(200))//Returns true if a new msg was received
  {
    Serial.println("New MSG!\n\n");


    //Set pins states using the switchs states
    float switch0Value = (float)controllium.GetDevice("SWITCH_0")->GetDoubleValue();
    float switch1Value = (float)controllium.GetDevice("SWITCH_1")->GetDoubleValue();
    float switch2Value = (float)controllium.GetDevice("SWITCH_2")->GetDoubleValue();
    float switch3Value = (float)controllium.GetDevice("SWITCH_3")->GetDoubleValue();

    if(switch0Value >= 0)
      digitalWrite(D[4], (int)switch0Value);

    if(switch1Value >= 0)
      digitalWrite(D[5], (int)switch1Value);

    if(switch2Value >= 0)
      digitalWrite(D[6], (int)switch2Value);

    if(switch3Value >= 0)
      digitalWrite(D[7], (int)switch3Value);
  }

}
