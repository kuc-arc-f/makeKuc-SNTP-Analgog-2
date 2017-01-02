
/* SoftwareSerial +OLED + Analog Sensor(LM60BIZ )
 , NTP receive sample. (Atmega328P )
*/

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define
SoftwareSerial mySerial(5, 6); /* RX:D5, TX:D6 */
const int mVoutPin = 0;
uint32_t mTimerTmp;
int mTemp=0;
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
  
//
void setup() {
  Serial.begin( 9600 );
  mySerial.begin( 9600 );
  Serial.println("#Start-setup-SS");
  pinMode(mVoutPin, INPUT);
    //
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(2000);
  display.clearDisplay(); 
}
//
long convert_Map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// reading LM60BIZ
int getTempNum(){
  int iRet=0;
  float fSen  = 0;
  unsigned long reading  = 0;   
  for (int i=0; i<10; i++) {
    int  iTmp = analogRead(mVoutPin);
//Serial.print("get.iTmp=");
//Serial.println(iTmp);
    reading  += iTmp; 
    //delay(100);
  }
  int SValue= reading / 10;
  int voltage=convert_Map(SValue, 0, 1000, 0,3300);  // V
//Serial.print("SValue=");
//Serial.print(SValue);
//Serial.print(" , voltage=");
//Serial.println(voltage);
  int iTemp = (voltage - 424) / 6.25; //電圧値を温度に変換, offset=425
  iRet= iTemp;
  
  return iRet;  
}
//
// void display_OLED(String sTime ){
void display_OLED(String sTime , String sTemp){
  String sBuff ="Temp:"+ sTemp+ "C";
  display.setTextSize(2);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println( sTime );
  display.println(sBuff);
  display.display();
  delay(500);  
  display.clearDisplay();
}
//
boolean Is_validHead(String sHd){
  boolean ret=false;
  if(sHd=="d1"){
    ret= true;
  }else if(sHd=="d2"){
    ret= true;
  }
  return ret;
}
String mBuff="";
//
void loop() {
    while( mySerial.available() ){
      char c= mySerial.read();
      mBuff.concat(c );
      if(  mBuff.length() >= 10 ){
        String sHead= mBuff.substring(0,2);
        boolean bChkHd= Is_validHead( sHead );
        if(bChkHd== true){
//Serial.println( "Hd="+ sHead );
//Serial.println( "mBuff="+ mBuff );
          String sTmp= mBuff.substring(2,10);
          display_OLED(sTmp ,String(mTemp));
        }        
        mBuff="";
      }
    } //end_while
    if (millis() > mTimerTmp) {
       mTimerTmp = millis()+ 5000;
       mTemp= getTempNum();
       int iD1=int(mTemp );
       char cD1[10+1];
     sprintf(cD1 , "d1%08d", iD1);       
     mySerial.print( cD1 );
//Serial.print("cD1=");
//Serial.println(cD1  );       
    }

}








