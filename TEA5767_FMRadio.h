/*##############################################################*/
/* Author: Marcus Nasarek                                       */
/* File:   TEA5767_FMRadio.h                                    */
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
#include <Arduino.h>
#include <Wire.h>

#ifndef TAE5767_FMRadio.h
#define TAE5767_FMRadio.h

#define TAE5767_ADDRESS (0x60)
#define NR_OF_DATABLOCKS 5

#define UP 1
#define DOWN 0
#define HIGH 1
#define LOW 0

#define CLOCK_FREQ_13MHZ 0
#define CLOCK_FREQ_327868HZ 1
#define CLOCK_FREQ_6_5MHZ 2

#define DTC_50US 0
#define DTC_75US 1

enum 
{
	// Data Byte #1
	MUTE 			= 0x80,	// set for mute 
	SM 				= 0x40,	// set for search mode on
	// Data Byte #3	
	SUD 			= 0x80,	// set for search mode up, unset for search mode down
	SSL_LOW 	= 0x20,	// -see table 11 for SSL--
	SSL_MID 	= 0x40,	// |                     |
	SSL_HIGH 	= 0x60, // -----------------------
	HLSI_HIGH = 0x10,	// Side Injection high side, unset for low side
	MS 				= 0x08, // set for Mono, unset for stereo
	MR 				= 0x04, // set for mute right channel
	ML 				= 0x02,	// set for mute left channel
	SWP1 			= 0x01, // set software programmable port 1 
	// Data Byte #4
	SWP2 			= 0x80,	// set for software programmable port 2
	STBY 			= 0x40,	// set for standby
	BL 				= 0x20, // set for Japanese Band limits, unset for EU band limits
	XTAL 			= 0x10, // set clock to 32.768 Hz - PLLREF must be unset
	SMUTE 		= 0x08,	// set for software mute
	HCC 			= 0x04, // set for high cur control
	SNC 			= 0x02,	// set for stereo noice cancel
	SI 				= 0x01, // search indicator and make SWP1 to ready flag SWP1 must be unset
	// Data Byte #5
	PLLREF 		= 0x80, // set for external 6.5 MHz clock, XTAL must be unset
	DTC 			= 0x40, // de-emphasis time constant to 75 us, unset for 50 us
};

class FMRadio
{
private:
	// I2C address of the NXP TAE5767 chip is 0x60
	uint8_t _i2c_address;
	uint8_t _fmTAE5767_config[5];
	uint8_t _fmTAE5767_info[5] = {0, 0, 0, 0, 0};

	void set_config_bit(bool b, uint8_t data_byte, uint8_t config_bit);
public:
	FMRadio(uint8_t i2c_address = 0x60);
	void init(void);
	// Get Config ////////////////////////////////////////////////////////
	// DEBUG
	uint8_t get_1st_byte(void);
	uint8_t get_2nd_byte(void);
	uint8_t get_3rd_byte(void);
	uint8_t get_4th_byte(void);
	uint8_t get_5th_byte(void);
	// END
	void send_data(void);
	void receive_data(void);
	uint8_t get_i2c_address(void);
	uint32_t get_frequency(void);
	float get_frequency_MHz(void);
	uint8_t get_config_bit(uint8_t data_byte, uint8_t config_bit);
	uint8_t get_mute(void);
	uint8_t get_search_mode(void);
	uint8_t get_search_dir(void);
	uint8_t get_search_stop_level(void);
	uint8_t get_side_injection(void);
	uint8_t get_stereo(void);
	uint8_t get_right_channel_mute(void);
	uint8_t get_left_channel_mute(void);
	uint8_t get_SW_programmable_port_1(void);
	uint8_t get_SW_programmable_port_2(void);
	uint8_t get_standby_setting(void);
	uint8_t get_bandlimit_setting(void);
	uint8_t get_clock_frequency(void);
	uint8_t get_SW_mute(void);
	uint8_t get_stereo_noice_cancel(void);
	uint8_t get_search_indicator(void);
	uint8_t deemphasis_time_const(void);
	// Set Config ////////////////////////////////////////////////////////
	void set_frequency_MHz(float frequency);
	void mute(bool on = true);
	void unmute(void);
	void set_search_mode(bool on = true);
	void set_search_dir(bool dir = UP);
	void set_search_stop_level(uint8_t ssl = SSL_LOW);
	void set_side_injection(bool hlsi = HIGH);
	void set_mono(void);
	void set_stereo(void);
	void set_right_channel_mute(void);
	void set_right_channel_unmute(void);
	void set_left_channel_mute(void);
	void set_left_channel_unmute(void);
	void set_SW_programmable_port_1(bool on=true);
	void set_SW_programmable_port_2(bool on=true);
	void standby(void);
	void wakeup(void);
	void set_japanese_band_limit(void);
	void set_EU_band_limit(void);
	void set_clock_frequency(uint8_t clock_freq = CLOCK_FREQ_327868HZ);
	void set_SW_mute(void);
	void set_SW_unmute(void);
	void set_stereo_noice_cancel(bool on=true);
	void set_search_indicator(bool on=true);
	void set_deemphasis_time_const(uint8_t time_constant = DTC_75US);
};

#endif