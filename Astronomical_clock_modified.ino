/* Astronomical Night Lamp by Paulo Oliveira. www.paulorenato.com
 * Turns a relay driven through LED I/O #13 ON/OFF at sunset/sunrise according to the current
 * date andthe user's Lattitude and Longitude. Uses ad DS1307 RTC for time and battery backup.
 * Displays time, sunrise and sunset on a 4x 7-segment display. Full article at:
 * http://paulorenato.com/joomla/index.php?option=com_content&view=article&id=125&Itemid=4
 */

/******************* CONSTANTS AND VARIABLES *******************
****************************************************************/
#include <time.h>  
#include <TimeLord.h>
#include <Wire.h>       // Needed for I2C communication
#include <DS3231.h>  // a basic DS1307 library that returns time as a time_t
DS3231  rtc(SDA, SCL);                // Init the DS3231 using the hardware interface
Time  t;   

const int TIMEZONE = -5; //PST
const float LATITUDE = 45.50, LONGITUDE = -73.56; // set your position here

TimeLord myLord; // TimeLord Object, Global variable
byte sunTime[]  = {0, 0, 0, 30, 12, 16}; // 17 Oct 2013
int mSunrise, mSunset; //sunrise and sunset expressed as minute of day (0-1439)
// Need to adapt this according to the actual physical connections:

byte yr;byte mt; byte dy; byte hr; byte mn;

int HSR; //hour of sunrise
int MSR; //minute of sunrise
int HSS; //hour of sunset
int MSS; //minute of sunset
int SRmod = 15; //Décalage en minute par rapport à l'heure du lever de soleil (ouverture) : entree -60 et 60 (mn)
int SSmod = 20; //Décalage en minute par rapport à l'heure du coucher de soleil (fermeture)
int heureReveil; // heure d'ouverture de la porte
int minuteReveil; //minute d'ouverture de la porte
int heureCoucher;
int minuteCoucher;

/************************ ARDUINO SETUP  ***********************
****************************************************************/
void setup()  {
  Serial.begin(9600);
  rtc.begin();
}

/******************** MAIN LOOP STARTS HERE  *******************
****************************************************************/
void loop(){   
  t = rtc.getTime();
  yr = t.year-2000;
  mt = t.mon;
  dy = t.date;
  hr = t.hour;
  mn = t.min;
  /* TimeLord Object Initialization */
  myLord.TimeZone(TIMEZONE * 60);
  myLord.Position(LATITUDE, LONGITUDE);
  myLord.DstRules(3,2,11,1,60); // DST Rules for USA
      sunTime[3] = dy; // Uses the Time library to give Timelord the current date
      sunTime[4] = mt;
            sunTime[5] = yr;
      myLord.SunRise(sunTime); // Computes Sun Rise. Prints:
      mSunrise = sunTime[2] * 60 + sunTime[1];
          DisplaySunRise(sunTime);
      /* Sunset: */
      sunTime[3] = dy; // Uses the Time library to give Timelord the current date
      sunTime[4] = mt;
            sunTime[5] = yr;
      myLord.SunSet(sunTime); // Computes Sun Set. Prints:
      mSunset = sunTime[2] * 60 + sunTime[1];
          DisplaySunSet(sunTime);

//assignation de l'heure du réveil et du coucher
  heureReveil = HSR;
  minuteReveil = MSR + SRmod;
  heureCoucher = HSS;
  minuteCoucher = MSS + SSmod;
  
//Convertir décimales en hr
if ((minuteReveil > 59) && (minuteReveil < 120)){
    heureReveil += 1;
    minuteReveil = minuteReveil - 60;
  }
  else if ((minuteReveil < 0) && (minuteReveil >= -60)){
    heureReveil -= 1;
    minuteReveil = 60 - minuteReveil;
  }
  
      Serial.print("Heure du reveil : ");
      Serial.print(heureReveil);
      Serial.print(":");
      Serial.println(minuteReveil);
      
  if ((minuteCoucher > 59) && (minuteCoucher < 120)){
    heureCoucher += 1;
    minuteCoucher = minuteCoucher - 60;
  }
  else if ((minuteCoucher < 0)&& (minuteCoucher >= -60)){
    heureCoucher -= 1;
    minuteCoucher = 60 - minuteCoucher;
  }

      Serial.print("heure du Coucher : ");
      Serial.print(heureCoucher);
      Serial.print(":");
      Serial.println(minuteCoucher);

//Exécution du programme
if (((hr == heureReveil)  && (mn >= minuteReveil))||((hr > heureReveil) && (hr < heureCoucher))||((hr == heureCoucher)  && (mn < minuteCoucher))){
    //insérer programme d'ouverture
    Serial.println("Mode : jour");
  }
  else if (((hr == heureCoucher)  && (mn >= heureCoucher))||(hr > heureCoucher)||(hr < heureReveil)||((hr == heureReveil)  && (mn < minuteReveil))){
    //insérer programme de fermeture
    Serial.println("Mode : nuit");
  }


Serial.println("");
delay(1000);

}
       
       
void DisplaySunRise(uint8_t * when)
{      HSR = when[2];
      MSR = when[1];
      Serial.print("Lever du soleil : ");
      Serial.print(HSR);
      Serial.print(":");
      Serial.println(MSR);
}
void DisplaySunSet(uint8_t * when)
{     HSS = when[2];
      MSS = when[1];
      Serial.print("Coucher du soleil : ");
      Serial.print(HSS);
      Serial.print(":");
      Serial.println(MSS);
}

