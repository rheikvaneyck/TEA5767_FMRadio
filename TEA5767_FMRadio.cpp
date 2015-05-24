/*##############################################################*/
/* Author: Marcus Nasarek                                       */
/* File:   TEA5767_FMRadio.cpp                                  */
/* Compile with:                                                */
/*      Arduino 1.6.4                                           */
/* License:                                                     */
/*                                                              */
/*  This program is free software; you can redistribute it      */ 
/*  and/or modify it under the terms of the GNU General         */  
/*  Public License as published by the Free Software            */
/*  Foundation; either version 3 of the License, or             */
/*  (at your option) any later version.                         */
/*                                                              */
/*  This program is distributed in the hope that it will        */
/*  be useful, but WITHOUT ANY WARRANTY; without even the       */
/*  implied warranty of MERCHANTABILITY or                      */
/*  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General       */
/*  Public License for more details.                            */
/*                                                              */
/*  You should have received a copy of the GNU General          */
/*  Public License along with this program; if not,             */
/*  see <http://www.gnu.org/licenses/>.                         */
/*                                                              */
/*##############################################################*/
#include "TEA5767_FMRadio.h"

FMRadio::FMRadio(uint8_t i2c_address) 
{
	_i2c_address = i2c_address;
	// set frequency to 100 MHz
	// and unset MUTE and SM 
	_fmTAE5767_config[0] = (0x2DC9 >> 8); // High byte for PLL
	_fmTAE5767_config[1] = (0x2DC9 & 0xFF); // Low byte for PLL
	_fmTAE5767_config[2] = (SUD | SSL_LOW | HLSI_HIGH);
	_fmTAE5767_config[3] = (XTAL); // set clock to 32.768 Hz
	_fmTAE5767_config[4] = 0;
};

void FMRadio::init(void) {
	FMRadio::send_data();
	delay(100);
	FMRadio::receive_data();
}

void FMRadio::send_data(void) 
{
  Wire.beginTransmission(_i2c_address);
  Wire.write(_fmTAE5767_config,5);
  Wire.endTransmission();
}

void FMRadio::receive_data(void)
{
	uint8_t nr = NR_OF_DATABLOCKS;
	Wire.requestFrom(_i2c_address, nr);
	if(Wire.available()) {
		for (int i=0;i<NR_OF_DATABLOCKS;i++) {
			_fmTAE5767_info[i] = Wire.read();
		}
	}
};

/* Get Config **************************************************/
uint8_t FMRadio::get_i2c_address(void) {
	return _i2c_address;
}

uint8_t FMRadio::get_config_bit(uint8_t data_byte, uint8_t config_bit)
{
	return (_fmTAE5767_config[data_byte]&config_bit);
}

void FMRadio::set_config_bit(bool on, uint8_t data_byte, uint8_t config_bit)
{
	if(on) {
		_fmTAE5767_config[data_byte] |= config_bit; 		
	} else {
		_fmTAE5767_config[data_byte] &= ~config_bit; 
	}
};

/* Get Info ***************************************************/
uint8_t FMRadio::get_info_bit(uint8_t data_byte, uint8_t config_bit)
{
	return (_fmTAE5767_info[data_byte]&config_bit);
};

