
//setup hall effect pin array
byte hall_effect [] = {
  8, 7, 6, 5, 4, 3
};
const byte hallEffectCount = 6;


//setup IR Sensor array
byte ir_sensor [] = {
  2, 3
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

long lastDebounceTime = 0;
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
      //smoothData(currentHallRead, prev_hall_readings[i], true);
      prev_hall_readings[i] = currentHallRead;

    }

    for (byte h = 2; h < irSensorCount; h ++) {
      currentIrRead = digitalRead(ir_sensor[h]);
      //smoothData(currentIrRead, prev_ir_readings[h], false);
      prev_ir_readings[h] = currentIrRead;
    }


  }

void smoothData(int senseData, int prevSenseData, boolean sensorGate) {
  int sensorData;
  int prevSensorData;
  int total,average;
  boolean hallEffect;

//store passed down var's
  sensorData = senseData;
  prevSensorData = prevSenseData;
  hallEffect = sensorGate;

//setup data smoothing array
int totalSensorData [10];
int totalSensorIndex = 0;

  //data smoothing
  total = total - totalSensorData[totalSensorIndex];
  totalSensorData[totalSensorIndex] = sensorData;
  total = total + totalSensorData[totalSensorIndex];

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
//   boolean movedForReal = false;
//   boolean pickedUp = false;
//   long lastDebounceTime;
   
    if (sensorData - prevSensorData > threshold) {
     lastDebounceTime = millis();
    movedForReal = true;
  }
  if (millis() - lastDebounceTime > debounceDelay) {
    measuredWeight = totalWeight;
    if (movedForReal) {
      idWeight =  measuredWeight - previousMeasuredWeight;
      if (idWeight > 0) {
        pickedUp = true;
      }
      else {
        pickedUp = false;
      }
//      checkObjects(abs(idWeight));
//      previousMeasuredWeight = measuredWeight;
      movedForReal = false;
    }    
  }
}

void irDebounce(int avg, int prevData){
   
}

  

