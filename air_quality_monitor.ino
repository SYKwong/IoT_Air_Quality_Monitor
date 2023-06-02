#include <BlynkSimpleEsp32.h>

#define PRODUTION

const static uint8_t statusLEDPin = 32;
const static uint8_t redLEDPin = 33;
const static uint8_t greenLEDPin = 25;

const static uint8_t mq135Pin = 34;

static bool enabledAllLED = false;

const static char* BLYNK_AUTH_TOKEN = "tIW1UQph5va1fvtjtCzuFWd3sr5cm1qT";
const static char* ssid = "";
const static char* password = "";

static BlynkTimer timer;

static void turnOnStatusLED(void) {
  digitalWrite(statusLEDPin, HIGH);
}

static void turnOffStatusLED(void) {
  digitalWrite(statusLEDPin, LOW);
}

static void turnOnRedLED(void) {
  digitalWrite(redLEDPin, HIGH);
}

static void turnOffRedLED(void) {
  digitalWrite(redLEDPin, LOW);
}

static void turnOnGreenLED(void) {
  digitalWrite(greenLEDPin, HIGH);
}

static void turnOffGreenLED(void) {
  digitalWrite(greenLEDPin, LOW);
}

static bool isPpmOverCriticalLimit(float ppm) {
  const float ppm_critical_limit = 2000.0f;
  return (ppm >= ppm_critical_limit);
}

static bool isPpmBad(float ppm) {
  const float ppm_bad = 800.0f;
  return (ppm >= ppm_bad);
}

static void checkAirQuality(float ppm = -1.0) {
  static float _ppm = 0;

  if (ppm > 0) {
    _ppm = ppm;
  }

  if (enabledAllLED) {
    if (isPpmOverCriticalLimit(_ppm)) {
      turnOnRedLED();
      turnOnGreenLED();
    } else if (isPpmBad(_ppm)) {
      turnOnRedLED();
      turnOffGreenLED();
    } else {
      turnOffRedLED();
      turnOnGreenLED();
    }
  }
}
static void timerEvent(void) {
  static uint32_t count = 0;
  const float ppm = analogRead(mq135Pin) ;

  
  checkAirQuality(ppm);
  Blynk.virtualWrite(V0, (int)ppm);


  if(count<=1024){
    #ifndef PRODUTION
    Serial.print(ppm);
    Serial.println(", ");
    count++;

#endif
  }
}

static void two_minute_delay_to_allow_sensor_to_warm_up(void) {
  const uint32_t two_minute = 1000 * 60 * 2;
  delay(two_minute);
}

static bool is_connected_to_bylnk_server(void) {
  return Blynk.connected();
}

void setup(void) {

#ifndef PRODUTION
  Serial.begin(115200);
  Serial.println("Starting up");
#endif

  pinMode(statusLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);

  turnOnRedLED();
  turnOnGreenLED();
  turnOnStatusLED();

  const uint32_t one_second = 1000;

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timer.setInterval(one_second/10, timerEvent);

  turnOffRedLED();
  turnOffGreenLED();
  turnOffStatusLED();

  two_minute_delay_to_allow_sensor_to_warm_up();
}

BLYNK_WRITE(V1) {
  if (param.asInt()) {
    enabledAllLED = true;
    checkAirQuality();
  } else {
    enabledAllLED = false;
    turnOffRedLED();
    turnOffGreenLED();
    turnOffStatusLED();
  }
}

static void run_blynk(void) {
  if (is_connected_to_bylnk_server()) {
    Blynk.run();
    if (enabledAllLED == true) {
      turnOnStatusLED();
    }
  } else {
    turnOffStatusLED();
  }
}

void loop(void) {
  run_blynk();
  timer.run();
}
