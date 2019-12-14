#include <stdlib.h>
#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial esp8266(4, 6); //bylo 2,3
String GET;
int LED=7; //LED PIN USUNAC
int itsONled[] = {0,0}; //USUNAC LED STATUS ARRAY eg- ON or OFF at startup. each value represent ech led.
     
//DHT11 dht11(A0);

String NazwaSieci = "FunBox2-8424"; //Tu podaj nazwe swojej sieci.
String HasloSieci = "7FF3CCAEA329263F3C747ED73A"; //Tu hasło do sieci.
//String NazwaSieci = "AndroidAP8607"; //Tu podaj nazwe swojej sieci.
//String HasloSieci = "eaed34aec159"; //Tu hasło do sieci.

#include "DHT.h"
#define DHT11_PIN 2
DHT dht;
#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float RS_gas = 0;
 float ratio = 0;
 float sensorValue = 0;
 float sensor_volt = 0;
 float R0 = 50000.0;
void setup()
{
	pinMode(LED, OUTPUT);
   Serial.begin(115200);
    esp8266.begin(115200);
	 sendData("AT+RST\r\n",2000,DEBUG); // reset module
     sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as Wireless Station mode
     sendData("AT+CWJAP=\"FunBox2-8424\",\"7FF3CCAEA329263F3C747ED73A\"\r\n", 6000, DEBUG); //Put Your SSID and password if activate as Station mode else comment down the line
     sendData("AT+CIFSR\r\n",2000,DEBUG); // get ip address
     sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
     sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
   // esp8266.println("AT+CIPMUX=1");
    delay(200);
   // connect_c();
    dht.setup(DHT11_PIN);
    if (!bmp.begin())
  {
    Serial.println("Nie odnaleziono czujnika BMP085 / BMP180");
    //while (1) {}
  }

}

void loop(){
  delay(7000); //czekam 15 sekund na polaczenie
  control_from_web();
  char buffer[10];
  char buffer1[10];
  float Btemp, Bhumi;
  /////////////////////////////////
  //Pobranie informacji o wilgotnosci
  int wilgotnosc = dht.getHumidity();
  //Pobranie informacji o temperaturze
  int temperatura = dht.getTemperature();
  String wilgotnosc_str = (String)wilgotnosc;
  String temperatura_str = (String)temperatura;
  String temperatura2_str = (String)bmp.readTemperature();
  String cisnienie_str = (String)(bmp.readPressure()/100);
  
  /*if (dht.getStatusString() == "OK") {
    Serial.print(wilgotnosc_str);
    Serial.print("%RH | ");
    Serial.print(temperatura_str);
    Serial.println("*C");
  }
   // Odczytujemy temperaturę
    Serial.print("Temperatura = ");
    Serial.print(temperatura2_str);
    Serial.println(" *C");
    
    // Odczytujemy cisnienie
    Serial.print("Cisnienie = ");
    Serial.print(cisnienie_str);
    Serial.println(" hPa");
    
    // Obliczamy wysokosc dla domyslnego cisnienia przy pozimie morza
    // p0 = 1013.25 millibar = 101325 Pascal
    Serial.print("Wysokosc = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" metrow");
    
    // Jesli znamy aktualne cisnienie przy poziomie morza,
    // mozemy dokladniej wyliczyc wysokosc, padajac je jako parametr
    Serial.print("Rzeczywista wysokosc = ");
    Serial.print(bmp.readAltitude(101400));
    Serial.println(" metrow");*/

  sensorValue = analogRead(A0);
  sensor_volt = sensorValue/1024*5.0;
  RS_gas = (5.0-sensor_volt)/sensor_volt;
  ratio = RS_gas/R0;
  float x = 1538.46*ratio;
  float ppm = pow(x, -1.709);
  String ppm_str = (String)ppm;
  /*Serial.print("PPM: ");
  Serial.println(ppm_str);*/
  ///////////////////////////////
  //dht11.read(Bhumi, Btemp);
  //delay(10000);
  //String Temp = dtostrf(Btemp, 4, 1, buffer);
  //String Humi = dtostrf(Bhumi, 4, 1, buffer1);
  //updateTemp(Temp); //wysylam dane
  //delay(30000); //czekam 30 sekund do wyslania wilgotnosci
  updateDHT(temperatura_str, wilgotnosc_str); //wysylam dane
  delay(1000);
  updateBMP(temperatura2_str, cisnienie_str);
  delay(1000);
  updateMQ(ppm_str);
  delay(3600); //odczekuje godzine. co tyle wykonuje odczyty
}
/*void updateTemp(String Temp){
  GET = "GET /update?key=";
        GET+= Thingspeak_KEY;
        GET += "&field1=";
  String command = GET;
  command += String(Temp);
  command += "\n\r\n\r";
  esp8266.println("AT+CIPSTART=4, \"TCP\",\"184.106.153.149\",80");
  delay(10000);
  esp8266.println("AT+CIPSEND=4,46");
  delay(3000);
  esp8266.println(command);
  delay(2000);
  esp8266.println("AT+CIPCLOSE");
}*/
void updateDHT(String a, String b){
  GET = "GET /dht.php?temperature="+ a +"&humidity=" +b+" HTTP/1.1";//48
        //GET+= Thingspeak_KEY;
        //GET += "&field2=";
  String command = GET;
//  delay(500);
 command += "\r\n";
// delay(500);
 command += "Host: 192.168.1.15";
// delay(500);
 command += "\r\n\r\n";
// delay(500);
//String command2 = "GET / HTTP/1.0\n\r";
  esp8266.println("AT+CIPSTART=4,\"TCP\",\"192.168.1.15\",80"); //192.168.43.85
  delay(5000);
  esp8266.print("AT+CIPSEND=4,");//bylo 73, 49 //nowe 72
  //delay(500);
  esp8266.println(command.length());  
  
  delay(300);
  esp8266.print(command);
/*  esp8266.print(GET);
  delay(1000);
  esp8266.print("\r\n");
  delay(1000);
  esp8266.print("Host: 192.168.43.85");
  delay(1000);
  esp8266.print("\r\n\r\n");
  delay(1000);*/
  
/** esp8266.print("GET /dht.php?temperature=32&humidity=62 HTTP/1.1");//48
  esp8266.println();
  esp8266.println("Host: 192.168.43.85");//18
    esp8266.println("Connection: close");//17
    esp8266.println();
    esp8266.println();* */
  /*  esp8266.print("GET /dht.php?temperature=32&humidity=62 ");
 // esp8266.print(a);
  esp8266.print("HTTP/1.1 ");
  esp8266.println("Host: 192.168.43.85");
  esp8266.println("Connection: close");
  esp8266.println();
  esp8266.println();*/
  delay(3000);
  esp8266.println("AT+CIPCLOSE=4");/// BYLO BEZ =4
}
void updateBMP(String a, String b){
  GET = "GET /bmp.php?temperature_bmp="+a+"&humidity_bmp="+b+" HTTP/1.1";//48
        //GET+= Thingspeak_KEY;
        //GET += "&field2=";
  String command = GET;
//  delay(500);
 command += "\r\n";
// delay(500);
 command += "Host: 192.168.1.15";
// delay(500);
 command += "\r\n\r\n";
// delay(500);
//String command2 = "GET / HTTP/1.0\n\r";
  esp8266.println("AT+CIPSTART=4,\"TCP\",\"192.168.1.15\",80");
  delay(5000);
  esp8266.print("AT+CIPSEND=4,");//bylo 73, 49 //nowe 72
  //delay(500);
  esp8266.println(command.length());  
  
  delay(300);
  esp8266.print(command);

  delay(3000);
  esp8266.println("AT+CIPCLOSE=4");/// BYLO BEZ =4
}
void updateMQ(String a){
  GET = "GET /mq.php?ppm="+a+" HTTP/1.1";//48
        //GET+= Thingspeak_KEY;
        //GET += "&field2=";
  String command = GET;
//  delay(500);
 command += "\r\n";
// delay(500);
 command += "Host: 192.168.1.15";
// delay(500);
 command += "\r\n\r\n";
// delay(500);
//String command2 = "GET / HTTP/1.0\n\r";
  esp8266.println("AT+CIPSTART=4,\"TCP\",\"192.168.1.15\",80");
  delay(5000);
  esp8266.print("AT+CIPSEND=4,");//bylo 73, 49 //nowe 72
  //delay(500);
  esp8266.println(command.length());  
  
  delay(300);
  esp8266.print(command);

  delay(3000);
  esp8266.println("AT+CIPCLOSE=4");/// BYLO BEZ =4
}
/*void connect_c(){
  esp8266.println("AT+CWMODE=1");
  delay(2000);
        String cmd = "AT+CWJAP=\""+NazwaSieci+"\", \""+HasloSieci+"\"";
        delay(200);
  esp8266.println(cmd);
  delay(2000);

}*/

