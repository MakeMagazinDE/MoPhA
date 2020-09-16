/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Based on the NTP Client library example
*********/

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HardwareSerial.h>
#include "mofistring2050.h"

#define PROGMEM   ICACHE_RODATA_ATTR // Initialize PROGMEM on ESP32

HardwareSerial MySerial1(1);

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

char buffer[30]; // The length of the buffer must match the longest string!
String bufferstring;

// Replace with your network credentials
const char* ssid     = "YOUR SSID";
const char* password = "YOUR PASSWORD";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String decadestring, DString, MString, YString;
int decade, D, M, Y;

bool MoFi;
double MAWert;

void setup() {

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

  //Initialize Software UART for HC-05
  MySerial1.begin(9600, SERIAL_8N1, 16, 17);

  // Start WiFi Connection
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


  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
}


void loop() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  //formattedDate = "2024-03-26"; //nur Test fuer MoFi Detektion
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  decadestring = dayStamp.substring(2, 3);
  YString = dayStamp.substring(0, 4);
  MString = dayStamp.substring(5, 7);
  DString = dayStamp.substring(8, 10);
  Serial.println(decade);
  Serial.println(YString);
  Serial.println(MString);
  Serial.println(DString);
  decade = decadestring.toInt();
  D = DString.toInt();
  M = MString.toInt();
  Y = YString.toInt();
  Serial.println(Y);
  Serial.println(M);
  Serial.println(D);

  // Calculate lunar age
  MAWert = getPhase(Y, M, D);

  Serial.println(MAWert);

  // Send lunar age via UART to HC-05 Bluetooth device
  MySerial1.print ("Mondalter:");
  MySerial1.print (MAWert);
  MySerial1.println (" Tage");

  // Switching of LED modules with regard to lunar age
  if (MAWert >= 0.50 && MAWert <= 3.00) {
    digitalWrite (R1, LOW);
    MySerial1.println ("Junge Mondsichel am Abendhimmel");
  }
  else if (MAWert > 3.00 && MAWert <= 6.00) {
    digitalWrite (R1, HIGH);
    digitalWrite (R2, LOW);
    MySerial1.println ("Zunehmende Mondsichel");
  }
  else if (MAWert > 6.00 && MAWert <= 9.50) {
    digitalWrite (R2, HIGH);
    digitalWrite (R3, LOW);
    MySerial1.println ("Erstes Viertel");
  }
  else if (MAWert > 9.50 && MAWert <= 13.50) {
    digitalWrite (R3, HIGH);
    digitalWrite (R4, LOW);
    MySerial1.println ("Zunehmender Halbmond");
  }
  else if (MAWert > 13.50 && MAWert <= 16.00) {
    if (MoFi == true) {
      digitalWrite (R4, HIGH);
      digitalWrite (R9, HIGH);
      digitalWrite (R10, LOW);

      //Serial.println ("Mondfinsternis steht bevor!");

      //MySerial1.println ("Mondfinsternis steht bevor!");
    }
    else {
      digitalWrite (R4, HIGH);
      digitalWrite (R9, LOW);
      digitalWrite (R10, HIGH);
      MySerial1.println ("Vollmond");
    }
  }
  else if (MAWert > 16.00 && MAWert <= 20.00) {
    digitalWrite (R9, HIGH);
    digitalWrite (R10, HIGH);
    digitalWrite (R5, LOW);
    MySerial1.println ("abnehmender Vollmond");
  }
  else if (MAWert > 20.00 && MAWert <= 23.50) {
    digitalWrite (R5, HIGH);
    digitalWrite (R6, LOW);
    MySerial1.println ("Letztes Viertel");
  }
  else if (MAWert > 23.50 && MAWert <= 26.50) {
    digitalWrite (R6, HIGH);
    digitalWrite (R7, LOW);
    MySerial1.println ("Abnehmende Mondsichel");
  }
  else if (MAWert > 26.50 && MAWert <= 29.00) {
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


  /*  Determination if a lunar eclipse is imminent. Therefore the actual date will be compared to
      several lists of eclipses up to the year 2050. The lists are separated with regard to the upcoming
      decades.  
   
      Special functions for reading data are necessary. "strcpy_P" will copy the string from the 
      flash-/program memory in the SRAM / the buffer. One has to ensure that the receiving string
      is long enough for the copied string.
  */

  // Iteration over the array, 18 times since 18 strings are in the array
  if (decade = 2) {
    for (int i = 0; i < 18; i++) {
      strcpy_P(buffer, (char*)pgm_read_dword(&(mofi_table20[i]))); // Casts und Dereferenzierung des Speichers
      bufferstring = buffer;
      if (bufferstring.substring (0, 4) == YString && bufferstring.substring (5, 7) == MString && bufferstring.substring (8, 10) == DString) {

        String gtype, etype, valuestring;
        float value;

        gtype = bufferstring.substring (11, 12); // General type of eclipse
        etype = bufferstring.substring (13, 14); // type of eclipse visible from Europe
        valuestring = bufferstring.substring(15, 18); // Magnitude of eclipse

        char char1[8];
        valuestring.toCharArray(char1, valuestring.length() + 2);
        value = atof(char1);

        //for BT transmission
        MySerial1.println ("Mondfinsternis steht bevor!");
        MySerial1.print ("Art der Finsternis:");
        MySerial1.println (gtype);
        MySerial1.print ("In Europa sichtbar als:");
        MySerial1.println (etype);
        MySerial1.print ("Bedeckungsgrad:");
        MySerial1.println (value);

        //for debugging
        Serial.println ("Mondfinsternis steht bevor!");
        Serial.print ("Art der Finsternis:");
        Serial.println (gtype);
        Serial.print ("In Europa sichtbar als:");
        Serial.println (etype);
        Serial.print ("Bedeckungsgrad:");
        Serial.println (value);

        MoFi = true;
        digitalWrite (R9, HIGH);
        digitalWrite (R10, LOW);

        break;

        
        
      } 
      Serial.println(buffer); // Gib den gelesenen Wert aus
      Serial.println(MoFi);
      delay(1000); // Warte eine halbe Sekunde

      MoFi = false; 
      
    }
  }

  else if (decade = 3) {
    for (int i = 0; i < 18; i++) {
      strcpy_P(buffer, (char*)pgm_read_dword(&(mofi_table30[i]))); // Casts und Dereferenzierung des Speichers
      bufferstring = buffer;
      if (bufferstring.substring (0, 4) == YString && bufferstring.substring (5, 7) == MString && bufferstring.substring (8, 10) == DString) {
        
        String gtype, etype, valuestring;
        float value;

        gtype = bufferstring.substring (11, 12); // General type of eclipse
        etype = bufferstring.substring (13, 14); // type of eclipse visible from Europe
        valuestring = bufferstring.substring(15, 18); // Magnitude of eclipse

        // Transformation of magnitude string into float
        char char1[8];
        valuestring.toCharArray(char1, valuestring.length() + 2);
        value = atof(char1);

        //for BT transmission
        MySerial1.println ("Mondfinsternis steht bevor!");
        MySerial1.print ("Art der Finsternis:");
        MySerial1.println (gtype);
        MySerial1.print ("In Europa sichtbar als:");
        MySerial1.println (etype);
        MySerial1.print ("Bedeckungsgrad:");
        MySerial1.println (value);

        //for debugging
        Serial.println ("Mondfinsternis steht bevor!");
        Serial.print ("Art der Finsternis:");
        Serial.println (gtype);
        Serial.print ("In Europa sichtbar als:");
        Serial.println (etype);
        Serial.print ("Bedeckungsgrad:");
        Serial.println (value);

        MoFi = true;
        digitalWrite (R9, HIGH);
        digitalWrite (R10, LOW);

        break;
      }  
      
      Serial.println(buffer); // Gib den gelesenen Wert aus
      Serial.println(MoFi);
      delay(1000); // Warte eine halbe Sekunde

      MoFi = false; 
    }
  }

  else if (decade = 4) {
    for (int i = 0; i < 14; i++) {
      strcpy_P(buffer, (char*)pgm_read_dword(&(mofi_table40[i]))); // Casts und Dereferenzierung des Speichers
      bufferstring = buffer;
      if (bufferstring.substring (0, 4) == YString && bufferstring.substring (5, 7) == MString && bufferstring.substring (8, 10) == DString) {
        String gtype, etype, valuestring;
        float value;

        gtype = bufferstring.substring (11, 12);
        etype = bufferstring.substring (13, 14);
        valuestring = bufferstring.substring(15, 18);

        char char1[8];
        valuestring.toCharArray(char1, valuestring.length() + 2);
        value = atof(char1);

        //Fuer BT Uebertragung
        MySerial1.println ("Mondfinsternis steht bevor!");
        MySerial1.print ("Art der Finsternis:");
        MySerial1.println (gtype);
        MySerial1.print ("In Europa sichtbar als:");
        MySerial1.println (etype);
        MySerial1.print ("Bedeckungsgrad:");
        MySerial1.println (value);

        //Zum Debuggen
        Serial.println ("Mondfinsternis steht bevor!");
        Serial.print ("Art der Finsternis:");
        Serial.println (gtype);
        Serial.print ("In Europa sichtbar als:");
        Serial.println (etype);
        Serial.print ("Bedeckungsgrad:");
        Serial.println (value);

        MoFi = true;
        digitalWrite (R9, HIGH);
        digitalWrite (R10, LOW);

        break;
      }  
      
      Serial.println(buffer); // Gib den gelesenen Wert aus
      Serial.println(MoFi);
      delay(1000); // Warte eine halbe Sekunde

      MoFi = false; 
    }
  }

  else if (decade = 5) {
    for (int i = 0; i < 2; i++) {
      strcpy_P(buffer, (char*)pgm_read_dword(&(mofi_table50[i]))); // Casts und Dereferenzierung des Speichers
      bufferstring = buffer;
      if (bufferstring.substring (0, 4) == YString && bufferstring.substring (5, 7) == MString && bufferstring.substring (8, 10) == DString) {
       String gtype, etype, valuestring;
        float value;

        gtype = bufferstring.substring (11, 12);
        etype = bufferstring.substring (13, 14);
        valuestring = bufferstring.substring(15, 18);

        char char1[8];
        valuestring.toCharArray(char1, valuestring.length() + 2);
        value = atof(char1);

        //Fuer BT Uebertragung
        MySerial1.println ("Mondfinsternis steht bevor!");
        MySerial1.print ("Art der Finsternis:");
        MySerial1.println (gtype);
        MySerial1.print ("In Europa sichtbar als:");
        MySerial1.println (etype);
        MySerial1.print ("Bedeckungsgrad:");
        MySerial1.println (value);

        //Zum Debuggen
        Serial.println ("Mondfinsternis steht bevor!");
        Serial.print ("Art der Finsternis:");
        Serial.println (gtype);
        Serial.print ("In Europa sichtbar als:");
        Serial.println (etype);
        Serial.print ("Bedeckungsgrad:");
        Serial.println (value);

        MoFi = true;
        digitalWrite (R9, HIGH);
        digitalWrite (R10, LOW);

        break;
      }  
      
      Serial.println(buffer); // Gib den gelesenen Wert aus
      Serial.println(MoFi);
      delay(1000); // Warte eine halbe Sekunde

      MoFi = false; 
    }
  }
  else {
    Serial.println("Error");
  }





  // Extract time - not needed
  /*timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    Serial.print("HOUR: ");
    Serial.println(timeStamp);*/
  delay(1000);
}

//Calculation of the lunar age by means of using the Julian date

double getPhase(int Y, int M, int D) {  // calculate the current phase of the moon
  double AG, IP;                      // based on the current date
  byte phase;                         // algorithm adapted from Stephen R. Schmitt's
  // Lunar Phase Computation program, originally
  long YY, MM, K1, K2, K3, JD;        // written in the Zeno programming language
  // http://home.att.net/~srschmitt/lunarphasecalc.html
  // calculate julian date
  YY = Y - floor((12 - M) / 10);
  MM = M + 9;
  if (MM >= 12)
    MM = MM - 12;

  K1 = floor(365.25 * (YY + 4712));
  K2 = floor(30.6 * MM + 0.5);
  K3 = floor(floor((YY / 100) + 49) * 0.75) - 38;

  JD = K1 + K2 + D + 59;
  if (JD > 2299160)
    JD = JD - K3;

  IP = normalize((JD - 2451550.1) / 29.530588853);
  AG = IP * 29.53;
  // Serial.print(AG);
  // Serial.println();
  // Serial.print(JD);
  // Serial.println();
  return AG;
}

double normalize(double v) {           // normalize moon calculation between 0-1
  v = v - floor(v);
  if (v < 0)
    v = v + 1;
  return v;
}