uint8_t FMRadio::get_ready_flag(void){return FMRadio::get_info_bit(0, RF);};
uint8_t FMRadio::get_band_limit_flag(void){return FMRadio::get_info_bit(0, BLF);};
uint32_t FMRadio::get_frequency(void) {
	int8_t hlsi_factor = 1;
	uint32_t frequency;
	// calculating PLL word
	// done mathematics
	// N = 4*(f_RF + f_IF)/f_ref on page 30 of datasheet
	if (!(_fmTAE5767_config[2] & HLSI_HIGH)) {
		hlsi_factor = -1;
	}
	frequency = ((_fmTAE5767_info[0]&0x3F)<<8) + _fmTAE5767_info[1];
	if(_fmTAE5767_config[4] & PLLREF) {
		// PLLREF is set
		frequency = ((frequency * 50000)>>2) - hlsi_factor*225000;
	} else { 
		// PLLREF is not set, XTAL should be
		frequency = (frequency << 13) - hlsi_factor*225000; 
	}
	return frequency;	
};
float FMRadio::get_frequency_MHz(void){return ((float) FMRadio::get_frequency())/1000000.0;};
uint8_t FMRadio::get_stereo_reception(void){return FMRadio::get_info_bit(2, STEREO);};
uint8_t FMRadio::get_if_counter(void){return (_fmTAE5767_info[2]&0x7F);};
uint8_t FMRadio::get_level_ADC(void) {return (_fmTAE5767_info[3]>>4);};
uint8_t FMRadio::get_chip_ID(void){return ((_fmTAE5767_info[3]&0xE)>>1);};

/* DEBUG ******************/


uint8_t FMRadio::get_1st_byte(void) {
	return 	_fmTAE5767_config[0];
}
uint8_t FMRadio::get_2nd_byte(void) {
	return 	_fmTAE5767_config[1];
}
uint8_t FMRadio::get_3rd_byte(void) {
	return 	_fmTAE5767_config[2];
}
uint8_t FMRadio::get_4th_byte(void) {
	return 	_fmTAE5767_config[3];
}
uint8_t FMRadio::get_5th_byte(void) {
	return 	_fmTAE5767_config[4];
};

/* END ********************/

/* Verbose Functions ******/
uint32_t FMRadio::get_config_frequency(void) {
	int8_t hlsi_factor = 1;
	uint32_t frequency;
	// calculating PLL word
	// done mathematics
	// N = 4*(f_RF + f_IF)/f_ref on page 30 of datasheet
	if (!(_fmTAE5767_config[2] & HLSI_HIGH)) {
		hlsi_factor = -1;
	}
	frequency = ((_fmTAE5767_config[0]&0x3F)<<8) + _fmTAE5767_config[1];
	if(_fmTAE5767_config[4] & PLLREF) {
		// PLLREF is set
		frequency = ((frequency * 50000)>>2) - hlsi_factor*225000;
	} else { 
		// PLLREF is not set, XTAL should be
		frequency = (frequency << 13) - hlsi_factor*225000; 
	}
	return frequency;	
};

float FMRadio::get_config_frequency_MHz(void) 
{
	return ((float) FMRadio::get_config_frequency())/1000000.0;
};

uint8_t FMRadio::get_mute(void){return FMRadio::get_config_bit(0,MUTE);};
uint8_t FMRadio::get_search_mode(void){return FMRadio::get_config_bit(0,SM);};
uint8_t FMRadio::get_search_dir(void){return FMRadio::get_config_bit(2,SUD);};
uint8_t FMRadio::get_search_stop_level(void){return FMRadio::get_config_bit(1,SSL_HIGH);};
uint8_t FMRadio::get_side_injection(void){return FMRadio::get_config_bit(2,HLSI_HIGH);};
uint8_t FMRadio::get_stereo(void){return FMRadio::get_config_bit(2,MS);};
uint8_t FMRadio::get_right_channel_mute(void){return FMRadio::get_config_bit(2,MR);};
uint8_t FMRadio::get_left_channel_mute(void){return FMRadio::get_config_bit(2,ML);};
uint8_t FMRadio::get_SW_programmable_port_1(void){return FMRadio::get_config_bit(2,SWP1);};
uint8_t FMRadio::get_SW_programmable_port_2(void){return FMRadio::get_config_bit(3,SWP2);};
uint8_t FMRadio::get_standby_setting(void){return FMRadio::get_config_bit(3,STBY);};
uint8_t FMRadio::get_bandlimit_setting(void){return FMRadio::get_config_bit(3,BL);};
uint8_t FMRadio::get_clock_frequency(void)
{
	if(FMRadio::get_config_bit(3,XTAL)) {
		return CLOCK_FREQ_327868HZ;
	} else if(FMRadio::get_config_bit(4,PLLREF)) {
		 return CLOCK_FREQ_6_5MHZ;	
	} else {
		return CLOCK_FREQ_13MHZ;
	}
};
uint8_t FMRadio::get_SW_mute(void){return FMRadio::get_config_bit(3,SMUTE);};
uint8_t FMRadio::get_stereo_noice_cancel(void){return FMRadio::get_config_bit(3,SNC);};
uint8_t FMRadio::get_search_indicator(void){return FMRadio::get_config_bit(3,SI);};
uint8_t FMRadio::deemphasis_time_const(void){return FMRadio::get_config_bit(4,DTC);};
/* End Verbose Functions ******/