void control_from_web()
{
     
  if(esp8266.available()) // check if the esp is sending a message 
  {
    if(esp8266.find("+IPD,"))
    {
     // subtract 48 because the read() function returns 
     // the ASCII decimal value and 0 (the first decimal number) starts at 48
     int connectionId = esp8266.read()-48; 
     //To read the url sent by the client
     String msg;
     esp8266.find("?");
     delay(100);
     msg = esp8266.readStringUntil(' ');
     String command1 = msg.substring(0);
     // HTML START
     String webpage = "<html><head><title>M&M control</title>";
     webpage += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><style>.button {background-color: rgb(61, 58, 65);border: none;color: white;padding: 14px 20px;text-align: center;display: inline-block;font-size: 16px;} .centre {text-align: center;}</style>";
     webpage += "</head><body class=\"centre\"><h1 class=\"centre\">check</h1>";
     //COMMANDS TO TURN ON or OFF LED RECEIVE BY WEB
                  if (command1 == "T"){//if recieve T from url it will turn on led by checking value in itsONled, if value is 0 it means on, if find 1 it will switch off led.
                    if (itsONled[1] == 1) // if find value 1 it will turn of led
                    {
                      digitalWrite(LED, LOW);
                      itsONled[1] = 0; // set value 0
                      webpage += "<p>LED STATUS OFF</p>";
                    }
                    else
                    {// if find value 0 it will turn on led
                      digitalWrite(LED, HIGH);
                      itsONled[1] = 1;// set value 1
                      webpage += "<p>LED STATUS ON</p>";
                    }
                }
     webpage += "<a class=\"button\" href=\"?T\">TEMPERATURE</a></body></html>";
     //webpage += "<a class=\"button\">HUMIDITY</a>";
     //webpage += "<a class=\"button\">ATMOSPHERIC PRESSURE</a>";
     //webpage += "<a class=\"button\">CO CONCENTRATION</a></body></html>";
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     sendData(cipSend,200,DEBUG);
     sendData(webpage,200,DEBUG);
     //BELOW THIS LINE CLOSE THE CONNECTION
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,300,DEBUG);
    }
  }
     
}
//PROGRAM TO SEND COMMAND TO ESP8266 
void sendData(String command, const int timeout, boolean debug)
{
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();

    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        Serial.write(esp8266.read());
      }  
    }
}

