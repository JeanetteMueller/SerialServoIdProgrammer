# SerialServoIdProgrammer
Helper to program your Serial Servo motors like ST3020

Use the Arduino IDE to set Servo IDs.
Open Serial Monitor and change LineBreak to CR/LF and 115200 Baud.
On reboot the Code searches for the lowest ID. 
When found, use the Message row to enter a new Number. 
After setting the scann will restart and hopefully find your servo with the new ID. 