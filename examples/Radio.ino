#include <Wire.h>
#include <TEA5767_FMRadio.h>

FMRadio Radio = FMRadio();

void setup() {
  Serial.begin(9600);
  while(!Serial) {;}
  Wire.begin();
  Radio.init();
  Serial.print("I2C Addr: "); Serial.println(Radio.get_i2c_address(), HEX);
  Serial.print("1st Byte: "); Serial.println(Radio.get_1st_byte(), HEX);
  Serial.print("2nd Byte: "); Serial.println(Radio.get_2nd_byte(), HEX);
  Serial.print("3rd Byte: "); Serial.println(Radio.get_3rd_byte(), HEX);
  Serial.print("4th Byte: "); Serial.println(Radio.get_4th_byte(), HEX);
  Serial.print("5th Byte: "); Serial.println(Radio.get_5th_byte(), HEX);
}

void loop() {
  float freq;
  uint8_t injection_dir = HIGH;
  
  if(Serial.available()>0) {
    switch(Serial.read()) {
      case 'r': Radio.receive_data();
        Serial.print("freq(MHz): "); Serial.println(Radio.get_frequency_MHz(), DEC);
        break;
      case 'f':
        freq = 100.6;
        injection_dir = Radio.auto_injection_dir(freq);
        Radio.send_data();
        delay(100);
        Radio.receive_data();
        Serial.print("freq(MHz): "); Serial.println(Radio.get_frequency_MHz(), DEC);
        Serial.print("inj. side: "); Serial.println(injection_dir, DEC);
        break;
      case 'm':
        Serial.print("mute: "); Serial.println(Radio.get_mute()==MUTE);
        if(Radio.get_mute()){
          Radio.unmute();
        } else {
          Radio.mute();
        }
        Radio.send_data(); 
        break;
      case 's': Radio.receive_data();
        Serial.print("Stereo? ");
        if(Radio.get_stereo_reception() == STEREO) {
          Serial.println("ja");
        } else {
          Serial.println("nein");
        }
        break;
      case 'a': Radio.receive_data();
        Serial.print("ADC Level: "); Serial.println(Radio.get_level_ADC());
        break;
      case 'i': Radio.receive_data();
        Serial.print("IF Counter: "); Serial.println(Radio.get_if_counter(), HEX);
        break;
      case 'c': Radio.receive_data();
        Serial.print("Chip ID: "); Serial.println(Radio.get_chip_ID());
        break;
      case 'n': freq = Radio.seek(UP, SSL_LOW);
        if(freq == UPPER_BAND_LIMIT) {
          Serial.println("Band limit reached");
        } else {
          Serial.println("Station found"); 
        }
        Serial.print("freq(MHz): "); Serial.println(freq, DEC);
        break; 
      case 'p': freq = Radio.seek(DOWN, SSL_LOW);
        if(freq == UPPER_BAND_LIMIT) {
          Serial.println("Band limit reached");
        } else {
          Serial.println("Station found"); 
        }
        Serial.print("freq(MHz): "); Serial.println(freq, DEC);
        break;
      }
  } 
}
