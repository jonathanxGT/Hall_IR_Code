
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
char* productNames [] = {
  "lotion", "salve", "spot healer", "nail stick", "soap"
};
const byte productNamesCount = 5;

//setup sensor data arrays
int prev_hall_readings [hallEffectCount];
int prev_ir_readings [irSensorCount];

byte debounceDelay = 500;
boolean hallEffect = true;

void setup() {

  for (byte h = 0; h < hallEffectCount; h++) {
    pinMode(hall_effect[h], OUTPUT);
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
      //smoothData(currentHallRead, prev_hall_readings[i], i);
      //hallEffect = true;
      prev_hall_readings[i] = currentHallRead;

    }

    for (byte h = 1; h < irSensorCount; h ++) {
      currentIrRead = digitalRead(ir_sensor[h]);
      //smoothData(currentIrRead, prev_ir_readings[h], h);
      //hallEffect = false;
      prev_ir_readings[h] = currentIrRead;
    }


  }

void smoothData(int senseData, int prevSenseData, byte index) {
  int sensorData;
  int prevSensorData;
  int total,average;
  byte arrayIndex;
  

//store passed down var's
  sensorData = senseData;
  prevSensorData = prevSenseData;
  arrayIndex = index;
  

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
  debounce(average, prevSensorData);



}

void debounce(int avg, int prevData){

   int threshold;
   int sensorData = avg;
   int prevSensorData = prevData;
   long lastDebounceTime = 0;
   

  unsigned long arrayLastDebounceTime [6][1];

if (hallEffect){
  threshold = 0;
}
else {
  threshold = 850;
}
   
    if (abs(sensorData - prevSensorData) > threshold) {
     lastDebounceTime = millis();
  }
  if (millis() - lastDebounceTime > debounceDelay) {

        
      
//      checkObjects(abs(idWeight));
//      previousMeasuredWeight = measuredWeight;
      
    }    
}


  

