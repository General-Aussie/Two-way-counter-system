//This code is for a counter to be placed in a shop entrance with separate entry and exit channels

#include <NewPing.h>
//Defining where the components are attached
int TRIG_IN = A1;
int TRIG_OUT = 3;
int ECHO_IN = A2;
int ECHO_OUT = 4;
int LED_WAIT = 12;
int LED_ENTER = 9;

#define iterations 2 //Number of readings in the calibration stage
#define MAX_DISTANCE 150 // Maximum distance (in cm) for the sensors to try to read.
#define DEFAULT_DISTANCE 45 // Default distance (in cm) is only used if calibration fails.
#define MIN_DISTANCE 15 // Minimum distance (in cm) for calibrated threshold.

float calibrate_in = 0, calibrate_out = 0; // The calibration in the setup() function will set these to appropriate values.
float distance_in, distance_out; // These are the distances (in cm) that each of the Ultrasonic sensors read.
int count = 0, limit = 2; //Occupancy limit should be set here: e.g. for maximum 20 people in the shop set 'limit = 20'.
bool prev_inblocked = false, prev_outblocked = false; //These booleans record whether the entry/exit was blocked on the previous reading of the sensor.

NewPing sonar[2] = {   // Sensor object array.
  NewPing(TRIG_IN, ECHO_IN, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(TRIG_OUT, ECHO_OUT, MAX_DISTANCE)
};

/*
   A quick note that the sonar.ping_cm() function returns 0 (cm) if the object is out of range / nothing is detected.
   We will include a test to remove these erroneous zero readings later.
*/

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
 pinMode(4, OUTPUT);
 pinMode(3, OUTPUT);
 pinMode(A2, OUTPUT);
pinMode(A1, OUTPUT);
 pinMode(11, OUTPUT);
  digitalWrite(4, HIGH);
 digitalWrite(3, LOW);
 digitalWrite(A2, HIGH);
 digitalWrite(A1, LOW);
 digitalWrite(11, LOW);
  pinMode(LED_WAIT, OUTPUT), pinMode(LED_ENTER, OUTPUT);
  digitalWrite(LED_WAIT, HIGH); digitalWrite(LED_ENTER, HIGH); //Both LEDs are lit to alert user to ongoing calibration.
  Serial.println("Calibrating...");
  delay(1500);
  for (int a = 0; a < iterations; a++) {
    delay(50);
    calibrate_in += sonar[0].ping_cm();
    delay(50);
    calibrate_out += sonar[1].ping_cm();
    delay(200);
  }
  calibrate_in = 0.75 * calibrate_in / iterations; //The threshold is set at 75% of the average of these readings. This should prevent the system counting people if it is knocked.
  calibrate_out = 0.75 * calibrate_out / iterations;

  if (calibrate_in > MAX_DISTANCE || calibrate_in < MIN_DISTANCE) { //If the calibration gave a reading outside of sensible bounds, then the default is used
    calibrate_in = DEFAULT_DISTANCE;
  }
  if (calibrate_out > MAX_DISTANCE || calibrate_out < MIN_DISTANCE) {
    calibrate_out = DEFAULT_DISTANCE;
  }

  Serial.print("Entry threshold set to: ");
  Serial.println(calibrate_in);
  Serial.print("Exit threshold set to: ");
  Serial.println(calibrate_out);
  digitalWrite(LED_WAIT, LOW); digitalWrite(LED_ENTER, LOW); //Both LEDs are off to alert user that calibration has finished.
  delay(1000);
}

void loop() {
  //  Serial.print("Count: ");
  //  Serial.println(count);
  distance_in = sonar[0].ping_cm();
  delay(40); // Wait 40 milliseconds between pings. 29ms should be the shortest delay between pings.
  distance_out = sonar[1].ping_cm();
  delay(40);
  if (distance_in < calibrate_in && distance_in > 0) { // If closer than wall/calibrated object (person is present) && throw out zero readings
    if (prev_inblocked == false) {
      count++; // Increase count by one
      Serial.print("Count: ");
      Serial.println(count);
    }
    prev_inblocked = true;
  } else {
    prev_inblocked = false;
  }
  if (distance_out < calibrate_out && distance_out > 0) {
    if (prev_outblocked == false) {
      count--; // Decrease count by one
      Serial.print("Count: ");
      Serial.println(count);
    }
    prev_outblocked = true;
  } else {
    prev_outblocked = false;
  }
//    //If there are fewer people in the shop than the limit, light is green, else it is red
  if (count < limit) {
    digitalWrite(LED_WAIT, LOW);
    digitalWrite(LED_ENTER, HIGH);
  } else {
    digitalWrite(LED_WAIT, HIGH);
    digitalWrite(LED_ENTER, LOW);
  }
}
