/*
@file    EVE_commands.h
@brief   contains FT8xx / BT8xx function prototypes
@version 4.0-bes
@date    2019-04-16, original 2019-04-07
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
- changes to this header

2.3

- changed type of result-value for EVE_init from void to uint8_t
- added prototype for EVE_cmd_romfont()
- added millis option to EVE_cmd_clock()
- switched to standard-C compliant comment-style

2.4
- added prototype for EVE_memWrite_flash_buffer()

3.0
- renamed from FT800_commands.h to EVE_commands.h
- changed FT_ prefixes to EVE_
- changed ft800_ prefixes to EVE_
- removed test-function EVE_cmd_loadimage_mf()

3.1
- added prototypes for EVE_cmd_setfont() and EVE_cmd_setfont2()

3.2
- removed several prototypes for commands that do not need a function of their own:
	EVE_cmd_stop(), EVE_cmd_loadidentity(), EVE_cmd_setmatrix(), EVE_cmd_screensaver(),
	EVE_cmd_logo(), EVE_cmd_coldstart()
	These all do not have any arguments and can be used with EVE_cmd_dl(), for example:
	EVE_cmd_dl(CMD_SCREENSAVER);
	EVE_cmd_dl(CMD_SETMATRIX);
- added prototype for EVE_cmd_snapshot2()
- added prototype for EVE_cmd_setscratch()

3.3
- added prototypes for EVE_cmd_memcrc(), EVE_cmd_getptr(), EVE_cmd_regread() and EVE_cmd_getprops()

3.4
- added protoypes for EVE_start_cmd_burst() and EVE_end_cmd_burst()

3.5
- added prototype for EVE_cmd_start()

3.6
- added prototype for EVE_report_cmdoffset()
- removed exporting var cmdOffset

3.7
- sorted functions
- changed #ifdef to #if defined for consistency

4.0
- changed FT8_ prefixes to EVE_
- added EVE_cmd_flashsource()
- added prototype for EVE_init_flash() - not functional yet
- added protoypes for EVE_cmd_flashwrite(), EVE_cmd_flashread(), EVE_cmd_flashupdate(), EVE_cmd_flashfast(), EVE_cmd_flashspitx() and EVE_cmd_flashspirx()
- added prototypes for EVE_cmd_inflate2(), EVE_cmd_rotatearound(), EVE_cmd_animstart(), EVE_cmd_animstop(), EVE_cmd_animxy(),
	EVE_cmd_animdraw(), EVE_cmd_animframe(), EVE_cmd_gradienta(), EVE_cmd_fillwidth() and EVE_cmd_appendf()
- added a paramter to EVE_get_touch_tag() to allow multi-touch

--------------------------------------------------------------------------------
Modified by Alexis Lockwood at Boulder Engineering Studio <alex@boulderes.com>

4.0-bes
- Add support for multiple display drivers and displays
- Add timeouts to the while-busy loops

*/

#ifndef EVE_COMMANDS_H_
#define EVE_COMMANDS_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct eve_context eve_context_t;

/// Display timing and geometry parameters
typedef struct {
    /// Thd Length of visible part of line (in PCLKs) - display width
    uint16_t hsize;
    /// Tvd Number of visible lines (in lines) - display height
    uint16_t vsize;

    /// Tvf Vertical Front Porch
    uint16_t vsync0;
    /// Tvf + Tvp Vertical Front Porch plus Vsync Pulse width
    uint16_t vsync1;
    /// Tvf + Tvp + Tvb Number of non-visible lines (in lines)
    uint16_t voffset;
    /// Tv Total number of lines (visible and non-visible lines) (in lines)
    uint16_t vcycle;

    /// Thf Horizontal Front Porch
    uint16_t hsync0;
    /// Thf + Thp Horizontal Front Porch plus Hsync Pulse width
    uint16_t hsync1;
    /// Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles)
    uint16_t hoffset;
    /// Th Total length of line (visible and non-visible) (in PCLKs)
    uint16_t hcycle;

    /// Touch sensitivity
    uint16_t touch_rzthresh;

    /// PCLK frequency, where freq = 60 MHz / eve_display_t.pclk
    uint16_t pclk;

    /// Helps with noise, when set to 1 fewer signals are changed simultaneously
    uint8_t cspread;

    /// Arrangement of the RGB pins of the FT800
    uint8_t swizzle;

    /// PCLK polarity (false = rising edge, true = falling edge)
    bool pclkpol_falling;

    /// Chip has a crystal
    bool has_crystal;
} eve_display_t;

