
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
boolean movedForReal = false;
boolean pickedUp = false;

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
      //smoothData(currentHallRead, prev_hall_readings[i], i, true);
      prev_hall_readings[i] = currentHallRead;

    }

    for (byte h = 1; h < irSensorCount; h ++) {
      currentIrRead = digitalRead(ir_sensor[h]);
      //smoothData(currentIrRead, prev_ir_readings[h], h, false);
      prev_ir_readings[h] = currentIrRead;
    }


  }

void smoothData(int senseData, int prevSenseData, byte index, boolean sensorGate) {
  int sensorData;
  int prevSensorData;
  int total,average;
  byte arrayIndex;
  boolean hallEffect;

//store passed down var's
  sensorData = senseData;
  prevSensorData = prevSenseData;
  arrayIndex = index;
  hallEffect = sensorGate;

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
if (hallEffect){
  hallDebounce(average, prevSensorData);
}
else {
  irDebounce(average, prevSensorData);
}


}

void hallDebounce(int avg, int prevData){

   int threshold = 0;
   int sensorData = avg;
   int prevSensorData = prevData;
   long lastDebounceTime = 0;
   
    if (abs(sensorData - prevSensorData) > threshold) {
     lastDebounceTime = millis();
  }
  if (millis() - lastDebounceTime > debounceDelay) {

        pickedUp = true;
      
//      checkObjects(abs(idWeight));
//      previousMeasuredWeight = measuredWeight;
      
    }    
}

void irDebounce(int avg, int prevData){
   
}

  

