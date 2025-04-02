#include <Arduino.h>
#include <RP2040_PWM.h>
#include "AudioTools.h"

// mehr als 21ms keine daten -> mute

//FMT -> LOW für I2S, High für Left-Justified  beide MSB first
//XSMT -> soft mute/unmute über XSMT <20ns ramp damit das wirklich soft ist
//BCK Bit Clock                                     -> 64xfs = 12.288MHz
//SCK System Master Clock                           -> 192kHzx128 = 24.576MHz
//LRCK -> Low-> Left, High -> Right   abwechselnd   -> 192kHz direkt
//DEMP -> GND -> permanent
//FLT -> High  -> IO wäre gut



#define LRCK_PIN 3  // GPIO-Pin für den Taktausgang
#define SCK_PIN 4  // GPIO-Pin für den Taktausgang
#define BCK_PIN 5  // GPIO-Pin für den Taktausgang
#define Data_PIN 6  // GPIO-Pin für den Taktausgang

// #define CLOCK_FREQ 48152000  // 49.152 MHz für 384 kHz Abtastrate
#define LRCK_FREQ 192000/2  // 49.152 MHz für 384 kHz Abtastrate
#define SCK_FREQ 24576000/2  // 49.152 MHz für 384 kHz Abtastrate
#define BCK_FREQ 12288000/2  // 49.152 MHz für 384 kHz Abtastrate
// #define CLOCK_FREQ 55000000  // 49.152 MHz für 384 kHz Abtastrate
#define DUTY_CYCLE 50  // 49.152 MHz für 384 kHz Abtastrate

// RP2040_PWM* PWM_Instance;

RP2040_PWM* LRCK_PWM;
RP2040_PWM* SCK_PWM;
RP2040_PWM* BCK_PWM;

I2SStream i2sStream;

void setup() {
  Serial.begin(115200);

  LRCK_PWM = new RP2040_PWM(LRCK_PIN, LRCK_FREQ, DUTY_CYCLE, true);
  SCK_PWM = new RP2040_PWM(SCK_PIN, SCK_FREQ, DUTY_CYCLE, true);
  BCK_PWM = new RP2040_PWM(BCK_PIN, BCK_FREQ, DUTY_CYCLE, true);
  // PWM_Instance = new RP2040_PWM(CLOCK_PIN, CLOCK_FREQ, DUTY_CYCLE,true);
  
  // if (PWM_Instance) {
    // PWM_Instance->setPWM();
  // }

  if (LRCK_PWM) {
    LRCK_PWM->setPWM();
  }
  if (SCK_PWM) {
    SCK_PWM->setPWM();
  }
  if (BCK_PWM) {
    BCK_PWM->setPWM();
  }


  auto config = i2sStream.defaultConfig();
    config.sample_rate = 192000; // Sample-Rate
    config.bits_per_sample = 24; // Auflösung
    config.channels = 2; // Stereo
    config.pin_data = Data_PIN; // Daten-Pin (DIN)
    config.pin_bclk = BCK_PIN; // Bit Clock (BCK)
    config.pin_lrclk = LRCK_PIN; // Word Clock (LRCK)
    
    i2sStream.begin(config);
}

void loop() {
  int16_t sample = audioUSB.read(); // USB-Daten empfangen
  i2sStream.write(sample);          // Sample an PCM5102A senden
}