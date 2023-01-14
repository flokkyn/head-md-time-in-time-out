#define VIBRATOR 9

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Vector_datatype.h>
#include <vector_type.h>


Adafruit_MPU6050 mpu;

bool mustInit = true;
vec3_t accPrev;
float averageMovement = 0;
float averagePeak = 0;

float satiety = 0;
float deltaTime = 0;
unsigned long ms = 0;
unsigned long lastMs = 0;

int compLength = 4;
int compNotEnough[] = {
  255, 255, 150, 100
};
int compGood[] = {
  100, 10, 200, 75
};
int compTooMuch[] = {
  20, 10, 150, 230
};



void setup(void) {
  Serial.begin(500000);

  pinMode(VIBRATOR, OUTPUT);



  Serial.print("Raw\tAvg\tPeak\tSatiety");
  while (!Serial)
    delay(10);  // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Accelerometer global movement demo. v1.0");
  sensorInit();

  Serial.println("");
  delay(100);
}




void loop() {

  

  ms = millis();
  deltaTime = (ms - lastMs) / 1000.0;
  lastMs = ms;
 

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  vec3_t acc = { a.acceleration.x, a.acceleration.y, a.acceleration.z };
  float instantMove = 0;

  if (mustInit) {
    accPrev = acc;
    mustInit = false;
  } else {

    vec3_t vChange = acc - accPrev;
    instantMove = vChange.mag();

    // some kind of average value
    averageMovement += (instantMove - averageMovement) * 0.01;

    // Average peak
    averagePeak -= 0.01;
    averagePeak = max(averagePeak, averageMovement);

    // Movement increase satiety
    float satietyIncreaseSpeed = 50;
    satiety += instantMove * satietyIncreaseSpeed * deltaTime;

    // Upper limit of satietey
    satiety = min(2000, satiety);
    //satiety = max(0, satiety);

    // Time decrease satiety
    float satietyDecreaseSpeed = 20;  // unités / seconde
    satiety = max(0, satiety - satietyDecreaseSpeed * deltaTime);

    // TODO: Handle the satiety levels to trigger things

    accPrev = acc;
  }



  Serial.print("instantMove:");
  Serial.print(instantMove);
  Serial.print("\t");
  Serial.print("averageMovement:");
  Serial.print(averageMovement);
  Serial.print("\t");
  Serial.print("averagePeak:");
  Serial.print(averagePeak);
  Serial.print("\t");
  Serial.print("satiety:");
  Serial.print(satiety);

  
int index = int(millis()/1000)%compLength;

// Changement de vibrations + vibrations without delay et tableur
if (satiety < 750) {
  Serial.println(compNotEnough[index]);
  analogWrite(VIBRATOR,compNotEnough[index]);
  delay(100);

} else if (satiety < 1250) {
Serial.println(compGood[index]);
  analogWrite(VIBRATOR,compGood[index]);
  delay(100);
} else if (satiety < 2000) {
Serial.println(compTooMuch[index]);
  analogWrite(VIBRATOR,compTooMuch[index]);
  delay(100);
}


Serial.println();
}