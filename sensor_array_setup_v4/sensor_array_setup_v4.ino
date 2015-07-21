
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"

#define LOG_INTERVAL  1000
#define SYNC_INTERVAL 1000
uint32_t syncTime = 0;
uint32_t m = 0;

#define ECHO_TO_SERIAL   1

#define redLEDpin 2

RTC_DS1307 RTC;

const int chipSelect = 10;

//---setup sensor pin array
int sensor [] = {
  A1, A2, 3, 4, 5, 6, 7, 8
};
const byte sensorCount = 8;

//---setup product names array
char* productName [] = {
  "soap", "salve", "lotion 1", "lotion 2", "spot healer 1",
  "spot heal 2", "nail stick 1", "nail stick 2"
};


//---setup 2D data smoothing array
int totalSensorData [sensorCount][10];

/*
[0] - Total Data
[1] - prev sensor readings
[2] - sensor differences
*/
int allDataArray[sensorCount][3];

//---setup debounceTime array
unsigned long arrayLastDebounceTime [sensorCount];


int debounceDelay = 1000;
byte arrayIndex;

File logfile;
void setup() {

  Serial.begin(9600);

  //---- Logging setup ----//
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);

  initializeSD();
  connectToRTC();

  logfile.println("millis,stamp,datetime,product name, status");
#if ECHO_TO_SERIAL
  Serial.println("millis,stamp,datetime,product name, status");
#endif //ECHO_TO_SERIAL

  for (byte h = 0; h < sensorCount; h++) {
    pinMode(sensor[h], INPUT);
    arrayLastDebounceTime[arrayIndex] = 0;
    for (byte i = 0; i < 3; i++) {
      allDataArray[sensorCount] [3] = 0;
    }
  }

}

void loop() {
  m = millis();
  readSensors();
}

void readSensors() {

  //vars for raw sensor data
  int currentSensorRead;
  for (byte j = 0; j < 10; j  ++) {
    for (byte h = 0; h < sensorCount; h ++) {

      if (h < 2) {
        currentSensorRead = analogRead(sensor[h]);
      }
      else {
        currentSensorRead = digitalRead(sensor[h]);
      }

      arrayIndex = h;
      smoothData(currentSensorRead, j);

    }

  }

}

void smoothData(int sensorData, byte j) {

  int average = 0;

  //for reference
  // int totalSensorData [sensorCount][10];
  //data smoothing

  allDataArray[arrayIndex] [0] = allDataArray[arrayIndex] [0] - totalSensorData[arrayIndex][j];
  totalSensorData[arrayIndex][j] = sensorData;
  allDataArray[arrayIndex] [0] = allDataArray[arrayIndex] [0] + totalSensorData[arrayIndex][j];

  average = allDataArray[arrayIndex] [0] / 10;

  //  Serial.print(arrayIndex);
  //  Serial.print(' ');
  //  Serial.println(average);


  delay(1);

  //debounce timer
  debounceAndCheck(average);

}

void debounceAndCheck(int avg) {

  int threshold;
  int hallThresh = 0;
  int irThresh = 7;
  int sensorDifference;
  long debounceDifference;

  //check if the reading is coming from a hall (digital) or IR (analog) sensor

  if (arrayIndex > 1) {
    threshold = hallThresh;
  }

  //check difference to see if it's meaningful and if it's a pick or place
  sensorDifference = (avg - allDataArray[arrayIndex] [1]);

  //check if the IR sensor is a place and then adjust IR threshold for a place
  if (arrayIndex < 2 && sensorDifference > 0) {
    threshold = irThresh;
  }
  else if (arrayIndex < 2 && sensorDifference < 0) {
    threshold = irThresh + 3;
  }


  if (abs(sensorDifference) > threshold) {

    //store difference in array for each sensor
    allDataArray[arrayIndex][2] = sensorDifference;

    //see if reading was greater than debounce delay
    debounceDifference = millis() - arrayLastDebounceTime[arrayIndex];
    if ( debounceDifference > debounceDelay) {

      //determine if it's a pick or place
      if (allDataArray[arrayIndex][2] > 0) {
        logData(productName[arrayIndex], "pick");
      }

      else if (allDataArray[arrayIndex][2] < 0) {
        logData(productName[arrayIndex], "place");
      }
      arrayLastDebounceTime[arrayIndex] = millis();
    }
  }

  allDataArray[arrayIndex] [1] = avg;

}

void logData(char *str, char *stat) {

  DateTime now;


  logfile.print(m);           // milliseconds since start
  logfile.print(", ");

#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");
#endif

  // fetch the time
  now = RTC.now();

  // log date
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(", ");

  //log time
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);

  //log activity
  logfile.print(", ");
  logfile.print(str);
  logfile.print(", ");
  logfile.print(stat);
  logfile.println();

#if ECHO_TO_SERIAL
  Serial.print(now.year(), DEC);
  Serial.print(F("/"));
  Serial.print(now.month(), DEC);
  Serial.print(F("/"));
  Serial.print(now.day(), DEC);
  Serial.print(F(", "));

  Serial.print(now.hour(), DEC);
  Serial.print(F(":"));
  Serial.print(now.minute(), DEC);
  Serial.print(F(":"));
  Serial.print(now.second(), DEC);

  Serial.print(F(", "));
  Serial.print(str);
  Serial.print(F(", "));
  Serial.print(stat);
  Serial.println();
#endif ECHO_TO_SERIAL



  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();

  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  //digitalWrite(statusLED, HIGH);
  logfile.flush();
  //digitalWrite(statusLED, LOW);
  digitalWrite(redLEDpin, LOW);
}

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while (1);
}

void initializeSD() {

  // initialize the SD card
  Serial.print(F("Initializing SD card..."));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println(F("card initialized."));

  // create a new file
  char filename[] = "3DMP00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print(F("Logging to: "));
  Serial.println(filename);
}

void connectToRTC() {
  // connect to RTC
  Wire.begin();
  if (!RTC.begin()) {
    logfile.println(F("RTC failed"));
#if ECHO_TO_SERIAL
    Serial.println(F("RTC failed"));
#endif  //ECHO_TO_SERIAL
  }
}






