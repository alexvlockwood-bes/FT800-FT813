/*
@file    EVE_config.h
@brief   configuration information for some TFTs and some pre-defined colors
@version 4.0
@date    2019-04-16, original 2019-04-03
@author  Rudolph Riedel
@author  Alexis Lockwood <alex@boulderes.com>

This code comes from: https://github.com/RudolphRiedel/FT800-FT813

License:

    MIT License

    Copyright (c) 2017 

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

@section History

2.2
- added prototypes for hardware abstraction funtions
- added hardware-selection defines, first implemented for AVR8
- added EVE_VM800B50A TFT setup as copy of EVE_VM800B43A since FTDI uses the same setup for both

2.3
- moved pin definitions for EVE_CS and FR_PDN to here from EVE_config.c
- added a set of definitions for Arduino

2.4
- switched from custom AVR8 to standard __AVR__ symbol to automatically generate plattform specific code
- removed the definition of the ARDUINO symbol at it already is defined by the Arduino enviroment

2.5
- added support for RH850
- switched to standard-C compliant comment-style
- added EVE_HAS_CRYSTAL to make EVE_init() automatically use the correct option

3.0
- renamed from FT800_config.h to EVE_config.h
- changed FT_ prefixes to EVE_
- changed ft800_ prefixes to EVE_

3.1
- added "#if defined (__ESP8266__)" for the Arduino side, now empty

3.2
- added config for FT811CB_HY50HD

3.3
- switched to inline definitions for all those one-line support-functions -> faster and EVE_config.c is no longer needed
- added timing parameters for Matrix Orbital EVE2 displays, derived from their sample code, untested
- added EVE_CSPREAD values to all display settings

3.4
- forgot that I switched to SPI.write() for Arduino/ESP8266 and that the standard Arduino only has SPI.transfer()
- added auto-detection for AVR over 64kB FLASH

3.5
- switched from "#if defined (RAMPZ)" to "#if defined (__AVR_HAVE_ELPM__)" as it turned out there are AVR that have the RAMPZ register but less than 64k FLASH

3.6
- changed the EVE_HAS_CRYSTAL for the Matrix Orbital modules from '1' to '0' as it turned out these are not using a crystal

3.7
- added timing parameters for Newhaven EVE2 displays, derived from their datasheets, untested

3.8
- added timing parameters for Glyn ADAM101-LCP-SWVGA-NEW 10.1" TFT with 1024x600
- fixed a copy-paste error with my latest changes to the Arduino code that killed the SPI

3.9
- changed the timing parameters for EVE2-38A to match those listed in the 1.3 manual from Matrix Orbital
- added a EVE_EVE2_35G profile with a new define: EVE_HAS_GT911
- added more explanatory text before the code-block
- added more EVE_EV2_xxG profiles as the other cap-touch modules from Matrix Orbital also have a GT911 as touch-controller

3.10
- added timings for FTDO/BRT modules VM810C, ME812A and ME813A, untested
- changed the way EVE_HAS_CRYSTAL works in EVE_commands.c -> removed from the profiles of displays that have no crystal
- moved HSIZE and VSIZE to the top of the parameter list
- merged some settings with identical parameters like EVE_EVE2_50G end EVE_EVE2_70G
- changed a few timing parameters for the Matrix Orbital EVE2 modules to match the values in the EVE_2_Module_Manual_Rev_1.4.pdf
- modified the timings for the EVE2-38 and EVE2-38G modules to match these in the Matrix Orbital EVE2-Library plus increased EVE_HCYCLE to 548

3.11
- changed EVE_RVT70AQ to EVE_RVT70 as the timing parameters apply to all 7" EVE modules from Riverdi
- added EVE_RVT50 timings, not sure if correct, Riverdi uses the same timings for their RVT50 modules as for their RVT70 modules but their datasheets show different timings - took the timings from the datasheets
- added EVE_RVT43 timings, following the sample code from Riverdi instead of the datasheets since it matches other 480x272 panels
- added EVE_RVT35 timings, following the sample code from Riverdi, the datasheets have not enough timing information to work with
- added EVE_RVT28 timings, following the sample code from Riverdi, the datasheets mostly appear to be the same
- changed the way FT81X_ENABLE works by adding it to the configs of the modules with FT81x for automatic selection
- added the lines for a TRICORE target, probably useless by itself, much like the RH80 target, but may still serve as an example for other targets
- removed the color settings

3.12
- added spi_transmit_async() functions, for now identical to spi_transmit()
- added software-spi functionality
- changed #ifdef to #if defined for consistency
- added a block for the SAMC21E18A, bare-metall but using software-spi for now

4.0
- renamed from EVE_config.h to EVE_config.h
- renamed EVE_81X_ENABLE to FT81X_ENABLE
- added a fictitious BT81x entry under the made-up name EVE_EVE3_70G, just to see the project compile with additional BT81x includes and functions
- still 4.0 for EVE itself, switched to hardware-SPI on SAMC21
- minor maintenance
- added DMA to SAMC21 branch
- started testing things with a BT816
- added a block for the SAME51J18A
- added profiles for the BT81x 4.3", 5" and 7" modules from Riverdi - the only tested is the 4.3" with a RVT43ULBNWC00

--------------------------------------------------------------------------------
Modified by Alexis Lockwood at Boulder Engineering Studio <alex@boulderes.com>

4.0-bes
- Add support for multiple display drivers and displays
- Add timeouts to the while-busy loops

*/