// Set Config ///////////////////////////////////////////////////////

void FMRadio::set_frequency_MHz(float frequency)
{
	int8_t hlsi_factor = 1;
	uint32_t frequencyB;
	// calculating PLL word
	// done mathematics
	// N = 4*(f_RF + f_IF)/f_ref on page 30 of datasheet
	if (!(_fmTAE5767_config[2] & HLSI_HIGH)) {
		hlsi_factor = -1;
	}
	if(_fmTAE5767_config[4] & PLLREF) {
		// PLLREF is set
		frequencyB=((uint32_t) (frequency*80+hlsi_factor*18));
	} else { 
		// PLLREF is not set, XTAL should be
		frequencyB=((uint32_t) (frequency*122.07 + hlsi_factor*24.47)); 
	}
	// set high byte by masking out MUTE and SM
  _fmTAE5767_config[0] = (((frequencyB>>8)&0x3F) | (_fmTAE5767_config[0] & 0xC0));
  _fmTAE5767_config[1] = frequencyB&0xFF;
};

void FMRadio::mute(bool on)
{
	FMRadio::set_config_bit(on, 0, MUTE);
};

void FMRadio::unmute()
{
	FMRadio::mute(false);
};

void FMRadio::set_search_mode(bool on)
{
	FMRadio::set_config_bit(on, 0, SM);
};

void FMRadio::set_search_dir(bool dir)
{
	FMRadio::set_config_bit(dir, 2, SUD);
};

void FMRadio::set_search_stop_level(uint8_t ssl)
{
	switch(ssl) {
		case SSL_MID: break;
		case SSL_HIGH: break;
		default: ssl = SSL_LOW; break;
	}
	_fmTAE5767_config[2] |= ssl;
};

void FMRadio::set_SW_mute(void)
{
	FMRadio::set_config_bit(true, 3, SMUTE);
};

void FMRadio::set_SW_unmute(void)
{
	FMRadio::set_config_bit(false, 3, SMUTE);
};

void FMRadio::set_stereo_noice_cancel(bool on)
{
	FMRadio::set_config_bit(on, 3, SNC);
};

void FMRadio::set_side_injection(bool hlsi)
{
	FMRadio::set_config_bit(hlsi, 2, MUTE);
};

void FMRadio::set_mono(void)
{
	FMRadio::set_config_bit(true, 2, MS);
};

void FMRadio::set_stereo(void)
{
	FMRadio::set_config_bit(false, 2, MS);
};

void FMRadio::set_right_channel_mute(void)
{
	FMRadio::set_config_bit(true, 2, MR);
};

void FMRadio::set_right_channel_unmute(void)
{
	FMRadio::set_config_bit(false, 2, MR);
};

void FMRadio::set_left_channel_mute(void)
{
	FMRadio::set_config_bit(true, 2, ML);
};

void FMRadio::set_left_channel_unmute(void)
{
	FMRadio::set_config_bit(false, 2, ML);
};

void FMRadio::set_SW_programmable_port_1(bool on)
{
	FMRadio::set_config_bit(on, 2, SWP1);
};

