#include <SCServo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

SMS_STS st;
// the UART used to control servos.
// GPIO 18 - S_RXD, GPIO 19 - S_TXD, as default.
#define S_RXD 18
#define S_TXD 19

// On-board OLED (Waveshare ESP32 Servo Driver): SSD1306 128x32 on I2C 0x3C.
// GPIO 21 - S_SDA, GPIO 22 - S_SCL, as default.
#define S_SDA 21
#define S_SCL 22
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDR   0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

static char input[16];
static uint8_t i;
int newID = -1;
int oldID = -1;

// Show a small header line and a large value line on the OLED.
void showStatus(const char* header, const char* big) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println(header);
  display.setCursor(0, 12);
  display.setTextSize(2);
  display.println(big);
  display.display();
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  st.pSerial = &Serial1;
  while (!Serial1) {}

  Wire.begin(S_SDA, S_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR)) {
    Serial.println("SSD1306 allocation failed");
  }
  showStatus("Starting", "");

  Serial.println("Starting: ");
  delay(1000);
}

void loop() {
  if (oldID == -1) {
    showStatus("Scanning bus...", "0-999");
    for (int i = 0; i < 1000; i++) {
      int ID = st.Ping(i);
      if (ID != -1) {
        Serial.println("");
        Serial.print("found Servo ID: ");
        Serial.println(ID, DEC);

        oldID = i;

        char buf[8];
        snprintf(buf, sizeof(buf), "%d", ID);
        showStatus("Found Servo ID", buf);

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
      showStatus("No servo", "found");
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

        char buf[8];
        snprintf(buf, sizeof(buf), "%d", newID);
        showStatus("Programmed ID", buf);

        newID = -1;
        oldID = -1;

        delay(1000);
      }
    }
  }
}
