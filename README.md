# FT810-FT813, BT81x
This is a code library for EVE/EVE2/EVE3 graphics controller ICs from FTDI/Bridgetek:

http://www.ftdichip.com/EVE.htm

http://brtchip.com/eve/

http://brtchip.com/ft80x/

http://brtchip.com/ft81x/

https://brtchip.com/bt81x/

It contains code for and has been used with various mikro-controllers and displays.

I have used it so far with:

- 8-Bit AVR, specifically the 90CAN series
- Arduino, Uno, mini-pro, ESP8266
- Renesas F1L RH850
- Infineon Aurix TC222
- ATSAMC21E18A (DMA)
- ATSAME51J19A

I have reports of successfully using it with:

- ATSAMV70
- ATSAMD20
- ATSAME4
- STM32
- some PICs
- ESP32

The TFTs I have tested myself so far:

- VM800B35A http://www.ftdichip.com/Products/Modules/VM800B.html
- VM800B50A
- FT800CB-HY50B http://www.hotmcu.com/5-graphical-lcd-touchscreen-480x272-spi-ft800-p-124.html
- FT810CB-HY50HD http://www.hotmcu.com/5-graphical-lcd-touchscreen-800x480-spi-ft810-p-286.html
- FT811CB-HY50HD  http://www.hotmcu.com/5-graphical-lcd-capacitive-touch-screen-800x480-spi-ft811-p-301.html
- RVT70UQFNWC0x https://riverdi.com/product/rvt70uqfnwc0x/
- RVT50
- ADAM101-LCP-SWVGA-NEW from Glyn, 10.1" 1024x600 cap-touch
- EVE2-38A https://www.matrixorbital.com/ftdi-eve/eve-ft812/eve2-38a
- EVE2-35G https://www.matrixorbital.com/ftdi-eve/eve-ft812/eve2-35g
- EVE2-43G https://www.matrixorbital.com/ftdi-eve/eve-ft812/eve2-43g
- EVE2-50G https://www.matrixorbital.com/ftdi-eve/eve-ft812/eve2-50g
- EVE2-70G https://www.matrixorbital.com/ftdi-eve/eve-ft812/eve2-70g
- NHD-3.5-320240FT-CSXV-CTP
- RVT43ULBNWC00 (RiTFT-43-CAP-UX) https://riverdi.com/product/ritft43capux/

Currently I am implementing BT81x support using a prototype board populated with a BT816 that Matrix Orbital was so kind to provide me with.
I have connected the panel of a EVE2-43G to it.


The examples in the "example_projects" drawer are for use with AtmelStudio7. For Arduino I am using the plugin from www.visualmicro.com .

The platform the code is compiled for is automatically detected thru compiler flags in EVE_config.h. This is the only file that should need editing to customize the library to your needs.

- Select the TFT attached by enabling one of the pre-defined setups.
- Provide the pins used for Chip-Select and Power-Down in the target configuration you are using

When compiling for AVR you need to provide the clock it is running at in order to make the _delay_ms() calls used to initialise the TFT work with the intended timing.
For other plattforms you need to provide a delay(ms) function that works between 1ms and 56ms at least and is at least not performing these delays shorter than requested.
The delay(ms) is only used during initialisation of the FT8xx.

In Addition you need to initialise the pins used for Chip-Select and PowerDown in your hardware correctly to output.
Plus setup the SPI accordingly, mode-0, 8-bit, MSB-first, not more than 11MHz for the init (if the display can handle it).

Originally the project went public in the German mikrocontroller.net forum, the thread contains some insight: https://www.mikrocontroller.net/topic/395608

Feel free to add to the discussion with questions or remarks.

Note, with so many options to choose from now, FT80x support will be removed at some point in the future.
