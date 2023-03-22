// astro calculator, moon phase and planetary day&time rulers
// written by Giorgos Saridakis
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include "MoonPhase.cpp"

using namespace std;

const double version=3.0;
const float PI=3.1415926;
const float ZENITH=-.83;
const int MAXSTRING=50;
const int MAXPATH=1024;

const char *daysofweek[]= { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }, *monthnames[]= { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" }, *planetdayrulers[]= { "Sun", "Moon", "Mars", "Mercury", "Jupiter", "Venus", "Saturn" }, *planetnames[]={ "Sun", "Venus", "Mercury", "Moon", "Saturn", "Jupiter", "Mars" };
const char *moonnames[] = { "Wolf", "Snow", "Worm", "Pink", "Flower", "Strawberry", "Buck", "Sturgeon", "Corn", "Hunter", "Beaver", "Cold" };

class NewFullMoon {
 public:
  float daysFull;
  float daysNew;
  float daysRotation;
  char moonName[MAXSTRING];
  NewFullMoon(MoonPhase &tmoon, int month);
};

// costructor
NewFullMoon::NewFullMoon( MoonPhase &tmoon, int month )
{
  int tphase=(int)(tmoon.phase * 8 + 0.5) % 8;
  float rotationdays =  ( ( (7*60) + 43 ) / ( 24 * 60 ) ) + 27;
  float cycledays = 29.5;
  daysRotation = rotationdays - tmoon.age + ( (cycledays - rotationdays) / 2); // estimate
  daysNew = cycledays - tmoon.age;
  // New EveningCrescent FirstQuarter WaxingGibbous Full WaningGibbous LastQuarter MorningCrescent
  daysFull = ( cycledays / 2 ) - tmoon.age;
  if ( tphase >= 4 )
   daysFull = ( cycledays - tmoon.age ) + ( cycledays / 2 );
  // full moon name for coming month
  if ( month == 11 )
   month = 0;
  else
   ++month;
  strcpy( moonName, moonnames[month] );
}

// function declarations
float calculateSunriseSunset(int year,int month,int day,float lat, float lng,double localOffset, int daylightSavings, int flag);
float AssignSunriseSunsetTime(int year,int month,int day,float lat, float lng,double localOffset, int daylightSavings, int flag, double &hours, double &minutes);
int ReadLocationData(const char city_name[], float *lat, float *lng, double *localOffset)
;
char* fixupperlowercharsforlocationanme(char *t);

// global variables
char datafilepath[MAXPATH];
char country[MAXSTRING], city[MAXSTRING], region[MAXSTRING];
    
int main(int argc, char *argv[])
{
   float lat, lng;
   double sunrise_hours, sunrise_minutes, sunset_hours, sunset_minutes;
   double localOffset;
   float hourlength, daylength, nightlength, sunriseT, sunsetT;
   float dayhourlenth, nighthourlength, hournow;
   time_t t = std::time(0);   // get time now
   tm* now = std::localtime(&t);
   MoonPhase moon;
   moon.calculate(t);
   NewFullMoon newfullmoon(moon, now->tm_mon);
   int i, daynightselector, planetary_hours[12][2]; // 0 day, 1 night
   int planet_selector, planetary_hour;
   char dayname[MAXSTRING], city_name[MAXSTRING], tstring[MAXSTRING];
   struct passwd *pw = getpwuid(getuid());
   sprintf(datafilepath, "%s/.astro", pw->pw_dir);
    
   printf("               ----------------------\n               -Astro Calculator %.1f"
          "-\n               ----------------------\n", version);
    
   // read or enquire location data
   if ( argc > 1 )
    strcpy(city_name, argv[1]);
   else {
    printf("city name:");
    cin >> city_name;
   }
   fixupperlowercharsforlocationanme(city_name);
   if ( (ReadLocationData(city_name, &lat, &lng, &localOffset)) == -1 ) {
    printf("country city region:");
    cin >> country >> city >> region;
    fflush(stdin);
    printf("latitude longtitude timezone (relative to GMT):");
    cin >> lat >> lng >> localOffset;
    ofstream datafile(datafilepath, ios_base::app);
    datafile << country << " " << city << " " << lat << " " << lng << " " << region << " " << localOffset << endl;
   }
   
   // local data and calculations
   printf("[%s, %.2d %s %d, the time is %.2d:%.2d:%.2d]\n", daysofweek[now->tm_wday], now->tm_mday, monthnames[now->tm_mon], now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);
   printf("country:%s city:%s region:%s\nlatitude:%.5f longitude:%.5f GMT%+-.1lf\n", country, city, region, lat, lng, localOffset); 
   // start calculated output
   printf("Julian day:%.2f", moon.jDate);
   sunriseT=AssignSunriseSunsetTime(now->tm_year+1900, now->tm_mon+1, now->tm_mday, lat, lng, localOffset, now->tm_isdst, 0, sunrise_hours, sunrise_minutes);
   printf(" sunrise %02.0f:%02.0f ", sunrise_hours, sunrise_minutes); //%02.0f
   sunsetT=AssignSunriseSunsetTime(now->tm_year+1900, now->tm_mon+1, now->tm_mday, lat, lng, localOffset, now->tm_isdst, 1, sunset_hours, sunset_minutes);
   printf("sunset %02.0f:%02.0f\n", sunset_hours, sunset_minutes);
   daylength=sunsetT-sunriseT; nightlength=24-sunsetT+sunriseT;
   dayhourlenth=(daylength/12)*60;
   strcpy(tstring, " ");
   if ( !strcmp(moon.phaseName, "Full") )
    sprintf( tstring, " [Full %s Moon] ", moonnames[now->tm_mon] );
   printf("Moon age:%.2f days phase:%s%s\n", moon.age, moon.phaseName, tstring);
   printf("zodiac transit:%s rotation in:%.2f days\n", moon.zodiacName, newfullmoon.daysRotation);
   printf("Full %s Moon in:%.2f days New Moon in:%.2f days\n", newfullmoon.moonName, newfullmoon.daysFull, newfullmoon.daysNew);
   printf("day:%f hours planetary hour:%f minutes\n", daylength, dayhourlenth);
   nighthourlength=(nightlength/12)*60;
   printf("night:%f hours planetary hour:%f minutes\n",  nightlength, nighthourlength);
   sunriseT*=60; sunsetT*=60;
   hournow=(float) (now->tm_hour*3600+now->tm_min*60+now->tm_sec)/60;
   if (hournow<sunriseT && now->tm_wday)
    --now->tm_wday; // remove one day, day is calculated from sunrise to sunset
   if (hournow<sunriseT && !now->tm_wday)
    now->tm_wday=6;
   printf("planetary ruler of the day:%s", planetdayrulers[now->tm_wday]);
   // find place of planet day ruler in hour sequence rulers
   planet_selector=now->tm_wday;
   strcpy(dayname, planetdayrulers[planet_selector]);
   while (strcmp(planetnames[planet_selector], dayname)) {
    ++planet_selector;
    if (planet_selector>6)
   planet_selector=0; }
   // arrange array with planet identities
   for (daynightselector=0;daynightselector<2;daynightselector++) {
    for (i=0;i<12;i++) {
     planetary_hours[i][daynightselector]=planet_selector;
     ++planet_selector;
     if (planet_selector>6)
   planet_selector=0; } }
   // calculate planetary hour
   daynightselector=0;
   if (hournow>sunsetT || hournow<sunriseT)
    daynightselector=1;
   planetary_hour=0;
   if (!daynightselector)
    while (sunriseT<hournow) {   
     sunriseT+=dayhourlenth;
    ++planetary_hour; }
   else {
    if (hournow<sunriseT)
     hournow+=1440;
     while (sunsetT<hournow) {
      sunsetT+=nighthourlength;
    ++planetary_hour; } }
    if (!planetary_hour)
     planetary_hour=1;
   printf(" | hour:%s\n", planetnames[planetary_hours[planetary_hour-1][daynightselector]]);

 return 0;
}

float calculateSunriseSunset(int year,int month,int day,float lat, float lng,double localOffset, int daylightSavings, int flag)
{
    /*
    localOffset will be <0 for western hemisphere and >0 for eastern hemisphere
    daylightSavings should be 1 if it is in effect during the summer otherwise it should be 0
    */
    //1. first calculate the day of the year
    float N1 = floor(275 * month / 9);
    float N2 = floor((month + 9) / 12);
    float N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
    float N = N1 - (N2 * N3) + day - 30;
    float t, H;
    //2. convert the longitude to hour value and calculate an approximate time
    float lngHour = lng / 15.0;
    if (!flag)
     t = N + ((6 - lngHour) / 24);   //if rising time is desired:
    else
     t = N + ((18 - lngHour) / 24);  //if setting time is desired:
    //3. calculate the Sun's mean anomaly   
    float M = (0.9856 * t) - 3.289;
    //4. calculate the Sun's true longitude
    float L = fmod(M + (1.916 * sin((PI/180)*M)) + (0.020 * sin(2 *(PI/180) * M)) + 282.634,360.0);
    //5a. calculate the Sun's right ascension      
    float RA = fmod(180/PI*atan(0.91764 * tan((PI/180)*L)),360.0);
    //5b. right ascension value needs to be in the same quadrant as L   
    float Lquadrant  = floor( L/90) * 90;
    float RAquadrant = floor(RA/90) * 90;
    RA = RA + (Lquadrant - RAquadrant);
    //5c. right ascension value needs to be converted into hours   
    RA = RA / 15;
    //6. calculate the Sun's declination
    float sinDec = 0.39782 * sin((PI/180)*L);
    float cosDec = cos(asin(sinDec));
    //7a. calculate the Sun's local hour angle
    float cosH = (sin((PI/180)*ZENITH) - (sinDec * sin((PI/180)*lat))) / (cosDec * cos((PI/180)*lat));
    if (cosH >  1 || cosH < -1)
     return -1;
//     the sun never rises on this location (on the specified date)
//     if (cosH < -1)
//     the sun never sets on this location (on the specified date)
    //7b. finish calculating H and convert into hours
    if (!flag)
     H = 360 - (180/PI)*acos(cosH);   //  if rising time is desired:
    else
     H = (180/PI)*acos(cosH); //   if setting time is desired:      
    H = H / 15;
    //8. calculate local mean time of rising/setting      
    float T = H + RA - (0.06571 * t) - 6.622;
    //9. adjust back to UTC
    float UT = fmod(T - lngHour,24.0);
    //10. convert UT value to local time zone of latitude/longitude
    return UT + localOffset + daylightSavings;
}

float AssignSunriseSunsetTime(int year,int month,int day,float lat, float lng,double localOffset, int daylightSavings, int flag, double &hours, double &minutes)
{
  float localT;
  
  if (!flag) 
   localT = calculateSunriseSunset(year, month, day, lat, lng, localOffset, daylightSavings, flag);
  else
   localT=fmod(24 + calculateSunriseSunset(year, month, day, lat, lng, localOffset, daylightSavings, flag),24.0); 
  
   minutes = modf(localT, &hours)*60;
  
 return localT;
}

// extract city data from datafile
int ReadLocationData(const char city_name[], float *lat, float *lng, double *localOffset) // file format location latitude longtitude time zone
{
  ifstream datafile;
  
   datafile.open(datafilepath, ios::in);
   if ( !datafile ) 
    return 0;
   
    while ( datafile ) {
     datafile >> country >> city >> *lat >> *lng >> region >> *localOffset;
     fixupperlowercharsforlocationanme(city);
     if ( !strcmp(city, city_name) )
      return 1;
    }
    datafile.close();
  
 return -1;
}

// first letter upper, rest lower
char* fixupperlowercharsforlocationanme(char *t)
{
  int i;
  
   t[0]=toupper(t[0]);
   for (i=1;i<strlen(t);i++)
    t[i]=tolower(t[i]);
   
 return t;
}
