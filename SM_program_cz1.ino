#include <stdlib.h>
#include <SoftwareSerial.h>

SoftwareSerial monitor(4, 6); //bylo 2,3
String GET;
//DHT11 dht11(A0);

/*String NazwaSieci = "FunBox2-8424"; //Tu podaj nazwe swojej sieci.
String HasloSieci = "7FF3CCAEA329263F3C747ED73A"; //Tu hasło do sieci.*/
String NazwaSieci = "AndroidAP8607"; //Tu podaj nazwe swojej sieci.
String HasloSieci = "eaed34aec159"; //Tu hasło do sieci.

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
   Serial.begin(115200);
    monitor.begin(115200);
    monitor.println("AT+CIPMUX=0");
    delay(200);
    connect_c();
    dht.setup(DHT11_PIN);
    if (!bmp.begin())
  {
    Serial.println("Nie odnaleziono czujnika BMP085 / BMP180");
    //while (1) {}
  }

}

void loop(){
  delay(7000); //czekam 15 sekund na polaczenie
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
  
  if (dht.getStatusString() == "OK") {
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
    Serial.println(" metrow");

  sensorValue = analogRead(A0);
  sensor_volt = sensorValue/1024*5.0;
  RS_gas = (5.0-sensor_volt)/sensor_volt;
  ratio = RS_gas/R0;
  float x = 1538.46*ratio;
  float ppm = pow(x, -1.709);
  String ppm_str = (String)ppm;
  Serial.print("PPM: ");
  Serial.println(ppm_str);
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
  monitor.println("AT+CIPSTART=4, \"TCP\",\"184.106.153.149\",80");
  delay(10000);
  monitor.println("AT+CIPSEND=4,46");
  delay(3000);
  monitor.println(command);
  delay(2000);
  monitor.println("AT+CIPCLOSE");
}*/
void updateDHT(String a, String b){
  GET = "GET /dht.php?temperature="+ a +"&humidity=" +b+" HTTP/1.1";//48
        //GET+= Thingspeak_KEY;
        //GET += "&field2=";
  String command = GET;
//  delay(500);
 command += "\r\n";
// delay(500);
 command += "Host: 192.168.43.85";
// delay(500);
 command += "\r\n\r\n";
// delay(500);
//String command2 = "GET / HTTP/1.0\n\r";
  monitor.println("AT+CIPSTART=\"TCP\",\"192.168.43.85\",80"); //192.168.43.85
  delay(5000);
  monitor.print("AT+CIPSEND=");//bylo 73, 49 //nowe 72
  //delay(500);
  monitor.println(command.length());  
  
  delay(300);
  monitor.print(command);
/*  monitor.print(GET);
  delay(1000);
  monitor.print("\r\n");
  delay(1000);
  monitor.print("Host: 192.168.43.85");
  delay(1000);
  monitor.print("\r\n\r\n");
  delay(1000);*/
  
/** monitor.print("GET /dht.php?temperature=32&humidity=62 HTTP/1.1");//48
  monitor.println();
  monitor.println("Host: 192.168.43.85");//18
    monitor.println("Connection: close");//17
    monitor.println();
    monitor.println();* */
  /*  monitor.print("GET /dht.php?temperature=32&humidity=62 ");
 // monitor.print(a);
  monitor.print("HTTP/1.1 ");
  monitor.println("Host: 192.168.43.85");
  monitor.println("Connection: close");
  monitor.println();
  monitor.println();*/
  delay(3000);
  monitor.println("AT+CIPCLOSE");/// BYLO BEZ =4
}
void updateBMP(String a, String b){
  GET = "GET /bmp.php?temperature_bmp="+a+"&humidity_bmp="+b+" HTTP/1.1";//48
        //GET+= Thingspeak_KEY;
        //GET += "&field2=";
  String command = GET;
//  delay(500);
 command += "\r\n";
// delay(500);
 command += "Host: 192.168.43.85";
// delay(500);
 command += "\r\n\r\n";
// delay(500);
//String command2 = "GET / HTTP/1.0\n\r";
  monitor.println("AT+CIPSTART=\"TCP\",\"192.168.43.85\",80");
  delay(5000);
  monitor.print("AT+CIPSEND=");//bylo 73, 49 //nowe 72
  //delay(500);
  monitor.println(command.length());  
  
  delay(300);
  monitor.print(command);

  delay(3000);
  monitor.println("AT+CIPCLOSE");/// BYLO BEZ =4
}
void updateMQ(String a){
  GET = "GET /mq.php?ppm="+a+" HTTP/1.1";//48
        //GET+= Thingspeak_KEY;
        //GET += "&field2=";
  String command = GET;
//  delay(500);
 command += "\r\n";
// delay(500);
 command += "Host: 192.168.43.85";
// delay(500);
 command += "\r\n\r\n";
// delay(500);
//String command2 = "GET / HTTP/1.0\n\r";
  monitor.println("AT+CIPSTART=\"TCP\",\"192.168.43.85\",80");
  delay(5000);
  monitor.print("AT+CIPSEND=");//bylo 73, 49 //nowe 72
  //delay(500);
  monitor.println(command.length());  
  
  delay(300);
  monitor.print(command);

  delay(3000);
  monitor.println("AT+CIPCLOSE");/// BYLO BEZ =4
}
void connect_c(){
  monitor.println("AT+CWMODE=1");
  delay(2000);
        String cmd = "AT+CWJAP=\""+NazwaSieci+"\", \""+HasloSieci+"\"";
        delay(200);
  monitor.println(cmd);
  delay(2000);

}