#ifndef EVE_CONFIG_H_
#define EVE_CONFIG_H_


/* select the settings for the TFT attached */
#if 0
	#define EVE_VM800B35A
	#define EVE_VM800B43A
	#define EVE_VM800B50A
	#define EVE_VM810C
	#define EVE_ME812A
	#define EVE_ME813A
	#define EVE_FT810CB_HY50HD
	#define EVE_FT811CB_HY50HD
	#define EVE_ET07
	#define EVE_RVT28
	#define EVE_RVT35
	#define EVE_RVT43
	#define EVE_RVT50
	#define EVE_RVT70
	#define EVE_RiTFT43
	#define EVE_RiTFT50
	#define EVE_RiTFT70
	#define EVE_EVE2_29
	#define EVE_EVE2_35
	#define EVE_EVE2_35G
	#define EVE_EVE2_38
	#define EVE_EVE2_38G
	#define EVE_EVE2_43
	#define EVE_EVE2_43G
	#define EVE_EVE2_50
	#define EVE_EVE2_50G
	#define EVE_EVE2_70
	#define EVE_EVE2_70G
	#define EVE_NHD_35
	#define EVE_NHD_43
	#define EVE_NHD_50
	#define EVE_NHD_70
	#define EVE_ADAM101
	#define EVE3_43
#endif

#define eve_ritft43


/* display timing parameters below */


/* test-entry with a made-up name, I have no knowledge if such a named product is even planned, just feels logical */
#if defined (EVE_EVE3_70G)
#define EVE_EVE2_70G	/* trigger including the setup for the EVE2_70G, assuming the same panel is used and that FT81x is a subset of BT81x */
#define BT81X_ENABLE
#endif

/* just annother test setup */
#if defined (EVE3_43)
#define EVE_EVE2_43
#define EVE_HAS_CRYSTAL
#define BT81X_ENABLE
#endif


#if defined (EVE_RiTFT43)
#define EVE_RVT43
#define EVE_HAS_CRYSTAL
#define FT81X_ENABLE
#define BT81X_ENABLE
#endif

#if defined (EVE_RiTFT50)
#define EVE_RVT50
#define EVE_HAS_CRYSTAL
#define BT81X_ENABLE
#endif

#if defined (EVE_RiTFT70)
#define EVE_RVT70
#define EVE_HAS_CRYSTAL
#define BT81X_ENABLE
#endif


// SAMPLE CONFIGURATIONS-------------------------------------------------------

#define EVE_800x480x_CONFIG (eve_display_t){ \
    .hsize = 800, .vsize = 400, \
    .vsync0 = 0, .vsync1 = 10, .voffset = 35, .vcycle = 516, \
    .hsync0 = 0, .hsync1 = 88, .hoffset = 169, .hcycle = 969, \
    .pclkpol_falling = true, .swizzle = 0, .pclk = 2, .cspread = 1, \
    .touch_rzthresh = 1200, .has_crystal = true, \
}

/* VM800B35A: FT800 320x240 3.5" FTDI FT800 */
#define EVE_VM800B35A_CONFIG (eve_display_t){ \
    .hsize = 320, .vsize = 240, \
    .vsync0 = 0, .vsync1 = 2, .voffset = 13, .vcycle = 263, \
    .hsync0 = 0, .hsync1 = 10, .hoffset = 70, .hcycle = 408, \
    .pclkpol_falling = false, .swizzle = 2, .pclk = 8, .cspread = 1, \
    .touch_rzthresh = 1200, .has_crystal = true, \
}



