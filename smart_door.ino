#include <Servo.h>

// Pin definitions
#define PIR_PIN 7
#define VIBRATION_SENSOR_PIN 3
#define BUZZER_PIN 4
#define LED_PIN 5
#define SERVO_PIN 6
#define BUTTON_PIN 2

// TTP224 setup
#define TTP224_1_PIN 8
#define TTP224_2_PIN 9
#define TTP224_3_PIN 10
#define TTP224_4_PIN 11
String pin = "";
int Sensor_State = 1;
int i = 0;
int vibration_counter = 0;

// Servo setup
Servo servo;

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(1);
  pinMode(PIR_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TTP224_1_PIN, INPUT);
  pinMode(TTP224_2_PIN, INPUT);
  pinMode(TTP224_3_PIN, INPUT);
  pinMode(TTP224_4_PIN, INPUT);
  servo.attach(SERVO_PIN);

  // Attach interrupt to button pin
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, FALLING);
}

void loop()
{
  if (digitalRead(PIR_PIN) == HIGH)
  {
    servo.write(90);
    // PIR sensor detects movement
    Serial.println("PIR sensor detects movement");
    delay(2000);
    if (Serial.available())
    {
      String str = Serial.readString();

      if (str == "DETECTED")
      {
        // Python detected a person
        Serial.println("Python detected a person");
        while (i < 10000)
        {
          Sensor_State = digitalRead(VIBRATION_SENSOR_PIN);
          vibration_counter += Sensor_State;
          i += 1;
          delay(1);
        }
        Serial.print("Vibration Counter: ");
        Serial.println(vibration_counter);
        if (vibration_counter < 2000)
        {
          // Vibration sensor does not detect vibration
          Serial.println("Vibration sensor does not detect vibration");
          delay(500);
          Serial.println("Input PIN:");
          while (pin.length() < 4)
          {
            if (digitalRead(TTP224_1_PIN) == 1)
            {
              pin += "1";
              Serial.println("User inputs: 1");
              delay(500); // Debounce delay
            }
            if (digitalRead(TTP224_2_PIN) == HIGH)
            {
              pin += "2";
              Serial.println("User inputs: 2");
              delay(500); // Debounce delay
            }
            if (digitalRead(TTP224_3_PIN) == HIGH)
            {
              pin += "3";
              Serial.println("User inputs: 3");
              delay(500); // Debounce delay
            }
            if (digitalRead(TTP224_4_PIN) == HIGH)
            {
              pin += "4";
              Serial.println("User inputs: 4");
              delay(500); // Debounce delay
            }
          }
          // User inputs PIN
          Serial.println("User inputs PIN: " + pin);
          if (pin == "1234")
          {
            // Correct PIN
            Serial.println("Correct PIN");
            servo.write(0);
            Serial.println("Door Unlocked");
            delay(10000);
            servo.write(90);
            Serial.println("Door Locked");
          }
          else
          {
            // Incorrect PIN
            Serial.println("Incorrect PIN");
            digitalWrite(BUZZER_PIN, HIGH);
            digitalWrite(LED_PIN, HIGH);
            servo.write(90);
          }
          pin = "";
        }
        else
        {
          Serial.println("Force Detected");
          digitalWrite(BUZZER_PIN, HIGH);
          digitalWrite(LED_PIN, HIGH);
          servo.write(90);
        }
        vibration_counter = 0;
        i = 0;
      }
      else
      {
        // Python did not detect a person
        Serial.println("Python did not detect a person");
      }
    }
  }
  else
  {
    // PIR sensor does not detect movement
    Serial.println("PIR sensor does not detect movement");
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
}

void buttonPressed()
{
  // Button is pressed
  Serial.println("Button is pressed, Alarm has been turned off~");
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}
