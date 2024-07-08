#include<SoftwareSerial.h>
int value;
String voice = ""; // Initialize voice variable outside loop()
int maxPeople = 10; // maximum number of people allowed before the alarm goes off
int sensitivity = 10; // lower values will make it more sensitive and higher values will make it less sensitive
int lightFlag=0;
int fanFlag=0;
int currentPeople = 0;
int manuallyAllOff=0;
int insideSensor[] = {9, 8}; // Pin configuration for the sensor placed inside the room
int outsideSensor[] = {11,10}; // Pin configuration for the sensor placed outside the room
int insideSensorInitial;
int outsideSensorInitial;

String sequence = "";

int timeoutCounter = 0;

void setup() {
  // Setup code
  Serial.begin(9600);

  delay(500);
  insideSensorInitial = measureDistance(insideSensor);
  outsideSensorInitial = measureDistance(outsideSensor);
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  // pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4,HIGH);
  digitalWrite(3,HIGH);

}

void loop() {
    while(Serial.available()) {
    char c = Serial.read();
    if (c != '\n') { // Check for newline character to end the string
      voice += c;
    } else {
      processVoiceCommand();
      voice = ""; // Reset voice string for next command
    }
  }
  // Read ultrasonic sensors
  int insideSensorVal = measureDistance(insideSensor);
  int outsideSensorVal = measureDistance(outsideSensor);
  
  // Process the data
  if (insideSensorVal < 10 && sequence.charAt(0) != '1') {
    sequence += "1";
  } else if (outsideSensorVal < 10 && sequence.charAt(0) != '2') {
    sequence += "2";
  }
  
  if (sequence.equals("21")) {
    currentPeople++;  
    sequence = "";
    Serial.print(" Entered ");
    delay(500); // Delay to avoid multiple counts for the same person
  }  if (sequence.equals("12") && currentPeople > 0) {
    currentPeople--;  
    sequence = "";
    Serial.print(" Gone outside ");
    delay(500); // Delay to avoid multiple counts for the same person
  }

  // Resets the sequence if it is invalid or timeouts
  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 20) {
    sequence = "";  
  }
  if(sequence.equals("1")&& timeoutCounter>5){
    sequence="";
  }

  if (sequence.length() == 1) {
    timeoutCounter++;
  } else {
    timeoutCounter = 0;
  }
 
  // Print values to serial
  Serial.print(" Seq: ");
  Serial.print(sequence);
  Serial.print(" Inside: ");
  Serial.print(insideSensorVal);
  Serial.print(" Outside: ");
  Serial.println(outsideSensorVal);
  Serial.print("  Current people ");
  Serial.print(currentPeople);
  //delay(350);

 if(currentPeople>0 && !fanFlag && !lightFlag && !manuallyAllOff)
  {
    digitalWrite(3,LOW);
    lightFlag=1;
    digitalWrite(4,LOW);
    fanFlag=1;
  }
  else if(currentPeople==0)
  {
    digitalWrite(3,HIGH);
    lightFlag=0;
    digitalWrite(4,HIGH);
    fanFlag=0;
    
  }
 
}

// Returns the distance of the ultrasonic sensor that is passed in
// a[0] = echo, a[1] = trig
int measureDistance(int a[]) {
  pinMode(a[1], OUTPUT);
  pinMode(a[0], INPUT);
  digitalWrite(a[1], LOW);
  delayMicroseconds(2);
  digitalWrite(a[1], HIGH);
  delayMicroseconds(10);
  digitalWrite(a[1], LOW);
  long duration = pulseIn(a[0], HIGH, 1000000);
  return duration / 29 / 2;
}
void processVoiceCommand() {
  if (voice.equals("all on")) {
    digitalWrite(3, LOW);
     digitalWrite(4, LOW);
     manuallyAllOff=0;
    Serial.println("Light fan turned on");
  }
  if (voice.equals("all off")) {
    digitalWrite(3, HIGH);
     digitalWrite(4, HIGH);
      manuallyAllOff=1;
    Serial.println("Ligh fan turned off");
  }
  if (voice.equals("light on")) {
    lightFlag=1;
    manuallyAllOff=0;
    digitalWrite(3, LOW);
    Serial.println("Light turned on");
  }
  if (voice.equals("light off")) {
    if(!fanFlag){
      manuallyAllOff=1;
    }
    lightFlag=0;

    digitalWrite(3, HIGH);
    Serial.println("Light turned off");
  }
   if (voice.equals("fan on")) {
    manuallyAllOff=0;
    fanFlag=1;
    digitalWrite(4, LOW);
    Serial.println("Fan turned on");
  }
  if (voice.equals("fan off")) {
     if(!lightFlag){
      manuallyAllOff=1;
    }
    fanFlag=0;
    digitalWrite(4, HIGH);
    Serial.println("Fan turned off");
  }
}