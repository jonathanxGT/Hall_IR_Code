
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
const byte productNamesCount = 8;

//---setup sensor data arrays
int prev_sensor_readings [sensorCount];

//---setup 2D data smoothing array
int totalSensorData [sensorCount][10];
int totalDataArray [sensorCount];
int total = 0;


//---setup debounceTime array
long arrayLastDebounceTime [sensorCount];

byte debounceDelay = 100;
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
      smoothData(currentSensorRead, j);

    }
    //delay(500);
  }

}

void smoothData(int sensorData, byte j) {

  int average = 0;
  //for reference
  // int totalSensorData [sensorCount][10];


  //data smoothing

  totalDataArray [arrayIndex] = totalDataArray [arrayIndex] - totalSensorData[arrayIndex][j];
  totalSensorData[arrayIndex][j] = sensorData;
  totalDataArray [arrayIndex] = totalDataArray [arrayIndex] + totalSensorData[arrayIndex][j];

  //  for ( byte k = 0; k < 10; k++){
  //    total = total + totalSensorData[arrayIndex][k];
  //  }
  average = totalDataArray [arrayIndex] / 10;


  //codeDebug(totalSensorData[arrayIndex][j]);
  delay(1);
  //Serial.println(average);
  //debounce timer
  debounceAndCheck(average);

}

void debounceAndCheck(int avg) {

  int threshold;
  int sensorDifference;
  boolean pickedUp = false;

  //codeDebug(avg);
  //check if the reading is coming from a hall (digital) or IR (analog) sensor
  if (hallEffect) {
    threshold = 0;
  }
  else {
    threshold = 1024;
  }

  //check difference to see if it's meaningful and if it's a pick or place
  sensorDifference = (avg -  prev_sensor_readings[arrayIndex]);

  if (abs(sensorDifference) > threshold) {
    arrayLastDebounceTime[arrayIndex] = millis();
    pickedUp = true;
    if (arrayIndex == 3) {
      codeDebug(sensorDifference);
    }

  }


  if ((millis() - arrayLastDebounceTime[arrayIndex]) > debounceDelay) {
      if (arrayIndex == 3) {
      Serial.println((millis() - arrayLastDebounceTime[arrayIndex]));
    }
    if (pickedUp) {

      //determine if it's a pick or place
      if (sensorDifference > 0) {
        logData(productName[arrayIndex], "pick");
      }
      else {
        logData(productName[arrayIndex], "place");
      }
      pickedUp = false;
    }

  }
  prev_sensor_readings[arrayIndex] = avg;

}


void logData(char *str, char *stat) {

  Serial.print(str);
  Serial.print(' ');
  Serial.println(stat);

}

void codeDebug(int x) {
  Serial.print(arrayIndex);
  Serial.print(' ');
  Serial.print(x);
  Serial.print(' ');
  Serial.println(productName[3]);
}