/// Set of function pointers implementing a low-level driver
typedef struct eve_impl {
    /// Send one byte via SPI
    void (* spi_transmit)(eve_context_t * ctx, uint8_t value);

    /// Send one byte via SPI and return the received byte
    uint8_t (* spi_receive)(eve_context_t * ctx, uint8_t tx_value);

    /// Set or clear the chip select pin
    ///
    /// @param value Whether the pin should be asserted (implementer is
    ///     responsible for active-high/low)
    void (* cs_set)(eve_context_t * ctx, bool value);

    /// Set or clear the power-down pin
    ///
    /// @param value Whether the pin should be asserted (implementer is
    ///     responsible for active-high/low)
    void (* pdn_set)(eve_context_t * ctx, bool value);

    /// Fetch a byte stored in flash. In most implementations this will be a
    /// simple dereference.
    uint8_t (* fetch_flash_byte)(uint8_t const * data);

    /// Delay. This function must delay for at least the specified time (it need
    /// not be accurate).
    ///
    /// @param t delay time in milliseconds, up to 100
    void (* delay_ms)(uint8_t t);

    /// Timeout start. This function marks the start of a timeout and returns a
    /// state to be passed to timeout_test().
    ///
    /// This function pointer may be NULL if timeouts are not required.
    void * (* timeout_start)(eve_context_t * ctx, uint32_t timeout_us);

    /// Timeout check. This function accepts a state saved by timeout_start()
    /// and an interval, and should return true if the timeout period is over.
    ///
    /// It is undefined behavior to pass a different interval to timeout_start()
    /// and timeout_test().
    bool (* timeout_test)(eve_context_t * ctx, void * state, uint32_t timeout_us);

    /// Begin a DMA transfer. A DMA transaction should control the chip select
    /// pin itself, and should write `true` to `ctx->EVE_dma_busy` when
    /// finished.
    ///
    /// Can be NULL if DMA is not to be supported.
    void (* start_dma_transfer)(eve_context_t * ctx);

    /// Initialize DMA.
    ///
    /// Can be NULL if DMA is not to be supported or if DMA needs no independent
    /// initialization. If defined, it will be called at the end of EVE_init().
    void (* init_dma)(eve_context_t * ctx);
} eve_impl_t;

typedef struct eve_context {
    /// Low-level driver implementation
    eve_impl_t const * impl;

    /// Display configuration
    eve_display_t cfg;

    /// Arbitrary data to be used by implementing functions
    void * data;

    /// True to use DMA instead of blocking transfers
    bool use_dma;

    bool volatile EVE_dma_busy;
    uint8_t volatile * EVE_dma_buffer;
    size_t volatile EVE_dma_buffer_index;
    size_t EVE_dma_buffer_len;


    // --- Private members ---
    uint16_t volatile cmdOffset;
    bool volatile cmd_burst;
    bool volatile dma_flushing;
} eve_context_t;

/// DMA-using implementers should call this in their DMA handler when DMA
/// completes.
void EVE_dma_complete(eve_context_t * ctx);

void EVE_cmdWrite(eve_context_t * ctx, uint8_t data);

uint8_t EVE_memRead8(eve_context_t * ctx, uint32_t ftAddress);
uint16_t EVE_memRead16(eve_context_t * ctx, uint32_t ftAddress);
uint32_t EVE_memRead32(eve_context_t * ctx, uint32_t ftAddress);
void EVE_memWrite8(eve_context_t * ctx, uint32_t ftAddress, uint8_t ftData8);
void EVE_memWrite16(eve_context_t * ctx, uint32_t ftAddress, uint16_t ftData16);
void EVE_memWrite32(eve_context_t * ctx, uint32_t ftAddress, uint32_t ftData32);
void EVE_memWrite_flash_buffer(eve_context_t * ctx, uint32_t ftAddress, const uint8_t *data, uint16_t len);
uint8_t EVE_busy(eve_context_t * ctx);
void EVE_get_cmdoffset(eve_context_t * ctx);
uint16_t EVE_report_cmdoffset(eve_context_t * ctx);
uint32_t EVE_get_touch_tag(eve_context_t * ctx, uint8_t num);


/* commands to operate on memory: */
void EVE_cmd_memzero(eve_context_t * ctx, uint32_t ptr, uint32_t num);
void EVE_cmd_memset(eve_context_t * ctx, uint32_t ptr, uint8_t value, uint32_t num);
/*(eve_context_t * ctx, void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *data); */
void EVE_cmd_memcpy(eve_context_t * ctx, uint32_t dest, uint32_t src, uint32_t num);


/* commands for loading image data into FT8xx memory: */
void EVE_cmd_inflate(eve_context_t * ctx, uint32_t ptr, const uint8_t *data, uint16_t len);
void EVE_cmd_loadimage(eve_context_t * ctx, uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len);

#if defined FT81X_ENABLE
void EVE_cmd_mediafifo(eve_context_t * ctx, uint32_t ptr, uint32_t size);
#endif


void EVE_cmd_start(eve_context_t * ctx);
/// @retval true - success
/// @retval false - timeout
bool EVE_cmd_execute(eve_context_t * ctx);

void EVE_start_cmd_burst(eve_context_t * ctx);
void EVE_end_cmd_burst(eve_context_t * ctx);

void EVE_cmd_dl(eve_context_t * ctx, uint32_t command);


/* EVE3 commands */
#if defined BT81X_ENABLE

