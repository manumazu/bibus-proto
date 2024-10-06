#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9); // RX, TX

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);
  Serial.begin(9600);

  // set bibus name
  sendCommand("AT+NAMEyour_name");
}

void sendCommand(const char * command){
  Serial.print("Command send :");
  Serial.println(command);
  mySerial.println(command);
  //wait some time
  delay(100);
  
  char reply[100];
  int i = 0;
  while (mySerial.available()) {
    reply[i] = mySerial.read();
    i += 1;
  }
  //end the string
  reply[i] = '\0';
  Serial.print(reply);
  Serial.println("Reply end");
}

void loop() {

}