/* FTDI/BRT EVE modules VM800B43A and VM800B50A  FT800 480x272 4.3" and 5.0" */
#define EVE_VM800B43A_CONFIG EVE_VM800B50A_CONFIG
#define EVE_VM800B50A_CONFIG (eve_display_t){ \
    .hsize = 480, .vsize = 272, \
    .vsync0 = 0, .vsync1 = 10, .voffset = 12, .vcycle = 292, \
    .hsync0 = 0, .hsync1 = 41, .hoffset = 43, .hcycle = 548, \
    .pclkpol_falling = true, .swizzle = 0, .pclk = 5, .cspread = 1, \
    .touch_rsthresh = 1200, .has_crystal = true, \
}



/* untested */
/* FTDI/BRT EVE2 modules VM810C50A-D, ME812A-WH50R and ME813A-WH50C, 800x480 5.0" */
#define EVE_VM810C_CONFIG EVE_ME813A_CONFIG
#define EVE_ME812A_CONFIG EVE_ME813A_CONFIG
#define EVE_ME813A_CONFIG (eve_display_t){ \
    .hsize = 800, .vsize = 480, \
    .vsync0 = 0, .vsync1 = 3, .voffset = 32, .vcycle = 525, \
    .hsync0 = 0, .hsync1 = 48, .hoffset = 88, .hcycle = 928, \
    .pclkpol_falling = true, .swizzle = 0, .pclk = 2, .cspread = 0, \
    .touch_rzthresh = 1200, .has_crystal = true, \
}


/* FT810CB-HY50HD: FT810 800x480 5.0" HAOYU */
#define EVE_FT810CB_HY50D_CONFIG (eve_display_t){ \
    .hsize = 800, .vsize = 480, \
    .vsync0 = 0, .vsync1 = 2, .voffset = 13, .vcycle = 525, \
    .hsync0 = 0, .hsync1 = 20, .hoffset = 64, .hcycle = 952, \
    .pclkpol_falling = true, .swizzle = 0, .pclk = 2, .cspread = 1, \
    .touch_rzthresh = 2000, .has_crystal = true, \
}


/* FT811CB-HY50HD: FT811 800x480 5.0" HAOYU */
#define EVE_FT811CB_HY50D_CONFIG (eve_display_t) { \
    .hsize = 800, .vsize = 480, \
    .vsync0 = 0, .vsync1 = 2, .voffset = 13, .vcycle = 525, \
    .hsync0 = 0, .hsync1 = 20, .hoffset = 64, .hcycle = 952, \
    .pclkpol_falling = true, .swizzle = 0, .pclk = 2, .cspread = 1, \
    .touch_rzthresh = 1200, .has_crystal = true, \
}

// TODO convert the rest

/* untested */
/* G-ET0700G0DM6 800x480 7.0" Glyn */
#if defined (EVE_ET07)
#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(35L)
#define EVE_VCYCLE	(525L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(128L)
#define EVE_HOFFSET (203L)
#define EVE_HCYCLE 	(1056L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* untested */
/* RVT28 240x320 2.8" Riverdi, various options, FT800/FT801 */
#if defined (EVE_RVT28)
#define EVE_HSIZE	(320L)
#define EVE_VSIZE	(240L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(2L)
#define EVE_VCYCLE	(326L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(10L)
#define EVE_HOFFSET	(20L)
#define EVE_HCYCLE 	(270L)
#define EVE_PCLKPOL	(0L)
#define EVE_SWIZZLE	(4L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#endif


/* untested */
/* RVT3.5 320x240 3.5" Riverdi, various options, FT800/FT801 */
#if defined (EVE_RVT35)
#define EVE_HSIZE	(320L)
#define EVE_VSIZE	(240L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(13L)
#define EVE_VCYCLE	(263L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(10L)
#define EVE_HOFFSET	(70L)
#define EVE_HCYCLE 	(408L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(2L)
#define EVE_PCLK	(6L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#endif


/* untested */
/* RVT43 / RVT4.3 480x272 4.3" Riverdi, various options, FT800/FT801 */
#if defined (EVE_RVT43)
#define EVE_HSIZE	(480L)
#define EVE_VSIZE	(272L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(10L)
#define EVE_VOFFSET	(12L)
#define EVE_VCYCLE	(292L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(41L)
#define EVE_HOFFSET	(43L)
#define EVE_HCYCLE 	(548L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#endif


