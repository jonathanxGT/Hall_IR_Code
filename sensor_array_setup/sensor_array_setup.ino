
//---setup sensor pin array
byte sensor [] = {
  A1, A2, 3, 4, 5, 6, 7, 8
};
const byte sensorCount = 8;

//---setup product names array
char* productName [] = {
  "soap", "salve", "lotion 1", "lotion 2", "spot healer 1", "spot heal 2", "nail stick 1", "nail stick 2"
};
const byte productNamesCount = 8;

//---setup sensor data arrays
int prev_sensor_readings [sensorCount];

byte debounceDelay = 1000;
boolean hallEffect = true;
byte arrayIndex;
int prevSensorReading;

void setup() {

  Serial.begin(9600);

  for (byte h = 0; h < sensorCount; h++) {
    pinMode(sensor[h], INPUT);
    prev_sensor_readings [h] = 0;
  }

}

void loop() {
  readSensors();
  delay(10000);
}

void readSensors() {

  //vars for raw sensor data
  int currentSensorRead;
  for (byte j = 0; j < 10; j  ++) {
    for (byte h = 0; h < sensorCount; h ++) {
      if (h < 2) {
        currentSensorRead = analogRead(sensor[h]);
        hallEffect = false;
      }
      else {
        currentSensorRead = digitalRead(sensor[h]);
        hallEffect = true;
      }
      arrayIndex = h;
      prevSensorReading = prev_sensor_readings[h];
      smoothData(currentSensorRead, j);

    }
  }

}

void smoothData(int senseData, byte j) {
  int sensorData;
  int total = 0;
  int average = 0;
  

  //store passed down var's
  sensorData = senseData;

  //setup 2D data smoothing array
  int totalSensorData [sensorCount][10];


  //data smoothing
  total = total - totalSensorData[arrayIndex][j];
  totalSensorData[arrayIndex][j] = sensorData;
  total = total + totalSensorData[arrayIndex][j];

  average = total / 10;
  
  delay(1);
  //Serial.println(average);
  //debounce timer
  debounceAndCheck(average);

}

void debounceAndCheck(int avg) {

  int threshold;
  int sensorDifference;
  unsigned long arrayLastDebounceTime [sensorCount];

  //check if the reading is coming from a hall (digital) or IR (analog) sensor
  if (hallEffect) {
    threshold = 0;
  }
  else {
    threshold = 1024;
  }

  //check difference to see if it's meaningful and if it's a pick or place
  sensorDifference = avg - prevSensorReading;

  codeDebug(sensorDifference);

  if (abs(sensorDifference) > threshold) {
    arrayLastDebounceTime[arrayIndex] = millis();
  }
  if (millis() - arrayLastDebounceTime[arrayIndex] > debounceDelay) {

    //determine if it's a pick or place
    if (sensorDifference > 0) {
      logData(productName[arrayIndex], "pick");
    }
    else {
      logData(productName[arrayIndex], "place");
    }
  }
  prev_sensor_readings[arrayIndex] = avg;
}


void logData(char *str, char *stat) {

  Serial.print(str);
  Serial.print(' ');
  Serial.println(stat);

}

void codeDebug(int sensorDiff) {
  Serial.print(arrayIndex);
  Serial.print(' ');
  Serial.print(sensorDiff);
  Serial.print(' ');
  Serial.println(productName[arrayIndex]);
}






