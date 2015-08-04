
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
#include <Adafruit_VC0706.h>

#define LOG_INTERVAL  1000
#define SYNC_INTERVAL 1000
uint32_t syncTime = 0;
uint32_t m = 0;

#define ECHO_TO_SERIAL   1

#define redLEDpin 2

RTC_DS1307 RTC;

const int chipSelect = 10;

String lastHour, lastFewMin;

//---setup sensor pin array
int sensor [] = {
  A1, A2, 3, 4, 5, 6, 7, 8
};
const byte sensorCount = 8;

//---setup product names array
char* productName [] = {
  "salve", "soap", "spot healer 2", "spot healer 1", "lotion 2",
  "lotion 1", "nail stick 1", "nail stick 2"
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

int debounceDelay = 500;
byte arrayIndex;

//--- Camera setup
SoftwareSerial cameraconnection = SoftwareSerial(9, 10);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

File logfile;
void setup() {

  if (chipSelect != 10) pinMode(10, OUTPUT); // SS on Uno, etc.
  Serial.begin(9600);

  //---- Logging setup ----//
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);

  connectToRTC();
  // initializeSD();
  setupCam();


  for (byte h = 0; h < sensorCount; h++) {
    pinMode(sensor[h], INPUT);
    allDataArray[h][2] = 0;
    arrayLastDebounceTime[h] = 0;
    for (byte i = 0; i < 3; i++) {
      allDataArray[sensorCount] [3] = 0;
    }
  }

}

void loop() {
  m = millis();
  readSensors();
  newFile();
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
  int irThresh = 1;
  int sensorDifference;
  long debounceDifference;



  //check if the reading is coming from a hall (digital) or IR (analog) sensor

  if (arrayIndex > 1) {
    threshold = hallThresh;
  }

  //check difference to see if it's meaningful and if it's a pick or place
  sensorDifference = (avg - allDataArray[arrayIndex] [1]);

  //check if the IR sensor is a place and then adjust IR threshold for a place
  if (arrayIndex < 2 && allDataArray[arrayIndex][2] > 0) {
    threshold = irThresh;
  }
  else if (arrayIndex < 2 && allDataArray[arrayIndex][2] < 0) {
    threshold = irThresh + 2;
  }


  if (abs(sensorDifference) > threshold) {

    //store difference in array for each sensor
    allDataArray[arrayIndex][2] = sensorDifference;

    //see if reading was greater than debounce delay
    debounceDifference = millis() - arrayLastDebounceTime[arrayIndex];

    //ignores readings of sensors in the first second.
    if (millis() < 1000) {
      debounceDifference = 0;
    }
    
    if ( debounceDifference > debounceDelay) {

      //determine if it's a pick or place
      if (allDataArray[arrayIndex][2] > 0) {
        Serial.println(productName[arrayIndex]);
          takePic();
        //logData(productName[arrayIndex], "pick");
      }

      else if (allDataArray[arrayIndex][2] < 0) {

        // logData(productName[arrayIndex], "place");
      }
      arrayLastDebounceTime[arrayIndex] = millis();
    }
  }

  allDataArray[arrayIndex] [1] = avg;

}

void logData(char *str, char *stat) {

  DateTime now;

  // fetch the time
  now = RTC.now();


  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  //digitalWrite(statusLED, HIGH);
  logfile.flush();
  //digitalWrite(statusLED, LOW);
  digitalWrite(redLEDpin, LOW);
}

void error(char *str)
{
  Serial.print(F("error: "));
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
  createNewFile();


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

void newFile() {

  DateTime newDate;
  newDate = RTC.now();

  String newDateCheck;
  newDateCheck = String(newDate.hour()) + ':' + String(newDate.minute()) + ':' + String(newDate.second());


  if ( newDateCheck == "20:00:0") {

    logfile.close();
    delay(1000);
    createNewFile();
    logData("new file", "time");
  }

  else if (newDateCheck == "8:00:0") {
    logfile.close();
    delay(1000);
    createNewFile();
    logData("new file", "time");
  }

}

void createNewFile() {

  DateTime fileDate;
  fileDate = RTC.now();

  // for hourly new file timer.
  lastHour = String(fileDate.month());
  lastFewMin = (String(fileDate.hour()) + ':' + String(fileDate.minute() + 5));

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

void setupCam() {

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  // Try to locate the camera
  if (cam.begin()) {
    Serial.println(F("Camera Found:"));
  } else {
    Serial.println(F("No camera found?"));
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print(F("Failed to get version"));
  } else {
    Serial.println(F("-----------------"));
    Serial.print(reply);
    Serial.println(F("-----------------"));
  }

  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120
  // Remember that bigger pictures take longer to transmit!

  cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");
}

void takePic() {
  if (! cam.takePicture())
    Serial.println(F("Failed to snap!"));
  else
    Serial.println(F("Picture taken!"));

  // Create an image with the name IMAGExx.JPG
  char filename[13];
  strcpy(filename, "IMAGE00.JPG");
  for (int i = 0; i < 100; i++) {
    filename[5] = '0' + i / 10;
    filename[6] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  // Open the file for writing
  File imgFile = SD.open(filename, FILE_WRITE);

  // Get the size of the image (frame) taken
  uint16_t jpglen = cam.frameLength();
  Serial.print(F("Storing "));
  Serial.print(jpglen, DEC);
  Serial.print(F(" byte image."));

  int32_t time = millis();
  pinMode(8, OUTPUT);
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    imgFile.write(buffer, bytesToRead);
    if (++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
      Serial.print(F('.'));
      wCount = 0;
    }
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  imgFile.close();

  time = millis() - time;
  Serial.println(F("done!"));
  Serial.print(time); Serial.println(F(" ms elapsed"));
}






