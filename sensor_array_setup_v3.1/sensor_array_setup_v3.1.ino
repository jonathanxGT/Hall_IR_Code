
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


int debounceDelay = 750;
byte arrayIndex;


void setup() {

  Serial.begin(9600);

  for (byte h = 0; h < sensorCount; h++) {
    pinMode(sensor[h], INPUT);
    arrayLastDebounceTime[arrayIndex] = 0;
    for (byte i = 0; i < 3; i++) {
      allDataArray[sensorCount] [3] = 0;
    }
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
  int sensorDifference;
  long debounceDifference;

  //check if the reading is coming from a hall (digital) or IR (analog) sensor

  if (arrayIndex > 1) {
    threshold = 0;
  }


  //check difference to see if it's meaningful and if it's a pick or place
  sensorDifference = (avg - allDataArray[arrayIndex] [1]);

  //check if the IR sensor is a place and then adjust IR threshold for a place
  if (arrayIndex < 2 ) {
    threshold = 5;
  }
  else if (arrayIndex < 2 ){   
    threshold = 8;
  }

  // delay(10);
  
//    if (arrayIndex == 0){
//      Serial.print(arrayIndex);
//      Serial.print(' ');
//      Serial.print(threshold);
//      Serial.print(' ');
//      Serial.print(avg);
//      Serial.print(' ');
//      Serial.print(sensorDifference);
//      Serial.print(' ');
//      Serial.print(allDataArray[arrayIndex][1]);
//      Serial.print(' ');
//      Serial.println(allDataArray[arrayIndex][2]);
//    }
    


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

  Serial.print(str);
  Serial.print(' ');
  Serial.println(stat);

}

void codeDebug(long x, long y) {
  Serial.print(arrayIndex);
  Serial.print(' ');
  Serial.print(x);
  Serial.print(' ');
  Serial.print(y);
  Serial.print(' ');
  Serial.println(productName[arrayIndex]);
}






