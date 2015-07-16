void setup() {
  // put your setup code here, to run once:

}

void loop() {
  

}

void smoothData(int senseData, int prevSenseData) {
  int sensorData;
  int prevSensorData;
  int total,average;

  sensorData = senseData;
  prevSensorData = prevSenseData;

int totalSensorData [10];
int totalSensorDataIndex = 0;
  //data smoothing
  total = total - totalSensorData[totalSensorDataIndex];
  totalSensorData[totalSensorDataIndex] = sensorData;
  total = total + totalSensorData[totalSensorDataIndex];

  totalSensorDataIndex = totalSensorDataIndex + 1;

  if (totalSensorDataIndex >= 10) {
    totalSensorDataIndex = 0;
  }

  average = total / 10;
  delay(1);

}


void detectChange(int senseData, int prevSenseData) {
  int sensorData;
  int prevSensorData;

  sensorData = senseData;
  prevSensorData = prevSenseData;




  //end data smoothing
  
  if (sensorData - prevSensorData > weightThresh) {
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
      checkObjects(abs(idWeight));
      previousMeasuredWeight = measuredWeight;
      movedForReal = false;
    }    
  }
  
  previousTotalWeight = totalWeight;
  return;
}
