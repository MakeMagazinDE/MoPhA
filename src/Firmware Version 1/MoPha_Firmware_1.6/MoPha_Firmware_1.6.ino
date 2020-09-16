/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>

HardwareSerial MySerial1(1);

HTTPClient http;

#define R1 23 //Abendsichel
#define R2 22 //zunehmende Sichel
#define R3 32 //Erstes Viertel
#define R4 33 //zunehmender Halbmond
#define R5 25 //abnehmender Vollmond
#define R6 26 //Letztes Viertel
#define R7 27 //abnehmende Sichel
#define R8 14 //Morgensichel
#define R9 19 //Vollmond
#define R10 18 //rote LED fuer MoFi

#define LED 2 //WiFi Kontroll-LED
  
const char* ssid     = "YOUR SSID";
const char* password = "YOUR PASSWORD";

//const char* host = "http://www.astronomie.at/Scripts/kalendarium.asp";

bool MoFi;
float MAWert;


void setup()
{
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  pinMode(R5, OUTPUT);
  pinMode(R6, OUTPUT);
  pinMode(R7, OUTPUT);
  pinMode(R8, OUTPUT);
  pinMode(R9, OUTPUT);
  pinMode(R10, OUTPUT);
  pinMode(LED, OUTPUT);
  
  digitalWrite (R1, HIGH);
  digitalWrite (R2, HIGH);
  digitalWrite (R3, HIGH);
  digitalWrite (R4, HIGH);
  digitalWrite (R5, HIGH);
  digitalWrite (R6, HIGH);
  digitalWrite (R7, HIGH);
  digitalWrite (R8, HIGH);
  digitalWrite (R9, HIGH);
  digitalWrite (R10, HIGH);
  
  Serial.begin(115200);
  Serial.println();

  MySerial1.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.mode(WIFI_STA);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  digitalWrite (LED, HIGH);


}


void loop()
{
   
 if ((WiFi.status() == WL_CONNECTED)) {
    
     http.begin("https://www.astronomie.de/"); //Specify the URL
     int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String line = http.getString();
        Serial.println(httpCode);
        //Serial.println(line);

        if(line.indexOf("Mondfinsternis") >= 0){
        MoFi = true;
         }
        else {
        MoFi = false;
        Serial.println(MoFi);
        }

        http.end();

        delay (5000);

        http.begin("https://www.astronomie.de/der-himmel-aktuell/?no_cache=1");

         int httpCode = http.GET();                                        //Make the request
 
        if (httpCode > 0) { //Check for the returning code
 
        String line = http.getString();
        Serial.println(httpCode);
       
        if (line.indexOf("Mondalter") >= 0) { // Searchstring exists?
        Serial.println();
        //Serial.println(line);
        int vonPos = line.indexOf("Mondalter "); 
        int bisPos = line.indexOf(" Tage");
        Serial.print("Das aktuelle Mondalter ist ");
        Serial.print(line.substring(vonPos+10, bisPos));
        Serial.println (" Tage");
        //Serial.println(" Grad C");
        // Umandlung in float
        String MA=line.substring(vonPos+10, bisPos);
        char char1[8];
        MA.toCharArray(char1, MA.length()+1);
        MAWert=atof(char1);
        Serial.println(MAWert);

        MySerial1.print ("Mondalter:");
        MySerial1.print (MAWert);
        MySerial1.println (" Tage");
        }

        if (MAWert >= 0.50 && MAWert <= 3.00){
        digitalWrite (R1, LOW);
        MySerial1.println ("Junge Mondsichel am Abendhimmel");
      }
      else if (MAWert > 3.00 && MAWert <= 6.00) {
        digitalWrite (R1, HIGH);
        digitalWrite (R2, LOW);
        MySerial1.println ("Zunehmende Mondsichel");
      }
      else if (MAWert > 6.00 && MAWert <= 9.50){
        digitalWrite (R2, HIGH);
        digitalWrite (R3, LOW);
        MySerial1.println ("Erstes Viertel");
      }
      else if (MAWert > 9.50 && MAWert <= 13.50){
        digitalWrite (R3, HIGH);
        digitalWrite (R4, LOW);
        MySerial1.println ("Zunehmender Halbmond");
      }
      else if (MAWert > 13.50 && MAWert <= 16.00){
        if (MoFi == true){
          digitalWrite (R4, HIGH);
          digitalWrite (R10, LOW);
          
          Serial.println ("Mondfinsternis steht bevor!");
        
          MySerial1.println ("Mondfinsternis steht bevor!");
        }
        else {
        digitalWrite (R4, HIGH);
        digitalWrite (R9, LOW);
        digitalWrite (R10, HIGH);
        MySerial1.println ("Vollmond");
        }
      }
      else if (MAWert > 16.00 && MAWert <= 20.00){
        digitalWrite (R9, HIGH);
        digitalWrite (R10, HIGH);
        digitalWrite (R5, LOW);
        MySerial1.println ("abnehmender Vollmond");
      }
      else if (MAWert > 20.00 && MAWert <= 23.50){
        digitalWrite (R5, HIGH);
        digitalWrite (R6, LOW);
        MySerial1.println ("Letztes Viertel");
      }
      else if (MAWert > 23.50 && MAWert <= 26.50){
        digitalWrite (R6, HIGH);
        digitalWrite (R7, LOW);
        MySerial1.println ("Abnehmende Mondsichel");
      }
      else if (MAWert > 26.50 && MAWert <=29.00) {
        digitalWrite (R7, HIGH);
        digitalWrite (R8, LOW);
        MySerial1.println ("Alte Mondsichel am Morgenhimmel");
      }
      else {
        digitalWrite (R1, HIGH);
        digitalWrite (R2, HIGH);
        digitalWrite (R3, HIGH);
        digitalWrite (R4, HIGH);
        digitalWrite (R5, HIGH);
        digitalWrite (R6, HIGH);
        digitalWrite (R7, HIGH);
        digitalWrite (R8, HIGH);
        digitalWrite (R9, HIGH);
        digitalWrite (R10, HIGH);
        
        MySerial1.println ("Neumond");
      }

         
    }
    else {
      Serial.println("Error on HTTP request");
    }
    }
    http.end(); //Free the resources
  }
 }
