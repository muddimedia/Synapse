/*************************************************************************************************************************
    DEVELOPED BY:   ARTUR FERREIRA MOREIRA
    DATE:           MARCH, 28 OF 2017
    EMAIL:          artur31415926@gmail.com
    WEBPAGE:        http://asgardlab.netne.net/
    INSTAGRAM:      https://www.instagram.com/artur31415/
    GOOGLEPLAY:     https://play.google.com/store/apps/developer?id=Synapse
    YOUTUBE:        https://www.youtube.com/channel/UC6blOB30re0J-Oiksqaob1g/videos
**************************************************************************************************************************/

#include <Controllium.h>

#include <ESP8266WiFi.h>

const char* ssid = "YOUR_SSID";
const char* pass = "YOUR_PASS";

//ESP8266 DIGITAL PINS
int D[] = {16, 5, 4, 0, 2, 14, 12, 13, 15};

long lastTime = 0;
long lastTimeUIBlink = 0;

int LedAnimationCounter = 1;

Controllium controllium("Led Matrix");//Name of this device, if you have more than one connected, provide a different name to each

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                              FUNCTIONS BEGIN                                                  /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                              FUNCTIONS END                                                    /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup()
{
  Serial.begin(115200);

  for(int i = 0; i < 6; ++i)
  {
    pinMode(D[i], OUTPUT);
    digitalWrite(D[i], LOW);
  }

   // setting up Station AP
  WiFi.begin(ssid, pass);

  // Wait for connect to AP
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  int tries=0;
  while (WiFi.status() != WL_CONNECTED) {
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
    Serial.println("Failed To Connect!");
    int animationDelay = 50;
    while(1)
    {
      delay(200);

      for(int i = 0; i < 5; ++i)
      {
        digitalWrite(D[i], HIGH);
        delay(animationDelay);
        digitalWrite(D[i], LOW);
        delay(animationDelay);
      }

      delay(200);

      for(int i = 4; i >= 0; --i)
      {
        digitalWrite(D[i], HIGH);
        delay(animationDelay);
        digitalWrite(D[i], LOW);
        delay(animationDelay);
      }
    }
  }


  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Udp server started at port ");
  Serial.println(UDP_DEFAULT_PORT);

  controllium.BeginUdpServer(UDP_DEFAULT_PORT);


  controllium.AddDevice(ControlliumDevice::FromString("B|SWITCH_0|0"));

  controllium.AddDevice(ControlliumDevice::FromString("L|LED_0|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_1|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_2|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_3|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_4|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_5|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_6|0"));
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_7|0"));\
  controllium.AddDevice(ControlliumDevice::FromString("L|LED_8|0"));
}

void loop()
{
  if(millis() - lastTime > 2000)
  {
    lastTime = millis();

    digitalWrite(D[5], LOW);
    blink(D[5]);

    Serial.print("ClientsCount = ");
    Serial.print(controllium.NumberOfClients());
    Serial.println("\n");

    if(controllium.NumberOfClients() > 0)
    {
      for(int i = 0; i < controllium.NumberOfClients(); ++i)
      {
        Serial.print("DEVICE ");
        Serial.print(i);
        Serial.print(":\t\t");
        Serial.print(controllium.GetDevice(i)->GetName());
        Serial.print("\t\tValue = ");
        Serial.println(controllium.GetDevice(i)->GetValueToString());
      }
    }




    Serial.println("=======================================\n");

  }

  if(controllium.Update(200))
  {
    Serial.print("DeviceCount = ");
    Serial.println(controllium.NumberOfDevices());

    for(int i = 0; i < controllium.NumberOfDevices(); ++i)
    {
      Serial.print("DEVICE ");
      Serial.print(i);
      Serial.print(":\t\t");
      Serial.print(controllium.GetDevice(i)->ToString());
    }
  }

//SWITCH TO pin
  float val = (float)controllium.GetDevice(0)->GetDoubleValue();
  if(val >= 0)
    digitalWrite(D[0], (int)val);


  //LED_0 TOGGLE
  if(millis() - lastTimeUIBlink >= 1000)
  {
    lastTimeUIBlink = millis();


    controllium.GetDevice(LedAnimationCounter)->ToggleState();
    if(controllium.GetDevice(LedAnimationCounter)->GetDoubleValue() == 0 && controllium.GetDevice(LedAnimationCounter)->GetPreviousDoubleValue() == 0)
    {
      ++LedAnimationCounter;
      if(LedAnimationCounter >= 8)
        LedAnimationCounter = 0;
    }
  }
}
