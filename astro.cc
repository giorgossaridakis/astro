// astro calculator, moon phase and planetary day&time
#include <stdio.h>
#include <math.h>
#include <ctime>
#include <iostream>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

#define PI 3.1415926
#define ZENITH -.83

const char *daysofweek[]= { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }, *monthnames[]= { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" }, *planetdayrulers[]= { "Sun", "Moon", "Mars", "Mercury", "Jupiter", "Venus", "Saturn" }, *planetnames[]={ "Sun", "Venus", "Mercury", "Moon", "Saturn", "Jupiter", "Mars" };

float calculateSunriseSunset(int year,int month,int day,float lat, float lng,int localOffset, int daylightSavings, int flag);
float AssignSunriseSunsetTime(int year,int month,int day,float lat, float lng,int localOffset, int daylightSavings, int flag, double &hours, double &minutes);
int ReadLocationData(const char location_name[], float *lat, float *lng, int *localOffset)
;
void fixupperlowercharsforlocationanme(char *t);

using namespace std;
    
int main(int argc, char *argv[])
{
   float lat=0, lng=0;
   double sunrise_hours, sunrise_minutes, sunset_hours, sunset_minutes;
   int localOffset;
   float hourlength, daylength, nightlength, sunriseT, sunsetT;
   float dayhourlenth, nighthourlength, hournow;
   time_t t = std::time(0);   // get time now
   tm* now = std::localtime(&t);
   int i, daynightselector, planetary_hours[12][2]; // 0 day, 1 night
   int planet_selector, planetary_hour;
   char dayname[20], location_name[25];
    
   cout << "Astro Calculator v1.4, with location data" << endl;
   cout << "-----------------------------------------" << endl;
   if (argc<2 || ReadLocationData(argv[1], &lat, &lng, &localOffset)==-1) {
    if (argc<2) {
     cout << "location name:";
    cin  >> location_name; }
    else 
     strcpy(location_name, argv[1]);
    if (ReadLocationData(location_name, &lat, &lng, &localOffset)==-1) {
     fixupperlowercharsforlocationanme(location_name);
     cout << "new location:" << location_name << endl;
     cout << "input latitude, longtitude, timezone:";
     scanf("%f,%f,%d", &lat, &lng, &localOffset);
     ofstream datafile("astro.dat", ios::app);
     if (!datafile && !lat &&!lng)
      cout << "unable to append to datafile" << endl;
     if (lat && lng && datafile.is_open())
   datafile << location_name << " " << lat << " " << lng << " " << localOffset << endl; } }
   else {
    fixupperlowercharsforlocationanme(const_cast <char *> (argv[1]));
    cout << "location name:" << argv[1] << endl;
   ReadLocationData(argv[1], &lat, &lng, &localOffset); }
   cout << "latitude:" << lat << " longtitude:" << lng << " time zone:" << localOffset << endl;
   // start calculated output
   cout << daysofweek[now->tm_wday] << ", " << now->tm_mday  << ' ' << monthnames[now->tm_mon]  << ' ' << now->tm_year + 1900 << ", the time is: "  << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ". ";
   cout << "Julian day:" << (1461 * (now->tm_year + 1900 + 4800 + (now->tm_mon-14)/12))/4 + (367 * (now->tm_mon-2-12 * ((now->tm_mon - 14)/12)))/12 - (3 * ((now->tm_year + 1900 + 4900 + (now->tm_mon - 14)/12)/100))/4 + now->tm_mday - 32075 << endl;
   // calculate sunrise, sunset
   sunriseT=AssignSunriseSunsetTime(now->tm_year+1900, now->tm_mon+1, now->tm_mday, lat, lng, localOffset, now->tm_isdst, 0, sunrise_hours, sunrise_minutes);
   printf("sunrise:%.0f.%.0f, ", sunrise_hours, sunrise_minutes); //%02.0f
   sunsetT=AssignSunriseSunsetTime(now->tm_year+1900, now->tm_mon+1, now->tm_mday, lat, lng, localOffset, now->tm_isdst, 1, sunset_hours, sunset_minutes);
   printf("sunset:%.0f.%.0f\n", sunset_hours, sunset_minutes);
   daylength=sunsetT-sunriseT; nightlength=24-sunsetT+sunriseT;
   dayhourlenth=(daylength/12)*60;
   printf("day is %f hours long, length of hour is %f minutes\n", daylength, dayhourlenth);
   nighthourlength=(nightlength/12)*60;
   printf("night is %f hours long, length of hour is %f minutes\n",  nightlength, nighthourlength);
   sunriseT*=60; sunsetT*=60;
   hournow=(float) (now->tm_hour*3600+now->tm_min*60+now->tm_sec)/60;
   if (hournow<sunriseT && now->tm_wday)
    --now->tm_wday; // remove one day, day is calculated from sunrise to sunset
   if (hournow<sunriseT && !now->tm_wday)
    now->tm_wday=6;
   printf("ruler of this day is:%s", planetdayrulers[now->tm_wday]);
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
   printf(", ruler of this hour is:%s\n", planetnames[planetary_hours[planetary_hour-1][daynightselector]]);

 return 0;
}

float calculateSunriseSunset(int year,int month,int day,float lat, float lng,int localOffset, int daylightSavings, int flag)
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

float AssignSunriseSunsetTime(int year,int month,int day,float lat, float lng,int localOffset, int daylightSavings, int flag, double &hours, double &minutes)
{
  float localT;
  
  if (!flag) 
   localT = calculateSunriseSunset(year, month, day, lat, lng, localOffset, daylightSavings, flag);
  else
   localT=fmod(24 + calculateSunriseSunset(year, month, day, lat, lng, localOffset, daylightSavings, flag),24.0); 
  
   minutes = modf(localT, &hours)*60;
  
 return localT;
}

int ReadLocationData(const char location_name[], float *lat, float *lng, int *localOffset) // file format location latitude longtitude time zone
{
  int i,n;
  char tdata[20], tlocation[strlen(location_name)];
  ifstream datafile;
  
   datafile.open("./astro.dat");
   if (!datafile.is_open())
    datafile.open("/usr/bin/astro.dat");
   if (!datafile.is_open()) {
    printf("datafile not found\n");
   exit (-1); }
   
    strcpy(tlocation, location_name);
    for (i=0;i<strlen(location_name);i++)
     tlocation[i]=tolower(tlocation[i]);

    while (!datafile.eof()) {
     datafile >> tdata >> *lat >> *lng >> *localOffset;
     for (i=0;i<strlen(location_name);i++)
      tdata[i]=tolower(tdata[i]);
     if (!strcmp(tdata, tlocation))
      break; 
     else {
      *lat=0;
    *lng=0; } }
    
    if (!*lat && !*lng) 
     return -1;
   
  datafile.close();
  
 return 0;
}

void fixupperlowercharsforlocationanme(char *t)
{
  int i;
  
   t[0]=toupper(t[0]);
   for (i=1;i<strlen(t);i++)
    t[i]=tolower(t[i]);
   
}