void FMRadio::set_SW_programmable_port_2(bool on)
{
	FMRadio::set_config_bit(on, 3, SWP2);
};

void FMRadio::standby(void)
{
	FMRadio::set_config_bit(true, 3, STBY);
};	

void FMRadio::wakeup(void)
{
	FMRadio::set_config_bit(false, 3, STBY);
};	

void FMRadio::set_japanese_band_limit(void)
{
	FMRadio::set_config_bit(true, 3, BL);
};

void FMRadio::set_EU_band_limit(void)
{
	FMRadio::set_config_bit(false, 3, BL);
};

void FMRadio::set_clock_frequency(uint8_t clock_freq)
{
	switch(clock_freq) {
		case CLOCK_FREQ_13MHZ: 
			_fmTAE5767_config[3] &= ~XTAL;
			_fmTAE5767_config[4] &= ~PLLREF;
			break;
		case CLOCK_FREQ_327868HZ:
			_fmTAE5767_config[3] |= XTAL;
			_fmTAE5767_config[4] &= ~PLLREF;
			break;
		case CLOCK_FREQ_6_5MHZ:
			_fmTAE5767_config[3] &= ~XTAL;
			_fmTAE5767_config[4] |= PLLREF;
			break;
		}
};

void FMRadio::set_high_cut_control(bool on){FMRadio::set_config_bit(on, 3, HCC);};

void FMRadio::set_search_indicator(bool on)
{
	// FIXME: Test for an other bit necassary
	FMRadio::set_config_bit(on, 3, SI);
};

void FMRadio::set_deemphasis_time_const(uint8_t time_constant)
{
	switch(time_constant) {
		case DTC_75US:
			FMRadio::set_config_bit(true, 4, DTC);
			break;
		case DTC_50US:
			FMRadio::set_config_bit(false, 4, DTC);
			break;
	}
};

uint8_t FMRadio::auto_injection_dir(float frequency_MHz)
{
	// Thanks to andy karpow
	uint8_t adc_high = 0;
	uint8_t adc_low = 0;
	uint8_t is = HIGH;

	FMRadio::mute();
	FMRadio::set_frequency_MHz(frequency_MHz + 0.45);
	FMRadio::send_data();
	delay(30);
	FMRadio::receive_data();
	delay(30);
	adc_high = FMRadio::get_level_ADC();

	FMRadio::set_frequency_MHz(frequency_MHz - 0.45);
	FMRadio::send_data();
	delay(30);
	FMRadio::receive_data();
	delay(30);
	adc_low = FMRadio::get_level_ADC();

	if(adc_high < adc_low) {
		is = HIGH;
	} else {
		is = LOW;
	}
	FMRadio::set_side_injection(is);
	FMRadio::set_frequency_MHz(frequency_MHz);
	FMRadio::unmute();
	delay(30);
	return is;
}
;

float FMRadio::seek(bool dir, uint8_t ssl)
{
	int8_t factor = -1 + (dir*2);

	FMRadio::receive_data();
	delay(30);
	FMRadio::set_frequency_MHz(FMRadio::get_frequency_MHz()+(factor*0.1)); // +0.1 if dir=1, -0.1 if dir=0;
  FMRadio::set_search_mode(ON);
  FMRadio::set_search_dir(dir);
  FMRadio::set_search_stop_level(ssl);
  FMRadio::set_search_indicator(OFF);
  FMRadio::send_data();
  delay(100);
  FMRadio::receive_data();
  delay(30);

  if(FMRadio::get_band_limit_flag()) {
  	if(dir == UP) {
  		return UPPER_BAND_LIMIT;
  	} else {
  		return LOWER_BAND_LIMIT;
  	}
  } else {
    if(FMRadio::get_ready_flag()) {
      return FMRadio::get_frequency_MHz();
    } else {
    	// FIXME shouldn't stop here, if RF not set
    	return FMRadio::get_frequency_MHz();
    }  	
  }
}
;