/* untested */
/* RVT50xQFxxxxx 800x480 5.0" Riverdi, various options, FT812/FT813 */
#if defined (EVE_RVT50)
#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(3L)
#define EVE_VOFFSET	(32L)
#define EVE_VCYCLE	(525L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(48L)
#define EVE_HOFFSET (88L)
#define EVE_HCYCLE 	(928L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* RVT70xQFxxxxx 800x480 7.0" Riverdi, various options, FT812/FT813, tested with RVT70UQFNWC0x */
#if defined (EVE_RVT70)
#define EVE_HSIZE	(800L)	/* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE	(480L)	/* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0	(0L)	/* Tvf Vertical Front Porch */
#define EVE_VSYNC1	(10L)	/* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET	(23L)	/* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE	(525L)	/* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0	(0L)	/* Thf Horizontal Front Porch */
#define EVE_HSYNC1	(10L)	/* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (46L)	/* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE 	(1056L)	/* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLKPOL (1L)	/* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE (0L)	/* Defines the arrangement of the RGB pins of the FT800 */
#define EVE_PCLK	(2L)	/* 60MHz / REG_PCLK = PCLK frequency 30 MHz */
#define EVE_CSPREAD	(1L)	/* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_TOUCH_RZTHRESH (1800L)	/* touch-sensitivity */
#define FT81X_ENABLE
#endif


/* untested */
/* EVE2-29A 320x102 2.9" 1U Matrix Orbital, non-touch, FT812 */
#if defined (EVE_EVE2_29)
#define EVE_HSIZE	(320L)
#define EVE_VSIZE	(102L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(156L)
#define EVE_VCYCLE	(262L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(10L)
#define EVE_HOFFSET	(70L)
#define EVE_HCYCLE 	(408L)
#define EVE_PCLKPOL	(0L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(8L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* EVE2-35A 320x240 3.5" Matrix Orbital, resistive, or non-touch, FT812 */
#if defined (EVE_EVE2_35)
#define EVE_HSIZE	(320L)
#define EVE_VSIZE	(240L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(18L)
#define EVE_VCYCLE	(262L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(10L)
#define EVE_HOFFSET	(70L)
#define EVE_HCYCLE 	(408L)
#define EVE_PCLKPOL	(0L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(8L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* EVE2-35G 320x240 3.5" Matrix Orbital, capacitive touch, FT813 */
#if defined (EVE_EVE2_35G)
#define EVE_HSIZE	(320L)
#define EVE_VSIZE	(240L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(18L)
#define EVE_VCYCLE	(262L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(10L)
#define EVE_HOFFSET	(70L)
#define EVE_HCYCLE 	(408L)
#define EVE_PCLKPOL	(0L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(8L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_GT911	/* special treatment required for out-of-spec touch-controller */
#define FT81X_ENABLE
#endif


/* EVE2-38A 480x116 3.8" 1U Matrix Orbital, resistive touch, FT812 */
#if defined (EVE_EVE2_38)
#define EVE_HSIZE	(480L)
#define EVE_VSIZE	(272L)

#define EVE_VSYNC0	(152L)
#define EVE_VSYNC1	(10L)
#define EVE_VOFFSET	(12L)
#define EVE_VCYCLE	(292L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(41L)
#define EVE_HOFFSET	(43L)
#define EVE_HCYCLE 	(548L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* EVE2-38G 480x116 3.8" 1U Matrix Orbital, capacitive touch, FT813 */
#if defined (EVE_EVE2_38G)
#define EVE_HSIZE	(480L)
#define EVE_VSIZE	(272L)

#define EVE_VSYNC0	(152L)
#define EVE_VSYNC1	(10L)
#define EVE_VOFFSET	(12L)
#define EVE_VCYCLE	(292L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(41L)
#define EVE_HOFFSET	(43L)
#define EVE_HCYCLE 	(548L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_GT911	/* special treatment required for out-of-spec touch-controller */
#define FT81X_ENABLE
#endif


/* untested */
/* EVE2-43A 480x272 4.3" Matrix Orbital, resistive or no touch, FT812 */
#if defined (EVE_EVE2_43)
#define EVE_HSIZE	(480L)
#define EVE_VSIZE	(272L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(10L)
#define EVE_VOFFSET	(12L)
#define EVE_VCYCLE	(292L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(41L)
#define EVE_HOFFSET	(43L)
#define EVE_HCYCLE 	(548L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* EVE2-43G 480x272 4.3" Matrix Orbital, capacitive touch, FT813 */
#if defined (EVE_EVE2_43G)
#define EVE_HSIZE	(480L)
#define EVE_VSIZE	(272L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(10L)
#define EVE_VOFFSET	(12L)
#define EVE_VCYCLE	(292L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(41L)
#define EVE_HOFFSET	(43L)
#define EVE_HCYCLE 	(548L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_GT911	/* special treatment required for out-of-spec touch-controller */
#define FT81X_ENABLE
#endif


