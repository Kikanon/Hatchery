
/*-----( Inport library )-----*/
#include <Servo.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//i2c pins
LiquidCrystal_I2C lcd(0x27, 16, 2); // 

#define LcdSda 21 //lcd display
#define LcdVcd 22

#define Senzor 27 //senzorji
#define BackupSenzor 15
#define DHTTYPE DHT22
DHT dht(Senzor, DHTTYPE);
int chk; //idk

#define Luc 33 //ogrevanje in indikacija
#define redLed 26
#define greLed 25

#define servo 5 //nagib

#define t1 18 //tipki
#define t2 19

//Vrednosti
int settemp = 30; //nastavljena temparatura
float temp = 0;//temparatura
float temp2 = 0;//alarm
float hum = 0;//vlaga
bool heat = 0;
int alarma = 0;
int alc = 0;

//Wifi
const char* ssid     = "Valilnica";
const char* password = "geslo123";
WiFiServer server(80); //Port
String header;

//čas
int currentMillis;
int oldMillis = 0;

//backup_senzor
OneWire oneWire(BackupSenzor); 
DallasTemperature sensors(&oneWire);
//servo
Servo myservo;
int pos;
bool nagib = 0;
//input
bool plkl;
bool mikl;
bool plin;
bool miin;
void setup() 
{
  //DEBUG
Serial.begin(115200);
  //LCD
lcd.begin();
lcd.backlight();//Power on the back light

//pini
pinMode(LcdSda, OUTPUT);
pinMode(LcdVcd, OUTPUT);
pinMode(Senzor, INPUT);
pinMode(BackupSenzor, INPUT);
pinMode(Luc, OUTPUT);
pinMode(redLed, OUTPUT);
pinMode(greLed, OUTPUT);
pinMode(servo, OUTPUT);
pinMode(t1, INPUT_PULLDOWN);
pinMode(t2, INPUT_PULLDOWN);

//Wifi
// Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(IP);
  server.begin();

//bsenzor
sensors.begin();

//senzor
dht.begin();

//servo
  myservo.attach(servo);
  myservo.write(80);
//input
plkl = 0;
mikl = 0;


 for (pos = 80; pos <= 100; pos += 1) { // sprememba nagiba 0->20
     // in steps of 1 degree
     myservo.write(pos);              // tell servo to go to position in variable 'pos'
     delay(100);                       // waits 15ms for the servo to reach the position
     }
}

void loop() {
  Serial.println("Temp: ");
  Serial.print(temp);
  Serial.println("Temp2: ");
  Serial.print(temp2);
setdata();
sensor1();
webpage();
output();
prikaz();
delay(200); 
}
void setdata(){
 //plus
plin = digitalRead(t2);
Serial.println("Senzor: ");
Serial.print(plin);
  if(plin == false){ //false, ce je kliknjen, kr pac Arduino
  if(plkl == 0){
    settemp= settemp + 1;
    plkl = 1;
    }
    Serial.println("plus kliknjen");
  }
else{
  plkl = 0;
  Serial.println("plus ne kliknjen");
  }
//minus
miin = digitalRead(t1);
  if(miin == false){ //false, ce je kliknjen, kr pac Arduino
  if(mikl == 0){
    settemp= settemp - 1;
    mikl = 1;
    }
  }
else{
  mikl = 0;
  }
  }

void sensor1(){
  //backup temp
  if(true){
   //Serial.print(" Requesting temperatures..."); 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 //Serial.println("DONE"); 
/********************************************************************/
// Serial.print("Backup temp. is: "); 
 //Serial.print(sensors.getTempCByIndex(0));
 temp2 = sensors.getTempCByIndex(0);
 temp2 = temp2 - 2; //zarad napake
  }
  //temp + hum
  hum = dht.readHumidity();
  temp= dht.readTemperature();
//  Serial.println("Temp: ");
//  Serial.print(temp);
//  Serial.println("Hum: ");
//  Serial.print(hum);
}
  
  
  
void webpage(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    //Serial.println("New Client.");          // print a message out in the serial port
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
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html><head><meta charset='utf-8'><title>Valilnica</title></head><body style='margin: 30px;'>");
            client.println("<h1>Temparatura: ");
            client.print(temp);
            client.print("°C</h1><br>");
            client.println("<h1>Vlažnost: ");
            client.print(hum);
            client.print(" %</h1></body></html>");
            
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
  
void output(){
  //                                                                     Gretje
  if(temp<=settemp-0.5){
    heat = 1;
    }
  if(temp>=settemp+0.5){
    heat = 0;
    }
  //                                                                     Varnost
  if(temp-temp2>=2.5||temp-temp2<=-2.5){   //
    alc++;
    if(alc==5){
    Serial.println("ALARM!!!!!!!");
    Serial.println("Temp: ");
    Serial.println(temp);
    Serial.println("Temp2: ");
    Serial.println(temp2);
    alarma = 1;
    }
    }else{
      alc=0;
      }
  //                                                                     NAGIB VSAKE POL URE
  currentMillis = millis();
  if(currentMillis == oldMillis + 1800000){ //mine pol ure     /   
    oldMillis = currentMillis;
    if(nagib == 1){ //
      nagib = 0;
     for (pos = 80; pos <= 100; pos += 1) { // sprememba nagiba 0->20
     // in steps of 1 degree
     myservo.write(pos);              // tell servo to go to position in variable 'pos'
     delay(500);                       // waits 15ms for the servo to reach the position
     }
    }
    else{
      nagib = 1;
     for (pos = 100; pos >= 80; pos -= 1) { // sprememba nagiba 20->0
     // in steps of 1 degree
     myservo.write(pos);              // tell servo to go to position in variable 'pos'
     delay(500);                       // waits 15ms for the servo to reach the position
     }
    }
    }
                                    //                                      REGULACIJA TEMPARATURE
   if(heat == 1){
   digitalWrite(Luc, HIGH);
 //  Serial.println("GRETJE ON");
   }
   if(heat == 0){
   digitalWrite(Luc, LOW);
  // Serial.println("GRETJE OFF");
   }
  }
  
void prikaz(){
  
  //Write your text:
lcd.setCursor(0,0); //Prva vrstica
lcd.print("Vlaga: "); //16 characters poer line
lcd.print(hum);
lcd.print("%     ");
lcd.setCursor(0,1);
lcd.print("Temp. :");//Druga vrstica
lcd.print(temp);
lcd.print(" C    ");
//                           GREEN LED
digitalWrite(greLed, HIGH);
delay(100);
digitalWrite(greLed, LOW);

//          ALARM
if(alarma == 1){
  while(true){
    Serial.println("ALARM!!!!!!!");
    delay(100);
    digitalWrite(redLed, HIGH);
    lcd.setCursor(0,0);
    lcd.print("   Napaka!!!");
    delay(100);
    digitalWrite(redLed, LOW);
    lcd.setCursor(0,1);
    lcd.print("   Napaka!!!");
    lcd.clear();
    }
  }

}
