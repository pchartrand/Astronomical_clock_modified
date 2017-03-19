/* Astronomical Night Lamp by Paulo Oliveira. www.paulorenato.com
 * Turns a relay driven through LED I/O #13 ON/OFF at sunset/sunrise according to the current
 * date andthe user's Lattitude and Longitude. Uses ad DS1307 RTC for time and battery backup.
 * Displays time, sunrise and sunset on a 4x 7-segment display. Full article at:
 * http://paulorenato.com/joomla/index.php?option=com_content&view=article&id=125&Itemid=4
 */

/******************* CONSTANTS AND VARIABLES *******************
****************************************************************/
#include <TimeLib.h>  
#include <TimeLord.h>
#include <Wire.h>       // Needed for I2C communication
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
DS1307RTC  rtc;                // Init the DS3231 using the hardware interface
//#include <Rtc_Pcf8563.h>
//Rtc_Pcf8563 rtc;
time_t  t;   

const int TIMEZONE = -5; //PST
const float LATITUDE = 45.50, LONGITUDE = -73.56; // set your position here

TimeLord myLord; // TimeLord Object, Global variable
byte sunTime[]  = {0, 0, 0, 30, 12, 16}; // 17 Oct 2013 // POURQUOI ?
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
  // rtc.begin();
    /* TimeLord Object Initialization */
  myLord.TimeZone(TIMEZONE * 60);
  myLord.Position(LATITUDE, LONGITUDE);
  myLord.DstRules(3,2,11,1,60); // DST Rules for USA
}

/*
void getDateAndTimeDS3231(){
  t = rtc.get();
  yr = t.year-2000;
  mt = t.mon;
  dy = t.date;
  hr = t.hour;
  mn = t.min;
}
*/

void getDateAndTimeDS1307RTC(){
  t = rtc.get();
  yr = year(t)-2000;
  mt = month(t);
  dy = day(t);
  hr = hour(t);
  mn = minute(t);
}

/*
void getDateAndTimePcf8563(){
  String date = rtc.formatDate(RTCC_DATE_ASIA);
  yr = date.substring(0,4).toInt();
  mt = date.substring(5,7).toInt();
  dy = date.substring(8,10).toInt();
  //Serial.println(date);

  String time = rtc.formatTime();
  hr = time.substring(0,2).toInt();
  mn = time.substring(3,5).toInt();
  //Serial.println(time);

}
*/
void ajouteHeure(int * const heure, int * const minut){
    *heure += 1;
    *minut = 60 - *minut;
}

void enleveHeure(int * const heure, int * const minut){
    *heure -= 1;
    *minut += 60;
}


/******************** MAIN LOOP STARTS HERE  *******************
****************************************************************/
void loop(){
  //getDateAndTimeDS3231();
  //getDateAndTimePcf8563();
  getDateAndTimeDS1307RTC();
  
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
    ajouteHeure(&heureReveil, &minuteReveil);
  }
  else if ((minuteReveil < 0) && (minuteReveil >= -60)){
    enleveHeure(&heureReveil, &minuteReveil);
  }
  
      Serial.print("Heure du reveil : ");
      Serial.print(heureReveil);
      Serial.print(":");
      Serial.println(minuteReveil);
      
  if ((minuteCoucher > 59) && (minuteCoucher < 120)){
     ajouteHeure(&heureCoucher, &minuteCoucher);
  }
  else if ((minuteCoucher < 0)&& (minuteCoucher >= -60)){
     enleveHeure(&heureCoucher, &minuteCoucher);
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