/* untested */
/* Matrix Orbital EVE2 modules EVE2-50A, EVE2-70A : 800x480 5.0" and 7.0" resistive, or no touch, FT812 */
#if defined (EVE_EVE2_50) || defined (EVE_EVE2_70)
#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(3L)
#define EVE_VOFFSET	(32L)
#define EVE_VCYCLE	(525L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(48L)
#define EVE_HOFFSET	(88L)
#define EVE_HCYCLE 	(928L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(0L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define FT81X_ENABLE
#endif


/* Matrix Orbital EVE2 modules EVE2-50G, EVE2-70G : 800x480 5.0" and 7.0" capacitive touch, FT813 */
#if defined (EVE_EVE2_50G) || defined (EVE_EVE2_70G)
#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(3L)
#define EVE_VOFFSET	(32L)
#define EVE_VCYCLE	(525L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(48L)
#define EVE_HOFFSET	(88L)
#define EVE_HCYCLE 	(928L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(0L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_GT911	/* special treatment required for out-of-spec touch-controller */
#define FT81X_ENABLE
#endif


/* NHD-3.5-320240FT-CxXx-xxx 320x240 3.5" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_35)
#define EVE_HSIZE	(320L)
#define EVE_VSIZE	(240L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(2L)
#define EVE_VOFFSET	(13L)
#define EVE_VCYCLE	(263L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(10L)
#define EVE_HOFFSET	(70L)
#define EVE_HCYCLE 	(408L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(2L)
#define EVE_PCLK	(6L)
#define EVE_CSPREAD	(0L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_CRYSTAL
#define FT81X_ENABLE
#endif


/* untested */
/* NHD-4.3-480272FT-CxXx-xxx 480x272 4.3" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_43)
#define EVE_HSIZE	(480L)
#define EVE_VSIZE	(272L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(10L)
#define EVE_VOFFSET	(12L)
#define EVE_VCYCLE	(292L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(41L)
#define EVE_HOFFSET	(43L)
#define EVE_HCYCLE 	(548L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(5L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_CRYSTAL
#define FT81X_ENABLE
#endif


/* untested */
/* NHD-5.0-800480FT-CxXx-xxx 800x480 5.0" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_50)
#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(3L)
#define EVE_VOFFSET	(32L)
#define EVE_VCYCLE	(525L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(48L)
#define EVE_HOFFSET	(88L)
#define EVE_HCYCLE 	(928L)
#define EVE_PCLKPOL	(0L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_CRYSTAL
#define FT81X_ENABLE
#endif


/* untested */
/* NHD-7.0-800480FT-CxXx-xxx 800x480 7.0" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_70)
#define EVE_HSIZE	(800L)
#define EVE_VSIZE	(480L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(3L)
#define EVE_VOFFSET	(32L)
#define EVE_VCYCLE	(525L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(48L)
#define EVE_HOFFSET	(88L)
#define EVE_HCYCLE 	(928L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_CRYSTAL
#define FT81X_ENABLE
#endif


/* ADAM101-LCP-SWVGA-NEW 1024x600 10.1" Glyn, capacitive, FT813 */
#if defined (EVE_ADAM101)
#define EVE_HSIZE	(1024L)
#define EVE_VSIZE	(600L)

#define EVE_VSYNC0	(0L)
#define EVE_VSYNC1	(1L)
#define EVE_VOFFSET	(1L)
#define EVE_VCYCLE	(720L)
#define EVE_HSYNC0	(0L)
#define EVE_HSYNC1	(1L)
#define EVE_HOFFSET	(1L)
#define EVE_HCYCLE 	(1100L)
#define EVE_PCLKPOL	(1L)
#define EVE_SWIZZLE	(0L)
#define EVE_PCLK	(2L)
#define EVE_CSPREAD	(1L)
#define EVE_TOUCH_RZTHRESH (1200L)
#define EVE_HAS_CRYSTAL
#define FT81X_ENABLE
#endif


#endif /* EVE_CONFIG_H */
