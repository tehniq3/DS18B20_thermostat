// simple thermostat by Nicu FLORICA (niq_ro)

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#define releu 4
#define minus 2
#define plus 3

#define adresa 100

byte prag = 0;
int temperatura = 0;
byte pragminim = 30;
byte pragmaxim = 90;
byte histerezis = 1;

unsigned long timpcitire = 0;
unsigned long pauzacitire = 10000;  // 10 secunde

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("termostat pompa centrala");

// initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Termostat pompa");
  lcd.setCursor(0,1);
  lcd.print("vers.1.0");

pinMode(minus, INPUT);
pinMode(plus, INPUT);
pinMode(releu, OUTPUT);
digitalWrite(minus, HIGH);
digitalWrite(plus, HIGH);
digitalWrite(releu, LOW);

// EEPROM.write(adresa, 30); // just first date
prag = EEPROM.read(adresa);
if (prag < 30)
{
  prag = pragminim;
  EEPROM.write(adresa, prag); 
}
if (prag > 90) 
{
  prag = pragmaxim;
  EEPROM.write(adresa, prag); 
}

  // Start up the library
  sensors.begin();

Serial.print("Requesting temperatures...");
sensors.requestTemperatures(); // Send the command to get temperatures
Serial.println("DONE");
Serial.print("Temperature for the device 1 (index 0) is: ");
temperatura = sensors.getTempCByIndex(0);
Serial.println(temperatura);  
timpcitire = millis();  

lcd.clear();
}

void loop(void)
{ 

if (timpcitire - millis() > pauzacitire)
{
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  Serial.print("Temperature for the device 1 (index 0) is: ");
temperatura = sensors.getTempCByIndex(0);
Serial.println(temperatura);  
timpcitire = millis();
}


lcd.setCursor(0,0);
lcd.print("cerut= ");
lcd.print(temperatura);
lcd.write(0b11011111);
lcd.print("C");
lcd.setCursor(0,1);
lcd.print("doritt= ");
lcd.print(prag);
lcd.write(0b11011111);
lcd.print("C");

if (digitalRead(minus) == LOW)
{
  prag = prag -1;
  if (prag < 30) prag = pragminim;
  delay(500);
  EEPROM.write(adresa, prag); 
}

if (digitalRead(plus) == LOW)
{
  prag = prag +1;
  if (prag > 90) prag = pragmaxim;
  delay(500);
  EEPROM.write(adresa, prag); 
}


if (temperatura > prag)
{
  digitalWrite(releu, HIGH);
lcd.setCursor(15,0);
lcd.print("*");
}

if (temperatura < prag - histerezis)
{
  digitalWrite(releu, LOW);
lcd.setCursor(15,0);
lcd.print(" ");
}

delay(100);
}  // end program



