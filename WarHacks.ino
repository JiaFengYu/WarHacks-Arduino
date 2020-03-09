#include <NewPing.h>
#include <AFMotor.h>

#define TRIGGER_PIN  A1  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A0  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
const int SENSOR_L = A4;
const int SENSOR_R = A5;
AF_DCMotor motor_l(1);
AF_DCMotor motor_r(2);

void move(int spdl, int spdr) {
  motor_l.setSpeed(spdl);
  motor_r.setSpeed(spdr);
}

void move_stop() {
  motor_l.run(RELEASE);
  motor_r.run(RELEASE);
}

void move_f() {
  move(100, 100);
  motor_l.run(FORWARD);
  motor_r.run(FORWARD);
 }
void move_l() {
  move(50, 55);
//  move(50, 100);
  motor_l.run(BACKWARD);
//  motor_l.run(FORWARD);
  motor_r.run(FORWARD);
}
void move_r() {
  move(55, 50);
//  move(100, 50);
  motor_l.run(FORWARD);
  motor_r.run(BACKWARD);
//  motor_r.run(FORWARD);
}

int bright_l = 0;
int bright_r = 0;
int distance = 10000;
bool dodged = false;

bool should_turn_l() { return bright_l <= 100 && bright_r >= 450; }
bool should_turn_r() { return bright_l >= 150 && bright_r <= 400; }
bool should_turn_f() { return bright_l >= 150 && bright_r >= 450; }

void track_lane() {
  if (should_turn_l()) {
    move_l();
  } else if (should_turn_r()) {
    move_r();
  } else if (should_turn_f()) {
    move_f();
  }
}

bool read() {
  bright_l = analogRead(SENSOR_L);
  bright_r = analogRead(SENSOR_R);
  distance = sonar.ping_cm();
//  Serial.print("ping: ");
//  Serial.print(distance);
//  Serial.print("; sensor: l:");
//  Serial.print(bright_l);
//  Serial.print(" r:");
//  Serial.println(bright_r);
  return true;
}

void change_lane(bool left) {
  if (!left) { 
    Serial.println("GOING LEFT");
    move_l();
    delay(250);
    move_f();
    delay(500);
  }
  else {
    Serial.println("GOING RIGHT");
    move_r();
    delay(140);
    move_f();
    delay(300);
  }

  while (read() && !should_turn_f())
    ;                         

  Serial.println("DONE CHANGING LANES");
}

void dodge() {
  Serial.println("DODGING");
  change_lane(true);
  int total = 0;
  while (read() && total <= 350) {
    track_lane();
    if (should_turn_f()) {
      move(120, 120);
      delay(50);
    }
    total += 1;
    delay(4);
  }
  change_lane(false);
  Serial.println("DONE DODGING");
}

void loop() {
  delay(5);
  read();
  track_lane();

  if (!dodged && distance >= 4 && distance <= 28) {
    dodge();
    dodged = true;
  }
}

void setup() {
  Serial.begin(9600);
  move_stop();
  dodged = false;
}
