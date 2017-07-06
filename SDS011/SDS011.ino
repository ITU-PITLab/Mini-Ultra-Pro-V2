#define Serial Serial1

unsigned int PM25 = 0;
unsigned int PM10 = 0;

void getPMValues() {
  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;
  while (Serial.available() > 0) {
    // Data sheet: http://breathe.indiaspend.org/wp-content/uploads/2015/11/nova_laser_sensor.pdf
    // packet format: AA   C0   PM25_Low   PM25_High   PM10_Low   PM10_High   0   0   CRC   AB
    mData = Serial.read();
    delay(1);
    if (mData == 0xAA) { //[0] is UART Message
      mPkt[0] =  mData;
      mData = Serial.read();
      if (mData == 0xc0)  { //Commander No.
        mPkt[1] =  mData;
        mCheck = 0;
        // [2] PM2.5 Low byte
        // [3] PM2.5 High byte
        // [4] PM10 Low byte
        // [5] PM10 High byte
        // [6] 0(Reserved)
        // [7] 0(Reserved)
        for (i = 0; i < 6; i++)
        {
          mPkt[i + 2] = Serial.read(); 
          delay(2);
          mCheck += mPkt[i + 2];
        }
        // [8] Check-sum
        mPkt[8] = Serial.read();
        delay(1);
        //message tail AB
        mPkt[9] = Serial.read();
        if (mCheck == mPkt[8]) {
          Serial.flush();
          PM25 = ((uint16_t)mPkt[2] | (uint16_t)(mPkt[3] << 8));
          PM10 = ((uint16_t)mPkt[4] | (uint16_t)(mPkt[5] << 8));
          return;
        }
      }
    }
  }
}


void setup() {
  Serial.begin(9600);
  SerialUSB.begin(9600);
}

void loop() {
  getPMValues();
  SerialUSB.print("PM2.5: ");   SerialUSB.println(float(PM25/10.0));
  SerialUSB.print("PM10: ");   SerialUSB.println(float(PM10/10.0));
  SerialUSB.println();
  delay(1000);
}
