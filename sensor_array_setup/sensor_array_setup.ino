
//---setup hall effect pin array
byte hall_effect [] = {
  8, 7, 6, 5, 4, 3
};
const byte hallEffectCount = 6;


//---setup IR Sensor array
byte ir_sensor [] = {
  1, 2
};
const byte irSensorCount = 2;

//---setup product names array
char* productName [] = {
  "soap", "salve", "lotion", "spot healer", "nail stick"
};
const byte productNamesCount = 5;

//---setup sensor data arrays
int prev_hall_readings [hallEffectCount];
int prev_ir_readings [irSensorCount];

byte debounceDelay = 500;
boolean hallEffect = true;
byte arrayIndex;
int prevSensorReading;

void setup() {

  Serial.begin(9600);

  for (byte h = 3; h < hallEffectCount; h++) {
    pinMode(hall_effect[h], OUTPUT);
  }

  for (byte i = 1; i < irSensorCount; i++) {
    pinMode(ir_sensor[i], OUTPUT);
  }



}

void loop() {
  readSensors();
}

void readSensors() {

  //vars for raw sensor data
  int currentHallRead;
  int currentIrRead;
  
  for (byte i = 3; i < hallEffectCount; i++) {
    currentHallRead = digitalRead(hall_effect[i]);
    hallEffect = true;
    arrayIndex = i;
    prevSensorReading = prev_hall_readings[i];
    smoothData(currentHallRead);
    prev_hall_readings[i] = currentHallRead;

  }

  for (byte h = 1; h < irSensorCount; h ++) {
    currentIrRead = digitalRead(ir_sensor[h]);
    hallEffect = false;
    arrayIndex = h;
    prevSensorReading = prev_ir_readings[h];
    smoothData(currentIrRead);
    prev_ir_readings[h] = currentIrRead;
  }


}

void smoothData(int senseData) {
  int sensorData;
  int total, average;

  //store passed down var's
  sensorData = senseData;

  //setup 2D data smoothing array
  int totalSensorData [6][10];
  int totalSensorIndex = 0;

  //data smoothing
  total = total - totalSensorData[arrayIndex][totalSensorIndex];
  totalSensorData[arrayIndex][totalSensorIndex] = sensorData;
  total = total + totalSensorData[arrayIndex][totalSensorIndex];

  totalSensorIndex = totalSensorIndex + 1;

  if (totalSensorIndex >= 10) {
    totalSensorIndex = 0;
  }

  average = total / 10;
  delay(1);

  //debounce timer
  debounceAndCheck(average);

}

void debounceAndCheck(int avg) {

  int threshold;
  int sensorData = avg;
  int sensorDifference;
  unsigned long arrayLastDebounceTime [6];

  //check if the reading is coming from a hall (digital) or IR (analog) sensor
  if (hallEffect) {
    threshold = 0;
  }
  else {
    threshold = 850;
  }

  //check difference to see if it's meaningful and if it's a pick or place
  sensorDifference = sensorData - prevSensorReading;

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
}


void logData(char *str, char *stat) {

Serial.print(str);
Serial.print(' ');
Serial.println(stat);

}






