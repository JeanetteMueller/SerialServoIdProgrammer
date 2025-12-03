#include <SCServo.h>

SMS_STS st;
// the UART used to control servos.
// GPIO 18 - S_RXD, GPIO 19 - S_TXD, as default.
#define S_RXD 18
#define S_TXD 19

static char input[16];
static uint8_t i;
int newID = -1;
int oldID = -1;

void setup() {
  Serial.begin(115200);
  Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  st.pSerial = &Serial1;
  while (!Serial1) {}

  Serial.println("Starting: ");
  delay(1000);
}

void loop() {
  if (oldID == -1) {
    for (int i = 0; i < 1000; i++) {
      int ID = st.Ping(i);
      if (ID != -1) {
        Serial.println("");
        Serial.print("found Servo ID: ");
        Serial.println(ID, DEC);

        oldID = i;

        Serial.println("Enter new ID");
        return;

      } else {
        Serial.print(i);
        Serial.print(", ");
      }
    }
    if (oldID == -1) {
      Serial.println("");
      Serial.println("No Servo found");
    }
  } else {
    if (Serial.available() > 0) {

      char c = Serial.read();

      if (c != '\r' && i < 15) {  // assuming "Carriage Return" is chosen in the Serial monitor as the line ending character
        input[i++] = c;

      } else {
        input[i] = '\0';
        i = 0;

        newID = atoi(input);

        st.unLockEprom(oldID);                   // unlock EPROM-SAFE
        st.writeByte(oldID, SMS_STS_ID, newID);  // ID
        st.LockEprom(newID);                     // EPROM-SAFE locked

        Serial.print("programming to ");
        Serial.print(newID);
        Serial.println(" done!");

        newID = -1;
        oldID = -1;

        delay(1000);
      }
    }
  }
}