void EVE_cmd_flashwrite(eve_context_t * ctx, uint32_t ptr, uint32_t num, const uint8_t *data);
void EVE_cmd_flashread(eve_context_t * ctx, uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashupdate(eve_context_t * ctx, uint32_t dest, uint32_t src, uint32_t num);
uint32_t EVE_cmd_flashfast(eve_context_t * ctx);
void EVE_cmd_flashspitx(eve_context_t * ctx, uint32_t num, const uint8_t *data);
void EVE_cmd_flashspirx(eve_context_t * ctx, uint32_t dest, uint32_t num);
void EVE_cmd_flashsource(eve_context_t * ctx, uint32_t ptr);

void EVE_cmd_inflate2(eve_context_t * ctx, uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len);
void EVE_cmd_rotatearound(eve_context_t * ctx, int32_t x0, int32_t y0, int32_t angle, int32_t scale);
void EVE_cmd_animstart(eve_context_t * ctx, int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstop(eve_context_t * ctx, int32_t ch);
void EVE_cmd_animxy(eve_context_t * ctx, int32_t ch, int16_t x0, int16_t y0);
void EVE_cmd_animdraw(eve_context_t * ctx, int32_t ch);
void EVE_cmd_animframe(eve_context_t * ctx, int16_t x0, int16_t y0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_gradienta(eve_context_t * ctx, int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void EVE_cmd_fillwidth(eve_context_t * ctx, uint32_t s);
void EVE_cmd_appendf(eve_context_t * ctx, uint32_t ptr, uint32_t num);

uint8_t EVE_init_flash(eve_context_t * ctx);
#endif


/* commands to draw graphics objects: */
void EVE_cmd_text(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_button(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_clock(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs);
void EVE_cmd_bgcolor(eve_context_t * ctx, uint32_t color);
void EVE_cmd_fgcolor(eve_context_t * ctx, uint32_t color);
void EVE_cmd_gradcolor(eve_context_t * ctx, uint32_t color);
void EVE_cmd_gauge(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_gradient(eve_context_t * ctx, int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void EVE_cmd_keys(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_progress(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_scrollbar(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_slider(eve_context_t * ctx, int16_t x1, int16_t y1, int16_t w1, int16_t h1, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_dial(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val);
void EVE_cmd_toggle(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text);
void EVE_cmd_number(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number);

#if defined FT81X_ENABLE
void EVE_cmd_setbase(eve_context_t * ctx, uint32_t base);
void EVE_cmd_setbitmap(eve_context_t * ctx, uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
#endif


void EVE_cmd_append(eve_context_t * ctx, uint32_t ptr, uint32_t num);


/* commands for setting the bitmap transform matrix: */
void EVE_cmd_translate(eve_context_t * ctx, int32_t tx, int32_t ty);
void EVE_cmd_scale(eve_context_t * ctx, int32_t sx, int32_t sy);
void EVE_cmd_rotate(eve_context_t * ctx, int32_t ang);
void EVE_cmd_getmatrix(eve_context_t * ctx, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f);


/* other commands: */
void EVE_cmd_calibrate(eve_context_t * ctx);
void EVE_cmd_interrupt(eve_context_t * ctx, uint32_t ms);
void EVE_cmd_setfont(eve_context_t * ctx, uint32_t font, uint32_t ptr);
#if defined FT81X_ENABLE
void EVE_cmd_romfont(eve_context_t * ctx, uint32_t font, uint32_t romslot);
void EVE_cmd_setfont2(eve_context_t * ctx, uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setrotate(eve_context_t * ctx, uint32_t r);
void EVE_cmd_setscratch(eve_context_t * ctx, uint32_t handle);
#endif
void EVE_cmd_sketch(eve_context_t * ctx, int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format);
void EVE_cmd_snapshot(eve_context_t * ctx, uint32_t ptr);
#if defined FT81X_ENABLE
void EVE_cmd_snapshot2(eve_context_t * ctx, uint32_t fmt, uint32_t ptr, int16_t x0, int16_t y0, int16_t w0, int16_t h0);
#endif
void EVE_cmd_spinner(eve_context_t * ctx, int16_t x0, int16_t y0, uint16_t style, uint16_t scale);
void EVE_cmd_track(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t tag);


/* commands that return values by writing to the command-fifo */
uint16_t EVE_cmd_memcrc(eve_context_t * ctx, uint32_t ptr, uint32_t num);
uint16_t EVE_cmd_getptr(eve_context_t * ctx);
uint16_t EVE_cmd_regread(eve_context_t * ctx, uint32_t ptr);
uint16_t EVE_cmd_getprops(eve_context_t * ctx, uint32_t ptr);


/* meta-commands, sequences of several display-list entries condensed into simpler to use functions at the price of some overhead */
void EVE_cmd_point(eve_context_t * ctx, int16_t x0, int16_t y0, uint16_t size);
void EVE_cmd_line(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t w0);
void EVE_cmd_rect(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t corner);


/* startup FT8xx: */
uint8_t EVE_init(eve_context_t * ctx);

#endif /* EVE_COMMANDS_H_ */
