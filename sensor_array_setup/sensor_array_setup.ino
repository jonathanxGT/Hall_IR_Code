
//setup hall effect pin array
byte hall_effect [] = {
  8, 7, 6, 5, 4, 3
};
const byte hallEffectCount = 6;


//setup IR Sensor array
byte ir_sensor [] = {
  1, 2
};
const byte irSensorCount = 2;

//setup product names array
char* productName [] = {
  "soap", "salve", "lotion", "spot healer", "nail stick"
};
const byte productNamesCount = 5;

//setup sensor data arrays
int prev_hall_readings [hallEffectCount];
int prev_ir_readings [irSensorCount];

byte debounceDelay = 500;
boolean hallEffect = true;
byte arrayIndex;
int prevSensorReading;

void setup() {

  for (byte h = 3; h < hallEffectCount; h++) {
    pinMode(hall_effect[h], OUTPUT);
  }

  for (byte i = 1; i < irSensorCount; i++){
    pinMode(ir_sensor[i], OUTPUT);
  }


}

void loop() {
  readSensors();

}

void readSensors() {
    int currentHallRead;
    int currentIrRead;
    for (byte i = 3; i < hallEffectCount; i++) {
      currentHallRead = digitalRead(hall_effect[i]);
      //smoothData(currentHallRead, prev_hall_readings[i]);
      //hallEffect = true;
      // arrayIndex = i;
      //prevSensorReading = prev_hall_readings[i]
      prev_hall_readings[i] = currentHallRead;

    }

    for (byte h = 1; h < irSensorCount; h ++) {
      currentIrRead = digitalRead(ir_sensor[h]);
      //smoothData(currentIrRead, prev_ir_readings[h]);
      //hallEffect = false;
      //arrayIndex = h;
      //prevSensorReading = prev_ir_readings[h];
      prev_ir_readings[h] = currentIrRead;
    }


  }

void smoothData(int senseData) {
  int sensorData;
  int total,average;  

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
  debounce(average);



}

void debounce(int avg){

   int threshold;
   int sensorData = avg;
//   long lastDebounceTime = 0;
   

  unsigned long arrayLastDebounceTime [6];

if (hallEffect){
  threshold = 0;
}
else {
  threshold = 850;
}


   
    if (abs(sensorData - prevSensorReading) > threshold) {
     arrayLastDebounceTime[arrayIndex] = millis();
  }
  if (millis() - arrayLastDebounceTime[arrayIndex] > debounceDelay) { 
//      logData(productName[arrayIndex],
      
    }    
}

void logData(char *str, char *stat) {
  
}




  

