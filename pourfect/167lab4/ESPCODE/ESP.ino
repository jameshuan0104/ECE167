/*
This is the driver code for the esp used to calculate cup height and then communicate with the
STM.
*/











const int numPins = 10;                  // Total ADC pins
const int numReadings = 6;              // Moving average window size
// Define your ADC pins here
const int adcPins[numPins] = {36,39,34,35,32,33,25,26,27,14};
int readings[numPins][numReadings];      // Stores readings for each pin
int readIndex[numPins] = {0};            // Current index for each pin
long total[numPins] = {0};               // Running total for each pin
int average[numPins] = {0};              // Average ADC value for each pin
int binary[numPins] = {0};

HardwareSerial MySerial(1); // UART1 instance
byte dataList[10]= {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
//byte dataList[1]= {0x01};
void setup() {
  
  Serial.begin(9600);
  analogReadResolution(12);              // 12-bit resolution
  MySerial.begin(9600, SERIAL_8N1, 16, 17);

  // Initialize readings array
  for (int pin = 0; pin < numPins; pin++) {
    for (int i = 0; i < numReadings; i++) {
      readings[pin][i] = 0;
    }
  }
}

void loop() {
  for (int pin = 0; pin < numPins; pin++) {
    total[pin] = total[pin] - readings[pin][readIndex[pin]];        // Remove oldest
    readings[pin][readIndex[pin]] = analogRead(adcPins[pin]);       // Read ADC
    total[pin] = total[pin] + readings[pin][readIndex[pin]];        // Add new reading
    readIndex[pin]++;                                               // Move to next index
    if (readIndex[pin] >= numReadings) {
      readIndex[pin] = 0;
    }
    Printav(pin);
                                            
  }
  // Send the entire array at once
  MySerial.write(dataList,10);
  delay(50); // Delay between transmissions
}

void Printav(int pin){
  int average = total[pin]/numReadings;
  // Hysteresis for moving average filter
    if(average >= 2150){
      dataList[pin] = 0x01;
    }else if(average <= 2100){
      dataList[pin] = 0x00;
    }

Serial.print("pin: ");
Serial.print(pin);
Serial.print("binary: ");
Serial.println(dataList[pin]);
}
