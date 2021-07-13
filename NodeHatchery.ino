
/*-----( Import library )-----*/
#include <Servo.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//i2c pins
LiquidCrystal_I2C lcd(0x27, 16, 2); // 

#define LcdSda A4 //lcd display
#define LcdVcd A5

#define Senzor 8 //senzorji
#define BackupSenzor 9
#define DHTTYPE DHT22
DHT dht(Senzor, DHTTYPE);
int chk; //idk

#define Luc 10 //ogrevanje in indikacija
#define redLed 11
#define greLed 12

#define servo 3 //nagib

#define t1 7 //tipki
#define t2 6

//Vrednosti
float settemp = 37; //nastavljena temparatura
float temp = 0;//temparatura
float temp2 = 0;//alarm
float hum = 0;//vlaga
bool heat = 0;
int alarma = 0;
int alc = 0;
int showSet = 0;


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
pinMode(t1, INPUT_PULLUP);
pinMode(t2, INPUT_PULLUP);



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
    showSet = 4;
    settemp= settemp + 0.5;
  }

//minus
miin = digitalRead(t1);
  if(miin == false){ //false, ce je kliknjen, kr pac Arduino
    showSet = 4;
    settemp= settemp - 0.5;
    mikl = 1;
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
    if(alc==1001){
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
   digitalWrite(Luc, LOW);
 //  Serial.println("GRETJE ON");
   }
   if(heat == 0){
   digitalWrite(Luc, HIGH);
  // Serial.println("GRETJE OFF");
   }
  }
  
void prikaz(){

  if(showSet <= 0){
    lcd.clear();
  //Write your text:
lcd.setCursor(0,0); //Prva vrstica
lcd.print("Vlaga: "); //16 characters poer line
lcd.print(hum);
lcd.print("%     ");
lcd.setCursor(0,1);
lcd.print("Temp. :");//Druga vrstica
lcd.print(temp);
lcd.print(" C    ");

  }
  else{
    lcd.clear();
    lcd.setCursor(0,0); //Prva vrstica
lcd.print("Set temp: "); //16 characters per line
lcd.setCursor(0,1);
lcd.print(settemp);
showSet--;
    }
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
