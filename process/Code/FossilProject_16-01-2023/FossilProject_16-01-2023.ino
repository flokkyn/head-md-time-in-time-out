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

//Permet de lire les valeurs en vibrations
const int compLength = 4;
int compNotEnough[compLength] = {
  255, 75, 150, 100
};
int compGood[compLength] = {
  100, 10, 200, 75
};
int compTooMuch[compLength] = {
  20, 10, 150, 230
};



void setup(void) {
  Serial.begin(500000);

  pinMode(VIBRATOR, OUTPUT);



  Serial.println("Raw\tAvg\tPeak\tSatiety");
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

  Serial.print("R ");
  sensors_event_t a, g, temp;
  bool success = mpu.getEvent(&a, &g, &temp);
  Serial.print("success:");
  Serial.print(success);
  Serial.print("\t");

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
    float satietyDecreaseSpeed = 30;  // unités / seconde
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


  int index = int(millis() / 1000) % compLength;
  Serial.print("\t");
  Serial.print(String("index:") + index);

  // Changement de vibrations + vibrations without delay et tableur
  if (satiety < 750) {
    Serial.print(String(" A:") + compNotEnough[index]);
    analogWrite(VIBRATOR, compNotEnough[index]);

  } else if (satiety < 1250) {
    Serial.print(String(" B:") + compGood[index]);
    analogWrite(VIBRATOR, compGood[index]);

  } else {
    Serial.print(String(" C:") + compTooMuch[index]);
    analogWrite(VIBRATOR, compTooMuch[index]);
  }


  Serial.println();
  delay(100);
}