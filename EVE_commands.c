/*
@file    EVE_commands.c
@brief   contains FT8xx / BT8xx functions
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

2.1
- removed the REG_ROTATE line from EVE_init()
- simplified EVE_busy() slightly
- implemented EVE_cmd_mediafifo(), EVE_cmd_setrotate(), EVE_cmd_setbitmap() (FT81x)

2.2
- moved hardware abstraction functions over to EVE_config.c

2.3
- changes to this header

2.3
- added "EVE_cmdWrite(EVE_CLKINT);" to top of EVE_init() to switch easier
  between displays with and without crystal populated
  note: need to add a define for internal/external clock to

- added a EVE_get_cmdoffset() call to the end of EVE_init() as it must be called
  after init anyways

2.4
- added a few lines in EVE_init() for the new config define EVE_HAS_CRYSTAL
- EVE_init() does a timeout now instead of an endless loop when waiting for the FT8xx to complete
  it's power-on selftest and returns 0x00 in case of failure'
- added EVE_cmd_romfont() (FT81x only)
- added millis option to EVE_cmd_clock()
- switched to standard-C compliant comment-style

2.5
- added EVE_memWrite_flash_buffer()
- modified EVE_cmd_loadimage() to ignore data when EVE_OPT_MEDIAFIFO is given (FT81x only)

2.6
- deleted a "#define EVE_HAS_CRYSTAL 0" line from EVE_init() which should have been long gone...

2.7
- modified EVE_cmd_loadimage() to load bigger files in chunks of 4000

3.0
- renamed from FT800_commands.c to EVE_commands.c
- changed FT_ prefixes to EVE_
- changed ft800_ prefixes to EVE_
- removed test-function EVE_cmd_loadimage_mf()
- added while (EVE_busy()); to the end of EVE_cmd_execute()
- removed while (EVE_busy()); from EVE_cmd_loadimage()

3.1
- added EVE_cmd_setfont() and EVE_cmd_setfont2() as pointed out missing by Rafael Drzewiecki

3.2
- Bugfix: EVE_cmd_interrupt() was using CMD_SNAPSHOT instead of CMD_INTERRUPT
- removed several functions for commands that do not need a function of their own:
	EVE_cmd_stop(), EVE_cmd_loadidentity(), EVE_cmd_setmatrix(), EVE_cmd_screensaver(),
	EVE_cmd_logo(), EVE_cmd_coldstart()
	These all do not have any arguments and can be used with EVE_cmd_dl(), for example:
	EVE_cmd_dl(CMD_SCREENSAVER);
	EVE_cmd_dl(CMD_SETMATRIX);
- added EVE_cmd_snapshot2()
- added EVE_cmd_setscratch()

3.3
- implemented EVE_cmd_memcrc(), EVE_cmd_getptr(), EVE_cmd_regread(), EVE_cmd_getprops()

3.4
- implemented functions EVE_start_cmd_burst() and EVE_end_cmd_burst()

3.5
- Bugifx: EVE_cmd_setbase() was incrementing the command-offset by 12 instead of 4

3.6
- Bugifix: EVE_cmd_getptr() was using CMD_MEMCRC instead of CMD_GETPTR

3.7
- Added EVE_cmd_start(), a non-blocking variant of EVE_cmd_execute() to be used at the end of a display-list update.
	Thanks for pointing out that oversight to user "Peter" of Mikrocontroller.net!

3.8
- Added setting of REG_CSPRED to EVE_init() as new parameter, some Matrix Orbital modules use '0' instead of the reset-default '1'.

3.9
- Added patching of the touch-interface for GT911 to EVE_init() if required by the module attached
- simplified EVE_init() a bit
- some house-keeping, fixing typos in comments left and right

3.10
- EVE_cmd_inflate() supports binaries >4k now and does not need to be executed anymore

3.11
- added fault-detection and co-processor reset to  EVE_busy(), this allows the FT8xx to continue to work even after beeing supplied with bad image data for example

3.12
- changed the way EVE_HAS_CRYSTAL works
- added special treatment for switching on and off the backlight in EVE_init() for ADAM101 modules.
- default backlight value after EVE_init() is 25% now

3.13
- divided functions into utility and display-list groups, for sync transfers and async transfers
- changed #ifdef to #if defined for consistency
- changed all spi_transmit() calls in the display-list functions to spi_transmit_async() calls, preparing for DMA display-list buffer transfer

4.0
- renamed from EVE_commands.c to EVE_commands.c
- changed FT8_ prefixes to EVE_
- apparently BT815 supports Goodix touch-controller directly, so changed EVE_init() accordingly
- added EVE_cmd_flashsource()
- added EVE_init_flash() - empty for now
- added EVE_cmd_flashwrite(), EVE_cmd_flashread(), EVE_cmd_flashupdate(), EVE_cmd_flashfast(), EVE_cmd_flashspitx(), EVE_cmd_flashspirx()
- changed block_transfer() that is used by EVE_cmd_inflate() and EVE_cmd_loadimage() to transfer the data in chunks of
  a maximum of 3840 bytes instead of 4000 bytes.
  The new EVE_cmd_flashwrite() uses block_transfer() as well and it needs the data in multiples of 256 bytes.
  Breaking the data down to any number should be ok, as long as the total is a multiple of 256 - but better safe than sorry...
- Bugfix, sort of: in order to add padding bytes for 4-byte alignment spi_flash_write() was reading up to 3 bytes past the supplied buffer, now 1, 2 or 3 zero-bytes are send
- extended the maximum timeout in EVE_init() to 400ms as at least according to the datasheets "The boot-up may take up to 300ms to complete"
- tried to fill in EVE_init_flash() - nothing to test it with, yet
- added DMA
- reworked all commands that support burst to use spi_transmit() in non burst mode and spi_transmit_async() when burst is aktive
- added a conditional reset to the init for the BT816 test-config
- implemented EVE_cmd_inflate2()
- made EVE_cmd_loadimage() aware of EVE_OPT_FLASH when compiled for BT81x
- implemented EVE_cmd_rotatearound(), EVE_cmd_animstart(), EVE_cmd_animstop(), EVE_cmd_animxy(), EVE_cmd_animdraw(),
	EVE_cmd_animframe(), EVE_cmd_gradienta(), EVE_cmd_fillwidth() and EVE_cmd_appendf()
- upgraded EVE_get_touch_tag() to multi-touch

--------------------------------------------------------------------------------
Modified by Alexis Lockwood at Boulder Engineering Studio <alex@boulderes.com>

4.0-bes
- Add support for multiple display drivers
- Add timeouts to busy check

*/

#include "EVE.h"
#include "EVE_commands.h"
#include "board.h"


/* EVE Memory Commands - used with EVE_memWritexx and EVE_memReadxx */
#define MEM_WRITE	0x80	/* EVE Host Memory Write */
#define MEM_READ	0x00	/* EVE Host Memory Read */

#define TIMEOUT_US 2000000u

/// Wait until the chip is not busy. If the context provides timeout support
/// functions, return true on success or false on timeout. Otherwise, always
/// return true.
static bool _wait_while_busy(eve_context_t * ctx, uint32_t timeout_us);

/// Return whether DMA is allowed for this particular instance
static bool _using_dma(eve_context_t * ctx);

/// If DMA is allowed, buffer another byte to be sent via DMA. If the buffer
/// is full, schedule a transmission and block until complete, then start
/// filling the buffer again.
///
/// If DMA is not allowed, just send the byte.
static void _spi_transmit_async(eve_context_t * ctx, uint8_t v);

void EVE_cmdWrite(eve_context_t * ctx, uint8_t data)
{
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, data);
	ctx->impl->spi_transmit(ctx, 0x00);
	ctx->impl->spi_transmit(ctx, 0x00);
    ctx->impl->cs_set(ctx, false);
}


uint8_t EVE_memRead8(eve_context_t * ctx, uint32_t ftAddress)
{
	uint8_t ftData8;
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_READ); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8)); /* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));	/* send low address byte */
	ctx->impl->spi_transmit(ctx, 0x00);	/* send dummy byte */
	ftData8 = ctx->impl->spi_receive(ctx, 0x00); /* read data byte by sending another dummy byte */
    ctx->impl->cs_set(ctx, false);
	return ftData8;	/* return byte read */
}


uint16_t EVE_memRead16(eve_context_t * ctx, uint32_t ftAddress)
{
	uint16_t ftData16 = 0;
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_READ); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8)); /* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress)); /* send low address byte */
	ctx->impl->spi_transmit(ctx, 0x00);	/* send dummy byte */
	ftData16 = (ctx->impl->spi_receive(ctx, 0x00));	/* read low byte */
	ftData16 = (ctx->impl->spi_receive(ctx, 0x00) << 8) | ftData16;	/* read high byte */
	ctx->impl->cs_set(ctx, false);
	return ftData16; /* return integer read */
}


uint32_t EVE_memRead32(eve_context_t * ctx, uint32_t ftAddress)
{
	uint32_t ftData32= 0;
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_READ); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8)); /* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));	/* send low address byte */
	ctx->impl->spi_transmit(ctx, 0x00);	/* send dummy byte */
	ftData32 = ((uint32_t)ctx->impl->spi_receive(ctx, 0x00)); /* read low byte */
	ftData32 = ((uint32_t)ctx->impl->spi_receive(ctx, 0x00) << 8) | ftData32;
	ftData32 = ((uint32_t)ctx->impl->spi_receive(ctx, 0x00) << 16) | ftData32;
	ftData32 = ((uint32_t)ctx->impl->spi_receive(ctx, 0x00) << 24) | ftData32; /* read high byte */
    ctx->impl->cs_set(ctx, false);
	return ftData32; /* return long read */
}


void EVE_memWrite8(eve_context_t * ctx, uint32_t ftAddress, uint8_t ftData8)
{
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));
	ctx->impl->spi_transmit(ctx, ftData8);
    ctx->impl->cs_set(ctx, false);
}


void EVE_memWrite16(eve_context_t * ctx, uint32_t ftAddress, uint16_t ftData16)
{
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8)); /* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress)); /* send low address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftData16)); /* send data low byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftData16 >> 8));  /* send data high byte */
    ctx->impl->cs_set(ctx, false);
}


void EVE_memWrite32(eve_context_t * ctx, uint32_t ftAddress, uint32_t ftData32)
{
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));	/* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));		/* send low address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftData32));		/* send data low byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftData32 >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftData32 >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftData32 >> 24));	/* send data high byte */
    ctx->impl->cs_set(ctx, false);
}


void EVE_memWrite_flash_buffer(eve_context_t * ctx, uint32_t ftAddress, const uint8_t *data, uint16_t len)
{
	uint16_t count;

    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));

	len = (len + 3)&(~3);

	for(count=0;count<len;count++)
	{
		ctx->impl->spi_transmit(ctx, ctx->impl->fetch_flash_byte(data+count));
	}

    ctx->impl->cs_set(ctx, false);
}



/* Check if the graphics processor completed executing the current command list. */
/* This is the case when REG_CMD_READ matches cmdOffset, indicating that all commands have been executed. */
uint8_t EVE_busy(eve_context_t * ctx)
{
	uint16_t cmdBufferRead;

	if(_using_dma(ctx) && ctx->EVE_dma_busy)
	{
		return 1;
	}

	cmdBufferRead = EVE_memRead16(ctx, REG_CMD_READ);	/* read the graphics processor read pointer */

	if(cmdBufferRead == 0xFFF)
	{
		EVE_memWrite8(ctx, REG_CPURESET, 1);		/* hold co-processor engine in the reset condition */
		EVE_memWrite16(ctx, REG_CMD_READ, 0);	/* set REG_CMD_READ to 0 */
		EVE_memWrite16(ctx, REG_CMD_WRITE, 0);	/* set REG_CMD_WRITE to 0 */
		ctx->cmdOffset = 0;						/* reset cmdOffset */
		EVE_memWrite8(ctx, REG_CPURESET, 0);		/* set REG_CMD_WRITE to 0 to restart the co-processor engine*/
	}

	if(ctx->cmdOffset != cmdBufferRead)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


uint32_t EVE_get_touch_tag(eve_context_t * ctx, uint8_t num)
{
	uint32_t value;

	if(_using_dma(ctx) && ctx->EVE_dma_busy)
	{
		return 0; /* just do nothing if a dma transfer is in progress */
	}

	switch(num)
	{
		case 1:
			value = EVE_memRead32(ctx, REG_TOUCH_TAG); /* read the value for the first touch point */
			break;
#ifdef FT81X_ENABLE
		case 2:
			value = EVE_memRead32(ctx, REG_TOUCH_TAG1);
			break;
		case 3:
			value = EVE_memRead32(ctx, REG_TOUCH_TAG2);
			break;
		case 4:
			value = EVE_memRead32(ctx, REG_TOUCH_TAG3);
			break;
		case 5:
			value = EVE_memRead32(ctx, REG_TOUCH_TAG4);
			break;
#endif
		default:
			value = EVE_memRead32(ctx, REG_TOUCH_TAG);
			break;
	}

	return value;
}


void EVE_get_cmdoffset(eve_context_t * ctx)
{
	ctx->cmdOffset = EVE_memRead16(ctx, REG_CMD_WRITE);
}


/* make current value of cmdOffset available while limiting access to that var to the EVE_commands module */
uint16_t EVE_report_cmdoffset(eve_context_t * ctx)
{
	return (ctx->cmdOffset);
}


void EVE_inc_cmdoffset(eve_context_t * ctx, uint16_t increment)
{
	ctx->cmdOffset += increment;
	ctx->cmdOffset &= 0x0fff;
}


/* order the command co-processor to start processing its FIFO queue and do not wait for completion */
void EVE_cmd_start(eve_context_t * ctx)
{
	uint32_t ftAddress;

	if(_using_dma(ctx) && ctx->EVE_dma_busy)
	{
		return; /* just do nothing if a dma transfer is in progress */
	}

	ftAddress = REG_CMD_WRITE;

    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));	/* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));			/* send low address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ctx->cmdOffset));			/* send data low byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ctx->cmdOffset >> 8));	/* send data high byte */
    ctx->impl->cs_set(ctx, false);
}


/* order the command co-processor to start processing its FIFO queue and wait for completion */
bool EVE_cmd_execute(eve_context_t * ctx)
{
	EVE_cmd_start(ctx);
    return _wait_while_busy(ctx, TIMEOUT_US);
}


/* Begin a co-processor command, this is used for all non-display-list commands */
void EVE_begin_cmd(eve_context_t * ctx, uint32_t command)
{
	uint32_t ftAddress;

	ftAddress = EVE_RAM_CMD + ctx->cmdOffset;
    ctx->impl->cs_set(ctx, true);
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));	/* send middle address byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));		/* send low address byte */

	ctx->impl->spi_transmit(ctx, (uint8_t)(command));		/* send data low byte */
	ctx->impl->spi_transmit(ctx, (uint8_t)(command >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(command >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(command >> 24));		/* Send data high byte */
	EVE_inc_cmdoffset(ctx, 4);			/* update the command-ram pointer */
}


/* co-processor commands that are not used in displays lists, these are no to be used with async transfers */


/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_memzero(eve_context_t * ctx, uint32_t ptr, uint32_t num)
{
	EVE_begin_cmd(ctx, CMD_MEMZERO);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 8);

    ctx->impl->cs_set(ctx, false);
}


/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_memset(eve_context_t * ctx, uint32_t ptr, uint8_t value, uint32_t num)
{
	EVE_begin_cmd(ctx, CMD_MEMSET);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, value);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 12);

    ctx->impl->cs_set(ctx, false);
}


/* this is meant to be called outside display-list building, does not support cmd-burst */
/*
void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *data)
{
	EVE_begin_cmd(CMD_MEMWRITE);

	spi_transmit((uint8_t)(dest));
	spi_transmit((uint8_t)(dest >> 8));
	spi_transmit((uint8_t)(dest >> 16));
	spi_transmit((uint8_t)(dest >> 24));

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	num = (num + 3)&(~3);

	for(count=0;count<len;count++)
	{
		spi_transmit(pgm_read_byte_far(data+count));
	}

	EVE_inc_cmdoffset(8+len);

	ctx->cs_set(ctx, false);
}
*/

/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_memcpy(eve_context_t * ctx, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_begin_cmd(ctx, CMD_MEMCPY);

	ctx->impl->spi_transmit(ctx, (uint8_t)(dest));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(src));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 12);

    ctx->impl->cs_set(ctx, false);
}


/* commands for loading image data into FT8xx memory: */

static void spi_flash_write(eve_context_t * ctx, const uint8_t *data, uint16_t len)
{
	uint16_t count;
	uint8_t padding;

	padding = len & 0x03; /* 0, 1, 2 or 3 */
	padding = 4-padding; /* 4, 3, 2 or 1 */
	padding &= 3; /* 3, 2 or 1 */

	for(count=0;count<len;count++)
	{
		ctx->impl->spi_transmit(ctx, ctx->impl->fetch_flash_byte(data+count));
	}

	len += padding;

	while(padding > 0)
	{
		ctx->impl->spi_transmit(ctx, 0);
		padding--;
	}

	EVE_inc_cmdoffset(ctx, len);
}


static void block_transfer(eve_context_t * ctx, const uint8_t *data, uint16_t len)
{
	uint16_t bytes_left;
	uint16_t block_len;
	uint32_t ftAddress;

	bytes_left = len;
	while(bytes_left > 0)
	{
		block_len = bytes_left>3840 ? 3840:bytes_left;

		ftAddress = EVE_RAM_CMD + ctx->cmdOffset;
        ctx->impl->cs_set(ctx, true);
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));	/* send middle address byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));		/* send low address byte */
		spi_flash_write(ctx, data,block_len);
        ctx->impl->cs_set(ctx, false);
		data += block_len;
		bytes_left -= block_len;
		EVE_cmd_execute(ctx);
	}
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void EVE_cmd_inflate(eve_context_t * ctx, uint32_t ptr, const uint8_t *data, uint16_t len)
{
	EVE_begin_cmd(ctx, CMD_INFLATE);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	EVE_inc_cmdoffset(ctx, 4);
    ctx->impl->cs_set(ctx, false);

	block_transfer(ctx, data, len);
}


#if defined (BT81X_ENABLE)
/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void EVE_cmd_inflate2(eve_context_t * ctx, uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len)
{
	EVE_begin_cmd(ctx, CMD_INFLATE2);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(options));
	ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 24));

	EVE_inc_cmdoffset(ctx, 8);
    ctx->impl->cs_set(ctx, false);

	if(options == 0) /* direct data, not by Media-FIFO or Flash */
	{
		block_transfer(ctx, data, len);
	}
}
#endif


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void EVE_cmd_loadimage(eve_context_t * ctx, uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len)
{
	EVE_begin_cmd(ctx, CMD_LOADIMAGE);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(options));
	ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 24));

	EVE_inc_cmdoffset(ctx, 8);
    ctx->impl->cs_set(ctx, false);

	#if defined (BT81X_ENABLE)
	if( ((options & EVE_OPT_MEDIAFIFO) == 0) && ((options & EVE_OPT_FLASH) == 0) )/* direct data, neither by Media-FIFO or from Flash */
	#elif defined (FT81X_ENABLE)
	if((options & EVE_OPT_MEDIAFIFO) == 0) /* direct data, not by Media-FIFO */
	#endif
	{
		block_transfer(ctx, data, len);
	}
}


#if defined (FT81X_ENABLE)
/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_mediafifo(eve_context_t * ctx, uint32_t ptr, uint32_t size)
{
	EVE_begin_cmd(ctx, CMD_MEDIAFIFO);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(size));
	ctx->impl->spi_transmit(ctx, (uint8_t)(size >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(size >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(size >> 24));

	EVE_inc_cmdoffset(ctx, 8);
    ctx->impl->cs_set(ctx, false);
}
#endif


/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_interrupt(eve_context_t * ctx, uint32_t ms)
{
	EVE_begin_cmd(ctx, CMD_INTERRUPT);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ms));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ms >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ms >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ms >> 24));

	EVE_inc_cmdoffset(ctx, 4);

    ctx->impl->cs_set(ctx, false);
}


/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_setfont(eve_context_t * ctx, uint32_t font, uint32_t ptr)
{
	EVE_begin_cmd(ctx, CMD_SETFONT);

	ctx->impl->spi_transmit(ctx, (uint8_t)(font));
	ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	EVE_inc_cmdoffset(ctx, 8);

    ctx->impl->cs_set(ctx, false);
}


#if defined (FT81X_ENABLE)
/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_setfont2(eve_context_t * ctx, uint32_t font, uint32_t ptr, uint32_t firstchar)
{
	EVE_begin_cmd(ctx, CMD_SETFONT2);

	ctx->impl->spi_transmit(ctx, (uint8_t)(font));
	ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(firstchar));
	ctx->impl->spi_transmit(ctx, (uint8_t)(firstchar >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(firstchar >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(firstchar >> 24));

	EVE_inc_cmdoffset(ctx, 12);

	ctx->impl->cs_set(ctx, false);
}
#endif


#if defined (FT81X_ENABLE)
/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_setrotate(eve_context_t * ctx, uint32_t r)
{
	EVE_begin_cmd(ctx, CMD_SETROTATE);

	ctx->impl->spi_transmit(ctx, (uint8_t)(r));
	ctx->impl->spi_transmit(ctx, (uint8_t)(r >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(r >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(r >> 24));

	EVE_inc_cmdoffset(ctx, 4);

	ctx->impl->cs_set(ctx, false);
}
#endif


/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_snapshot(eve_context_t * ctx, uint32_t ptr)
{
	EVE_begin_cmd(ctx, CMD_SNAPSHOT);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	EVE_inc_cmdoffset(ctx, 4);

	ctx->impl->cs_set(ctx, false);
}


#if defined (FT81X_ENABLE)
/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_snapshot2(eve_context_t * ctx, uint32_t fmt, uint32_t ptr, int16_t x0, int16_t y0, int16_t w0, int16_t h0)
{
	EVE_begin_cmd(ctx, CMD_SNAPSHOT2);

	ctx->impl->spi_transmit(ctx, (uint8_t)(fmt));
	ctx->impl->spi_transmit(ctx, (uint8_t)(fmt >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(fmt >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(fmt >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

	EVE_inc_cmdoffset(ctx, 16);

	ctx->impl->cs_set(ctx, false);
}
#endif


/* this is meant to be called outside display-list building, does not support cmd-burst */
void EVE_cmd_track(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t tag)
{
	EVE_begin_cmd(ctx, CMD_TRACK);

	ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
	ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

	ctx->impl->spi_transmit(ctx, (uint8_t)(tag));
	ctx->impl->spi_transmit(ctx, (uint8_t)(tag >> 8));

	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	EVE_inc_cmdoffset(ctx, 12);

	ctx->impl->cs_set(ctx, false);
}



/* commands that return values by writing to the command-fifo */
/* this is handled by having this functions return the offset address on the command-fifo from
   which the results can be fetched after execution: EVE_memRead32(EVE_RAM_CMD + offset) */
/* note: yes, these are different than the functions in the Programmers Guide from FTDI,
	this is because I have no idea why anyone would want to pass "result" as an actual argument to these functions
	when this only marks the offset the command-processor is writing to, it may even be okay to not transfer anything at all,
	just advance the offset by 4 bytes */

/*
example of using EVE_cmd_memcrc:

 offset = EVE_cmd_memcrc(my_ptr_to_some_memory_region, some_amount_of_bytes);
 EVE_cmd_execute();
 crc32 = EVE_memRead32(EVE_RAM_CMD + offset);

*/

/* this is meant to be called outside display-list building, does not support cmd-burst */
uint16_t EVE_cmd_memcrc(eve_context_t * ctx, uint32_t ptr, uint32_t num)
{
	uint16_t offset;

	EVE_begin_cmd(ctx, CMD_MEMCRC);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	EVE_inc_cmdoffset(ctx, 8);
	offset = ctx->cmdOffset;
	EVE_inc_cmdoffset(ctx, 4);

	ctx->impl->cs_set(ctx, false);

	return offset;
}


/* this is meant to be called outside display-list building, does not support cmd-burst */
uint16_t EVE_cmd_getptr(eve_context_t * ctx)
{
	uint16_t offset;

	EVE_begin_cmd(ctx, CMD_GETPTR);

	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	offset = ctx->cmdOffset;
	EVE_inc_cmdoffset(ctx, 4);

	ctx->impl->cs_set(ctx, false);

	return offset;
}


/* this is meant to be called outside display-list building, does not support cmd-burst */
uint16_t EVE_cmd_regread(eve_context_t * ctx, uint32_t ptr)
{
	uint16_t offset;

	EVE_begin_cmd(ctx, CMD_REGREAD);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	EVE_inc_cmdoffset(ctx, 4);
	offset = ctx->cmdOffset;
	EVE_inc_cmdoffset(ctx, 4);

	ctx->impl->cs_set(ctx, false);

	return offset;
}


/* be aware that this returns the first offset pointing to "width", in order to also read
"height" you need to:

 offset = EVE_cmd_getprops(my_last_picture_pointer);
 EVE_cmd_execute();
 width = EVE_memRead32(EVE_RAM_CMD + offset);
 offset += 4;
 offset &= 0x0fff;
 height = EVE_memRead32(EVE_RAM_CMD + offset);
*/

/* this is meant to be called outside display-list building, does not support cmd-burst */
uint16_t EVE_cmd_getprops(eve_context_t * ctx, uint32_t ptr)
{
	uint16_t offset;

	EVE_begin_cmd(ctx, CMD_REGREAD);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);

	EVE_inc_cmdoffset(ctx, 4);
	offset = ctx->cmdOffset;
	EVE_inc_cmdoffset(ctx, 4);

	ctx->impl->cs_set(ctx, false);

	return offset;
}



/* FT811 / FT813 binary-blob from FTDIs AN_336 to patch the touch-engine for Goodix GT911 / GT9271 touch controllers */
#if defined (EVE_HAS_GT911)

#if	defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

const uint16_t EVE_GT911_len = 1184;
const uint8_t EVE_GT911_data[1184] PROGMEM =
{
	26,255,255,255,32,32,48,0,4,0,0,0,2,0,0,0,
	34,255,255,255,0,176,48,0,120,218,237,84,221,111,84,69,20,63,51,179,93,160,148,101,111,76,5,44,141,123,111,161,11,219,154,16,9,16,17,229,156,75,26,11,13,21,227,3,16,252,184,179,
	45,219,143,45,41,125,144,72,67,100,150,71,189,113,18,36,17,165,100,165,198,16,32,17,149,196,240,128,161,16,164,38,54,240,0,209,72,130,15,38,125,48,66,82,30,76,19,31,172,103,46,
	139,24,255,4,227,157,204,156,51,115,102,206,231,239,220,5,170,94,129,137,75,194,216,98,94,103,117,115,121,76,131,177,125,89,125,82,123,60,243,58,142,242,204,185,243,188,118,156,
	227,155,203,238,238,195,251,205,229,71,92,28,169,190,184,84,143,113,137,53,244,103,181,237,87,253,113,137,233,48,12,198,165,181,104,139,25,84,253,155,114,74,191,0,54,138,163,
	12,62,131,207,129,23,217,34,91,31,128,65,246,163,175,213,8,147,213,107,35,203,94,108,3,111,40,171,83,24,15,165,177,222,116,97,23,188,140,206,150,42,102,181,87,78,86,182,170,134,
	215,241,121,26,243,252,2,76,115,217,139,222,206,173,136,132,81,61,35,185,39,113,23,46,199,76,178,54,151,183,224,0,40,189,28,149,182,58,131,79,152,30,76,34,98,234,162,216,133,141,
	102,39,170,40,192,101,53,201,146,191,37,77,44,177,209,74,211,5,206,187,5,6,216,47,53,96,123,22,50,103,251,192,84,17,74,227,185,56,106,51,91,161,96,182,163,48,171,141,139,65,152,
	66,66,11,102,43,158,75,36,80,147,184,147,139,112,17,235,216,103,111,239,245,92,10,175,194,40,44,58,125,5,59,112,50,103,245,4,78,192,5,156,194,51,60,191,134,75,110,173,237,46,192,
	121,156,192,115,184,218,120,67,63,115,46,11,102,10,97,232,50,235,114,182,148,118,178,41,188,12,135,77,202,124,12,96,238,35,161,234,189,129,23,249,212,139,230,25,53,48,205,52,93,
	163,117,53,154,170,81,85,163,178,70,69,66,167,241,14,46,241,1,226,136,152,179,197,59,184,148,254,49,132,48,15,176,137,192,76,131,196,105,104,162,86,81,160,165,255,26,173,162,137,
	86,145,210,183,192,55,175,194,211,60,91,120,230,184,174,27,41,131,155,40,224,29,87,179,232,16,55,55,7,165,147,81,23,165,49,101,54,224,75,180,81,108,18,29,226,69,225,110,175,224,
	42,212,25,47,130,193,110,234,192,215,252,56,74,162,24,46,251,174,54,106,68,245,14,9,155,160,22,120,207,104,240,29,90,178,140,28,24,220,47,166,112,61,251,208,192,111,56,239,238,
	93,255,251,62,99,32,193,75,61,190,235,123,229,110,218,194,85,79,225,59,98,20,238,227,235,220,11,221,149,25,180,116,194,159,111,96,192,24,213,59,139,179,156,215,69,230,19,24,35,
	135,117,206,171,206,162,67,129,234,61,235,11,104,103,84,64,223,167,254,40,163,101,92,84,43,150,46,249,219,205,7,116,11,91,104,61,57,75,223,8,48,25,28,119,252,222,113,49,86,249,
	74,180,211,156,181,61,215,168,157,7,251,199,150,242,250,91,58,132,94,121,7,53,151,139,98,6,165,153,69,214,32,110,211,100,101,31,89,45,81,98,23,205,205,197,209,109,186,198,35,
	141,191,249,25,60,132,223,153,251,98,20,239,146,139,20,217,250,41,250,137,58,177,90,57,79,51,108,233,20,253,194,187,49,222,205,114,141,96,48,175,219,107,54,111,138,22,154,103,
	108,79,58,252,179,178,79,164,195,2,153,36,39,170,199,201,167,197,85,106,8,59,177,81,46,56,2,230,75,114,17,55,112,188,65,208,137,77,114,10,115,55,58,208,197,173,122,87,6,140,
	110,42,208,124,163,70,108,241,104,18,245,98,214,187,134,53,42,221,22,182,133,211,116,148,177,194,209,192,85,90,199,58,55,203,2,229,19,137,187,161,228,154,112,203,145,125,244,
	188,220,118,228,41,201,181,41,195,144,215,183,51,80,250,21,217,16,217,200,235,109,227,188,122,218,142,60,170,224,112,240,184,130,229,224,113,5,223,148,163,80,165,183,130,187,
	132,116,64,238,161,85,220,115,139,205,98,227,244,29,102,125,7,37,243,123,223,11,26,92,63,243,116,61,191,138,123,244,160,84,186,74,31,5,174,247,119,135,199,248,253,135,242,97,
	102,145,190,144,14,85,238,221,231,193,158,48,205,25,120,248,15,220,29,158,9,70,185,30,103,229,33,254,23,237,160,172,62,193,90,222,224,232,14,200,56,90,104,142,227,120,110,6,
	21,211,203,65,150,99,151,220,247,87,164,50,159,49,239,234,58,142,0,109,108,123,18,79,227,36,100,248,222,205,96,127,120,26,171,228,69,63,36,17,252,200,17,116,242,187,227,88,143,
	247,2,75,191,6,130,59,188,11,55,240,31,243,122,152,226,183,207,154,73,188,39,219,43,105,222,87,41,143,141,140,175,73,112,184,252,61,184,16,90,250,35,168,82,119,176,57,116,94,
	200,150,22,190,179,44,104,12,235,84,149,102,252,89,154,193,99,228,106,242,125,248,64,194,255,223,127,242,83,11,255,2,70,214,226,128,0,0
};
#endif


/* init, has to be executed with the SPI setup to 11 MHz or less as required by FT8xx */
uint8_t EVE_init(eve_context_t * ctx)
{
	uint8_t chipid;
	uint16_t timeout = 0;

    ctx->impl->pdn_set(ctx, true);
	ctx->impl->delay_ms(6);	/* minimum time for power-down is 5ms */
    ctx->impl->pdn_set(ctx, false);
	ctx->impl->delay_ms(21);	/* minimum time to allow from rising PD_N to first access is 20ms */

	#if defined (EVE3_43)
		EVE_cmdWrite(ctx, EVE_CORERST); /* test-setup needs a reset at warmstart, probably an issue with the PCB */
	#endif

//	EVE_cmdWrite(EVE_CORERST); /* reset, only required for warmstart if PowerDown line is not used */

    if (ctx->cfg.has_crystal) {
		EVE_cmdWrite(ctx, EVE_CLKEXT);	/* setup FT8xx for external clock */
    } else {
		EVE_cmdWrite(ctx, EVE_CLKINT);	/* setup FT8xx for internal clock */
    }

	EVE_cmdWrite(ctx, EVE_ACTIVE);	/* start FT8xx */

	chipid = EVE_memRead8(ctx, REG_ID);	/* Read ID register */
	while(chipid != 0x7C)	/* if chipid is not 0x7c, continue to read it until it is, FT81x may need a moment for it's power on selftest */
	{
		chipid = EVE_memRead8(ctx, REG_ID);
		ctx->impl->delay_ms(1);
		timeout++;
		if(timeout > 400)
		{
			return 0;
		}
	}

	/* we have a display with a Goodix GT911 / GT9271 touch-controller on it, so we patch our FT811 or FT813 according to AN_336 or setup a BT815 accordingly */
	#if defined (EVE_HAS_GT911)

	#if defined (BT81X_ENABLE)
		EVE_memWrite32(ctx, REG_TOUCH_CONFIG, 0x000005d1); /* switch to Goodix touch controller */
	#else
		uint32_t ftAddress;

		EVE_get_cmdoffset(ctx);
		ftAddress = EVE_RAM_CMD + ctx->cmdOffset;

		ctx->impl->cs_set(ctx, true);
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8)); /* send middle address byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress)); /* send low address byte */
		spi_flash_write(ctx, EVE_GT911_data, EVE_GT911_len);
		ctx->impl->cs_set(ctx, false);
		EVE_cmd_execute(ctx);

		EVE_memWrite8(ctx, REG_TOUCH_OVERSAMPLE, 0x0f); /* setup oversample to 0x0f as "hidden" in binary-blob for AN_336 */
		EVE_memWrite16(ctx, REG_TOUCH_CONFIG, 0x05D0); /* write magic cookie as requested by AN_336 */

		/* specific to the EVE2 modules from Matrix-Orbital we have to use GPIO3 to reset GT911 */
		EVE_memWrite16(ctx, REG_GPIOX_DIR,0x8008); /* Reset-Value is 0x8000, adding 0x08 sets GPIO3 to output, default-value for REG_GPIOX is 0x8000 -> Low output on GPIO3 */
		ctx->impl->delay_ms(1); /* wait more than 100µs */
		EVE_memWrite8(ctx, REG_CPURESET, 0x00); /* clear all resets */
		ctx->impl->delay_ms(56); /* wait more than 55ms */
		EVE_memWrite16(ctx, REG_GPIOX_DIR,0x8000); /* setting GPIO3 back to input */
	#endif
	#endif


	/*	EVE_memWrite8(REG_PCLK, 0x00);	*/	/* set PCLK to zero - don't clock the LCD until later, line disabled because zero is reset-default and we just did a reset */

	#if defined (EVE_ADAM101)
	EVE_memWrite8(ctx, REG_PWM_DUTY, 0x80);	/* turn off backlight for Glyn ADAM101 module, it uses inverted values */
	#else
	EVE_memWrite8(ctx, REG_PWM_DUTY, 0);		/* turn off backlight for any other module */
	#endif

	/* Initialize Display */
	EVE_memWrite16(ctx, REG_HSIZE,   ctx->cfg.hsize);		/* active display width */
	EVE_memWrite16(ctx, REG_HCYCLE,  ctx->cfg.hcycle);	/* total number of clocks per line, incl front/back porch */
	EVE_memWrite16(ctx, REG_HOFFSET, ctx->cfg.hoffset);	/* start of active line */
	EVE_memWrite16(ctx, REG_HSYNC0,  ctx->cfg.hsync0);	/* start of horizontal sync pulse */
	EVE_memWrite16(ctx, REG_HSYNC1,  ctx->cfg.hsync1);	/* end of horizontal sync pulse */
	EVE_memWrite16(ctx, REG_VSIZE,   ctx->cfg.vsize);		/* active display height */
	EVE_memWrite16(ctx, REG_VCYCLE,  ctx->cfg.vcycle);	/* total number of lines per screen, incl pre/post */
	EVE_memWrite16(ctx, REG_VOFFSET, ctx->cfg.voffset);	/* start of active screen */
	EVE_memWrite16(ctx, REG_VSYNC0,  ctx->cfg.vsync0);	/* start of vertical sync pulse */
	EVE_memWrite16(ctx, REG_VSYNC1,  ctx->cfg.vsync1);	/* end of vertical sync pulse */
	EVE_memWrite8(ctx, REG_SWIZZLE,  ctx->cfg.swizzle);	/* FT8xx output to LCD - pin order */
	EVE_memWrite8(ctx, REG_PCLK_POL, ctx->cfg.pclkpol_falling ? 1 : 0);	/* LCD data is clocked in on this PCLK edge */
	EVE_memWrite8(ctx, REG_CSPREAD,	ctx->cfg.cspread);	/* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */

	/* Don't set PCLK yet - wait for just after the first display list */

	/* configure Touch */
	EVE_memWrite8(ctx, REG_TOUCH_MODE, EVE_TMODE_CONTINUOUS);	/* enable touch */
	EVE_memWrite16(ctx, REG_TOUCH_RZTHRESH, ctx->cfg.touch_rzthresh);	/* eliminate any false touches */

	/* disable Audio for now */
	EVE_memWrite8(ctx, REG_VOL_PB, 0x00);	/* turn recorded audio volume down */
	EVE_memWrite8(ctx, REG_VOL_SOUND, 0x00);	/* turn synthesizer volume off */
	EVE_memWrite16(ctx, REG_SOUND, 0x6000);	/*	set synthesizer to mute */

	/* write a basic display-list to get things started */
	EVE_memWrite32(ctx, EVE_RAM_DL, DL_CLEAR_RGB);
	EVE_memWrite32(ctx, EVE_RAM_DL + 4, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
	EVE_memWrite32(ctx, EVE_RAM_DL + 8, DL_DISPLAY);	/* end of display list */
	EVE_memWrite32(ctx, REG_DLSWAP, EVE_DLSWAP_FRAME);

	/* nothing is being displayed yet... the pixel clock is still 0x00 */
	EVE_memWrite8(ctx, REG_GPIO, 0x80); /* enable the DISP signal to the LCD panel, it is set to output in REG_GPIO_DIR by default */
	EVE_memWrite8(ctx, REG_PCLK, ctx->cfg.pclk); /* now start clocking data to the LCD panel */

	#if defined (EVE_ADAM101)
	EVE_memWrite8(ctx, REG_PWM_DUTY, 0x60);	/* turn on backlight to 25% for Glyn ADAM101 module, it uses inverted values */
	#else
	EVE_memWrite8(ctx, REG_PWM_DUTY, 0x20);	/* turn on backlight to 25% for any other module */
	#endif

	ctx->impl->delay_ms(2); /* just to be safe */
    if (!_wait_while_busy(ctx, TIMEOUT_US)) {
        return 0;
    }
	EVE_get_cmdoffset(ctx); /* just to be safe */

    if (_using_dma(ctx) && ctx->impl->init_dma) {
        ctx->impl->init_dma(ctx);
    }

	return 1;
}


/*
These eliminate the overhead of transmitting the command-fifo address with every single command, just wrap a sequence of commands
with these and the address is only transmitted once at the start of the block.
Be careful to not use any functions in the sequence that do not address the command-fifo as for example any EVE_mem...() function.
*/
void EVE_start_cmd_burst(eve_context_t * ctx)
{
	uint32_t ftAddress;

	ctx->cmd_burst = true;
	ftAddress = EVE_RAM_CMD + ctx->cmdOffset;

    if (_using_dma(ctx)) {
        while (ctx->EVE_dma_busy);
        ctx->EVE_dma_buffer[0] = (uint8_t)(ftAddress >> 16) | MEM_WRITE;
        ctx->EVE_dma_buffer[1] = (uint8_t)(ftAddress >> 8);
        ctx->EVE_dma_buffer[2] = (uint8_t)(ftAddress);
        ctx->EVE_dma_buffer_index = 3;
    } else {
        ctx->impl->cs_set(ctx, true);
        ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
        ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));	/* send middle address byte */
        ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));		/* send low address byte */
    }
}


void EVE_end_cmd_burst(eve_context_t * ctx)
{
	ctx->cmd_burst = false;

    if (_using_dma(ctx)) {
        ctx->impl->start_dma_transfer(ctx);
    } else {
        ctx->impl->cs_set(ctx, false);
        EVE_cmd_execute(ctx);
    }
}


/* Begin a co-processor command */
void EVE_start_cmd(eve_context_t * ctx, uint32_t command)
{
	uint32_t ftAddress;

	if(!ctx->cmd_burst)
	{
		ftAddress = EVE_RAM_CMD + ctx->cmdOffset;
		ctx->impl->cs_set(ctx, true);
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress >> 8));	/* send middle address byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(ftAddress));		/* send low address byte */

		ctx->impl->spi_transmit(ctx, (uint8_t)(command));		/* send data low byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(command >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(command >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(command >> 24));		/* Send data high byte */
	}
	else
	{
		_spi_transmit_async(ctx, (uint8_t)(command));		/* send data low byte */
		_spi_transmit_async(ctx, (uint8_t)(command >> 8));
		_spi_transmit_async(ctx, (uint8_t)(command >> 16));
		_spi_transmit_async(ctx, (uint8_t)(command >> 24));		/* Send data high byte */
	}

	EVE_inc_cmdoffset(ctx, 4);			/* update the command-ram pointer */
}


/* generic function for all commands that have no arguments and all display-list specific control words */
/*
 examples:
 EVE_cmd_dl(CMD_DLSTART);
 EVE_cmd_dl(CMD_SWAP);
 EVE_cmd_dl(CMD_SCREENSAVER);
 EVE_cmd_dl(LINE_WIDTH(1*16));
 EVE_cmd_dl(VERTEX2F(0,0));
 EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
*/
void EVE_cmd_dl(eve_context_t * ctx, uint32_t command)
{
	EVE_start_cmd(ctx, command);
	if(!ctx->cmd_burst)
	{
		ctx->impl->cs_set(ctx, false);
	}
}


/* Write a string to co-processor memory in context of a command: no chip-select, just plain SPI-transfers */
void EVE_write_string(eve_context_t * ctx, const char *text)
{
	uint8_t textindex = 0;
	uint8_t padding = 0;

	while(text[textindex] != 0)
	{
		if(ctx->cmd_burst)
		{
			_spi_transmit_async(ctx, text[textindex]);
		}
		else
		{
			ctx->impl->spi_transmit(ctx, text[textindex]);
		}
		textindex++;
	}

	/* we need to transmit at least one 0x00 byte and up to four if the string happens to be 4-byte aligned already */
	padding = textindex & 3;  /* 0, 1, 2 or 3 */
	padding = 4-padding; /* 4, 3, 2 or 1 */
	textindex += padding;

	while(padding > 0)
	{
		if(ctx->cmd_burst)
		{
			_spi_transmit_async(ctx, 0);
		}
		else
		{
			ctx->impl->spi_transmit(ctx, 0);
		}
		padding--;
	}

	EVE_inc_cmdoffset(ctx, textindex);
}


/* EVE3 FLASH functions */
#if defined (BT81X_ENABLE)

/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
/* write "num" bytes from *data to the external flash on a BT81x board at address ptr */
/* note: ptr must be 256 byte aligned, num must be a multiple of 256 */
void EVE_cmd_flashwrite(eve_context_t * ctx, uint32_t ptr, uint32_t num, const uint8_t *data)
{
	EVE_begin_cmd(ctx, CMD_FLASHWRITE);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 8);
	ctx->impl->cs_set(ctx, false);

	block_transfer(ctx, data, num);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
/* write "num" bytes from src in the external flash on a BT81x board to dest in RAM_G */
/* note: src must be 64-byte aligned, dest must be 4-byte aligned, num must be a multiple of 4 */
void EVE_cmd_flashread(eve_context_t * ctx, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_begin_cmd(ctx, CMD_FLASHREAD);

	ctx->impl->spi_transmit(ctx, (uint8_t)(dest));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(src));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 12);
	ctx->impl->cs_set(ctx, false);
	EVE_cmd_execute(ctx);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
/* write "num" bytes from src in RAM_G to to the external flash on a BT81x board at address dest */
/* note: dest must be 4096-byte aligned, src must be 4-byte aligned, num must be a multiple of 4096 */
void EVE_cmd_flashupdate(eve_context_t * ctx, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_begin_cmd(ctx, CMD_FLASHUPDATE);

	ctx->impl->spi_transmit(ctx, (uint8_t)(dest));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(src));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(src >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 12);
	ctx->impl->cs_set(ctx, false);
	EVE_cmd_execute(ctx);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
uint32_t EVE_cmd_flashfast(eve_context_t * ctx)
{
	uint16_t offset;

	EVE_begin_cmd(ctx, CMD_FLASHFAST);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	ctx->impl->spi_transmit(ctx, 0);
	offset = ctx->cmdOffset;
	EVE_inc_cmdoffset(ctx, 4);
	ctx->impl->cs_set(ctx, false);
	EVE_cmd_execute(ctx);
	return EVE_memRead32(ctx, EVE_RAM_CMD + offset);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
/* write "num" bytes from *data to the BT81x SPI interface */
/* note: raw direct access, not really useful for anything */
void EVE_cmd_flashspitx(eve_context_t * ctx, uint32_t num, const uint8_t *data)
{
	EVE_begin_cmd(ctx, CMD_FLASHSPITX);

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 4);
	ctx->impl->cs_set(ctx, false);

	block_transfer(ctx, data, num);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
/* write "num" bytes from the BT81x SPI interface dest in RAM_G */
/* note: raw direct access, not really useful for anything */
void EVE_cmd_flashspirx(eve_context_t * ctx, uint32_t dest, uint32_t num)
{
	EVE_begin_cmd(ctx, CMD_FLASHREAD);

	ctx->impl->spi_transmit(ctx, (uint8_t)(dest));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(dest >> 24));

	ctx->impl->spi_transmit(ctx, (uint8_t)(num));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

	EVE_inc_cmdoffset(ctx, 8);
	ctx->impl->cs_set(ctx, false);
	EVE_cmd_execute(ctx);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void EVE_cmd_flashsource(eve_context_t * ctx, uint32_t ptr)
{
	EVE_begin_cmd(ctx, CMD_FLASHSOURCE);

	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
	ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

	EVE_inc_cmdoffset(ctx, 4);
	ctx->impl->cs_set(ctx, false);
	EVE_cmd_execute(ctx);
}


/* switch the FLASH attached to a BT815/BT816 to full-speed mode, returns 0 for failing to do so */
uint8_t EVE_init_flash(eve_context_t * ctx)
{
	uint8_t timeout = 0;
	uint8_t status;
	uint32_t result;

	status = EVE_memRead8(ctx, REG_FLASH_STATUS); /* should be 0x02 - FLASH_STATUS_BASIC, power-up is done and the attached flash is detected */

	while(status == 0) /* FLASH_STATUS_INIT - we are somehow stll in init, give it a litte more time, this should never happen */
	{
		status = EVE_memRead8(ctx, REG_FLASH_STATUS);
		ctx->impl->delay_ms(1);
		timeout++;
		if(timeout > 100) /* 100ms and still in init, lets call quits now and exit with an error */
		{
			return 0;
		}
	}

	if(status == 1) /* FLASH_STATUS_DETACHED - no flash was found during init, no flash present or the detection failed, but have hope and let the BT81x have annother try */
	{
		EVE_cmd_dl(ctx, CMD_FLASHATTACH);
		EVE_cmd_execute(ctx);
		status = EVE_memRead8(ctx, REG_FLASH_STATUS);
		if(status != 2) /* still not in FLASH_STATUS_BASIC, time to give up */
		{
			return 0;
		}
	}

	if(status == 2) /* FLASH_STATUS_BASIC - flash detected and ready for action, lets move it up to FLASH_STATUS_FULL */
	{
		result = EVE_cmd_flashfast(ctx);

		if(result == 0) /* cmd_flashfast was succesful */
		{
			return 1;
		}
		else /* room for improvement, cmd_flashfast provided an error code but there is no way to return it without returning a value that is FALSE all the same */
		{
			return 0;
		}
	}

	if(status == 3) /* FLASH_STATUS_FULL - we are already there, why has this function been called? */
	{
		return 1;
	}

	return 0;
}
#endif


/* commands to draw graphics objects: */

void EVE_cmd_text(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text)
{
	EVE_start_cmd(ctx, CMD_TEXT);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(font));
		_spi_transmit_async(ctx, (uint8_t)(font >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(font));
		ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));
	}

	EVE_inc_cmdoffset(ctx, 8);
	EVE_write_string(ctx, text);

	if(!ctx->cmd_burst)
	{
		ctx->impl->cs_set(ctx, false);
	}
}


void EVE_cmd_button(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text)
{
	EVE_start_cmd(ctx, CMD_BUTTON);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w0));
		_spi_transmit_async(ctx, (uint8_t)(w0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(h0));
		_spi_transmit_async(ctx, (uint8_t)(h0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(font));
		_spi_transmit_async(ctx, (uint8_t)(font >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(font));
		ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));
	}

	EVE_inc_cmdoffset(ctx, 12);
	EVE_write_string(ctx, text);

	if(!ctx->cmd_burst)
	{
		ctx->impl->cs_set(ctx, false);
	}
}


/* draw a clock */
void EVE_cmd_clock(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs)
{
	EVE_start_cmd(ctx, CMD_CLOCK);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(r0));
		_spi_transmit_async(ctx, (uint8_t)(r0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(hours));
		_spi_transmit_async(ctx, (uint8_t)(hours >> 8));

		_spi_transmit_async(ctx, (uint8_t)(minutes));
		_spi_transmit_async(ctx, (uint8_t)(minutes >> 8));

		_spi_transmit_async(ctx, (uint8_t)(seconds));
		_spi_transmit_async(ctx, (uint8_t)(seconds >> 8));

		_spi_transmit_async(ctx, (uint8_t)(millisecs));
		_spi_transmit_async(ctx, (uint8_t)(millisecs >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(r0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(r0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(hours));
		ctx->impl->spi_transmit(ctx, (uint8_t)(hours >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(minutes));
		ctx->impl->spi_transmit(ctx, (uint8_t)(minutes >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(seconds));
		ctx->impl->spi_transmit(ctx, (uint8_t)(seconds >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(millisecs));
		ctx->impl->spi_transmit(ctx, (uint8_t)(millisecs >> 8));
		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_bgcolor(eve_context_t * ctx, uint32_t color)
{
	EVE_start_cmd(ctx, CMD_BGCOLOR);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(color));
		_spi_transmit_async(ctx, (uint8_t)(color >> 8));
		_spi_transmit_async(ctx, (uint8_t)(color >> 16));
		_spi_transmit_async(ctx, 0x00);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(color));
		ctx->impl->spi_transmit(ctx, (uint8_t)(color >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(color >> 16));
		ctx->impl->spi_transmit(ctx, 0x00);
		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


void EVE_cmd_fgcolor(eve_context_t * ctx, uint32_t color)
{
	EVE_start_cmd(ctx, CMD_FGCOLOR);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(color));
		_spi_transmit_async(ctx, (uint8_t)(color >> 8));
		_spi_transmit_async(ctx, (uint8_t)(color >> 16));
		_spi_transmit_async(ctx, 0x00);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(color));
		ctx->impl->spi_transmit(ctx, (uint8_t)(color >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(color >> 16));
		ctx->impl->spi_transmit(ctx, 0x00);
		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


void EVE_cmd_gradcolor(eve_context_t * ctx, uint32_t color)
{
	EVE_start_cmd(ctx, CMD_GRADCOLOR);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(color));
		_spi_transmit_async(ctx, (uint8_t)(color >> 8));
		_spi_transmit_async(ctx, (uint8_t)(color >> 16));
		_spi_transmit_async(ctx, 0x00);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(color));
		ctx->impl->spi_transmit(ctx, (uint8_t)(color >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(color >> 16));
		ctx->impl->spi_transmit(ctx, 0x00);
		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


void EVE_cmd_gauge(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
	EVE_start_cmd(ctx, CMD_GAUGE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(r0));
		_spi_transmit_async(ctx, (uint8_t)(r0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(major));
		_spi_transmit_async(ctx, (uint8_t)(major >> 8));

		_spi_transmit_async(ctx, (uint8_t)(minor));
		_spi_transmit_async(ctx, (uint8_t)(minor >> 8));

		_spi_transmit_async(ctx, (uint8_t)(val));
		_spi_transmit_async(ctx, (uint8_t)(val >> 8));

		_spi_transmit_async(ctx, (uint8_t)(range));
		_spi_transmit_async(ctx, (uint8_t)(range >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(r0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(r0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(major));
		ctx->impl->spi_transmit(ctx, (uint8_t)(major >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(minor));
		ctx->impl->spi_transmit(ctx, (uint8_t)(minor >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(val));
		ctx->impl->spi_transmit(ctx, (uint8_t)(val >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(range));
		ctx->impl->spi_transmit(ctx, (uint8_t)(range >> 8));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_gradient(eve_context_t * ctx, int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
	EVE_start_cmd(ctx, CMD_GRADIENT);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(rgb0));
		_spi_transmit_async(ctx, (uint8_t)(rgb0 >> 8));
		_spi_transmit_async(ctx, (uint8_t)(rgb0 >> 16));
		_spi_transmit_async(ctx, 0x00);

		_spi_transmit_async(ctx, (uint8_t)(x1));
		_spi_transmit_async(ctx, (uint8_t)(x1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y1));
		_spi_transmit_async(ctx, (uint8_t)(y1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(rgb1));
		_spi_transmit_async(ctx, (uint8_t)(rgb1 >> 8));
		_spi_transmit_async(ctx, (uint8_t)(rgb1 >> 16));
		_spi_transmit_async(ctx, 0x00);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(rgb0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(rgb0 >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(rgb0 >> 16));
		ctx->impl->spi_transmit(ctx, 0x00);

		ctx->impl->spi_transmit(ctx, (uint8_t)(x1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(rgb1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(rgb1 >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(rgb1 >> 16));
		ctx->impl->spi_transmit(ctx, 0x00);

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_keys(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text)
{
	EVE_start_cmd(ctx, CMD_KEYS);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w0));
		_spi_transmit_async(ctx, (uint8_t)(w0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(h0));
		_spi_transmit_async(ctx, (uint8_t)(h0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(font));
		_spi_transmit_async(ctx, (uint8_t)(font >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(font));
		ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));
	}

	EVE_inc_cmdoffset(ctx, 12);
	EVE_write_string(ctx, text);

	if(!ctx->cmd_burst)
	{
		ctx->impl->cs_set(ctx, false);
	}
}


void EVE_cmd_progress(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range)
{
	EVE_start_cmd(ctx, CMD_PROGRESS);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w0));
		_spi_transmit_async(ctx, (uint8_t)(w0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(h0));
		_spi_transmit_async(ctx, (uint8_t)(h0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(val));
		_spi_transmit_async(ctx, (uint8_t)(val >> 8));

		_spi_transmit_async(ctx, (uint8_t)(range));
		_spi_transmit_async(ctx, (uint8_t)(range >> 8));

		_spi_transmit_async(ctx, 0x00);	/* dummy byte for 4-byte alignment */
		_spi_transmit_async(ctx, 0x00); /* dummy byte for 4-byte alignment */
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(val));
		ctx->impl->spi_transmit(ctx, (uint8_t)(val >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(range));
		ctx->impl->spi_transmit(ctx, (uint8_t)(range >> 8));

		ctx->impl->spi_transmit(ctx, 0x00);	/* dummy byte for 4-byte alignment */
		ctx->impl->spi_transmit(ctx, 0x00); /* dummy byte for 4-byte alignment */

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);	/* update the command-ram pointer */
}


void EVE_cmd_scrollbar(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
	EVE_start_cmd(ctx, CMD_SCROLLBAR);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w0));
		_spi_transmit_async(ctx, (uint8_t)(w0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(h0));
		_spi_transmit_async(ctx, (uint8_t)(h0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(val));
		_spi_transmit_async(ctx, (uint8_t)(val >> 8));

		_spi_transmit_async(ctx, (uint8_t)(size));
		_spi_transmit_async(ctx, (uint8_t)(size >> 8));

		_spi_transmit_async(ctx, (uint8_t)(range));
		_spi_transmit_async(ctx, (uint8_t)(range >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(val));
		ctx->impl->spi_transmit(ctx, (uint8_t)(val >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(size));
		ctx->impl->spi_transmit(ctx, (uint8_t)(size >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(range));
		ctx->impl->spi_transmit(ctx, (uint8_t)(range >> 8));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_slider(eve_context_t * ctx, int16_t x1, int16_t y1, int16_t w1, int16_t h1, uint16_t options, uint16_t val, uint16_t range)
{
	EVE_start_cmd(ctx, CMD_SLIDER);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x1));
		_spi_transmit_async(ctx, (uint8_t)(x1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y1));
		_spi_transmit_async(ctx, (uint8_t)(y1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w1));
		_spi_transmit_async(ctx, (uint8_t)(w1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(h1));
		_spi_transmit_async(ctx, (uint8_t)(h1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(val));
		_spi_transmit_async(ctx, (uint8_t)(val >> 8));

		_spi_transmit_async(ctx, (uint8_t)(range));
		_spi_transmit_async(ctx, (uint8_t)(range >> 8));

		_spi_transmit_async(ctx, 0x00); /* dummy byte for 4-byte alignment */
		_spi_transmit_async(ctx, 0x00); /* dummy byte for 4-byte alignment */
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(h1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(h1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(val));
		ctx->impl->spi_transmit(ctx, (uint8_t)(val >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(range));
		ctx->impl->spi_transmit(ctx, (uint8_t)(range >> 8));

		ctx->impl->spi_transmit(ctx, 0x00); /* dummy byte for 4-byte alignment */
		ctx->impl->spi_transmit(ctx, 0x00); /* dummy byte for 4-byte alignment */

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_dial(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val)
{
	EVE_start_cmd(ctx, CMD_DIAL);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(r0));
		_spi_transmit_async(ctx, (uint8_t)(r0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(val));
		_spi_transmit_async(ctx, (uint8_t)(val >> 8));

		_spi_transmit_async(ctx, 0);
		_spi_transmit_async(ctx, 0);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(r0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(r0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(val));
		ctx->impl->spi_transmit(ctx, (uint8_t)(val >> 8));

		ctx->impl->spi_transmit(ctx, 0);
		ctx->impl->spi_transmit(ctx, 0);

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 12);
}


void EVE_cmd_toggle(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text)
{
	EVE_start_cmd(ctx, CMD_TOGGLE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w0));
		_spi_transmit_async(ctx, (uint8_t)(w0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(font));
		_spi_transmit_async(ctx, (uint8_t)(font >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(state));
		_spi_transmit_async(ctx, (uint8_t)(state >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(font));
		ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(state));
		ctx->impl->spi_transmit(ctx, (uint8_t)(state >> 8));
	}

	EVE_inc_cmdoffset(ctx, 12);
	EVE_write_string(ctx, text);

	if(!ctx->cmd_burst)
	{
		ctx->impl->cs_set(ctx, false);
	}
}


#if defined (FT81X_ENABLE)
void EVE_cmd_setbase(eve_context_t * ctx, uint32_t base)
{
	EVE_start_cmd(ctx, CMD_SETBASE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(base));		/* send data low byte */
		_spi_transmit_async(ctx, (uint8_t)(base >> 8));
		_spi_transmit_async(ctx, (uint8_t)(base >> 16));
		_spi_transmit_async(ctx, (uint8_t)(base >> 24));	/* send data high byte */
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(base));		/* send data low byte */
		ctx->impl->spi_transmit(ctx, (uint8_t)(base >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(base >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(base >> 24));	/* send data high byte */

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);	/* update the command-ram pointer */
}
#endif


#if defined (FT81X_ENABLE)
void EVE_cmd_setbitmap(eve_context_t * ctx, uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
	EVE_start_cmd(ctx, CMD_SETBITMAP);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(addr));
		_spi_transmit_async(ctx, (uint8_t)(addr >> 8));
		_spi_transmit_async(ctx, (uint8_t)(addr >> 16));
		_spi_transmit_async(ctx, (uint8_t)(addr >> 24));

		_spi_transmit_async(ctx, (uint8_t)(fmt));
		_spi_transmit_async(ctx, (uint8_t)(fmt>> 8));

		_spi_transmit_async(ctx, (uint8_t)(width));
		_spi_transmit_async(ctx, (uint8_t)(width >> 8));

		_spi_transmit_async(ctx, (uint8_t)(height));
		_spi_transmit_async(ctx, (uint8_t)(height >> 8));

		_spi_transmit_async(ctx, 0);
		_spi_transmit_async(ctx, 0);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(addr));
		ctx->impl->spi_transmit(ctx, (uint8_t)(addr >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(addr >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(addr >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(fmt));
		ctx->impl->spi_transmit(ctx, (uint8_t)(fmt>> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(width));
		ctx->impl->spi_transmit(ctx, (uint8_t)(width >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(height));
		ctx->impl->spi_transmit(ctx, (uint8_t)(height >> 8));

		ctx->impl->spi_transmit(ctx, 0);
		ctx->impl->spi_transmit(ctx, 0);

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 12);
}
#endif


void EVE_cmd_number(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number)
{
	EVE_start_cmd(ctx, CMD_NUMBER);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(font));
		_spi_transmit_async(ctx, (uint8_t)(font >> 8));

		_spi_transmit_async(ctx, (uint8_t)(options));
		_spi_transmit_async(ctx, (uint8_t)(options >> 8));

		_spi_transmit_async(ctx, (uint8_t)(number));
		_spi_transmit_async(ctx, (uint8_t)(number >> 8));
		_spi_transmit_async(ctx, (uint8_t)(number >> 16));
		_spi_transmit_async(ctx, (uint8_t)(number >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(font));
		ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(options));
		ctx->impl->spi_transmit(ctx, (uint8_t)(options >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(number));
		ctx->impl->spi_transmit(ctx, (uint8_t)(number >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(number >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(number >> 24));

		ctx->impl->cs_set(ctx, false);
	}


	EVE_inc_cmdoffset(ctx, 12);
}


void EVE_cmd_append(eve_context_t * ctx, uint32_t ptr, uint32_t num)
{
	EVE_start_cmd(ctx, CMD_APPEND);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ptr));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 24));

		_spi_transmit_async(ctx, (uint8_t)(num));
		_spi_transmit_async(ctx, (uint8_t)(num >> 8));
		_spi_transmit_async(ctx, (uint8_t)(num >> 16));
		_spi_transmit_async(ctx, (uint8_t)(num >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(num));
		ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}


/* commands for setting the bitmap transform matrix: */

void EVE_cmd_translate(eve_context_t * ctx, int32_t tx, int32_t ty)
{
	EVE_start_cmd(ctx, CMD_TRANSLATE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(tx));
		_spi_transmit_async(ctx, (uint8_t)(tx >> 8));
		_spi_transmit_async(ctx, (uint8_t)(tx >> 16));
		_spi_transmit_async(ctx, (uint8_t)(tx >> 24));

		_spi_transmit_async(ctx, (uint8_t)(ty));
		_spi_transmit_async(ctx, (uint8_t)(ty >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ty >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ty >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(tx));
		ctx->impl->spi_transmit(ctx, (uint8_t)(tx >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(tx >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(tx >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(ty));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ty >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ty >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ty >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}


void EVE_cmd_scale(eve_context_t * ctx, int32_t sx, int32_t sy)
{
	EVE_start_cmd(ctx, CMD_SCALE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(sx));
		_spi_transmit_async(ctx, (uint8_t)(sx >> 8));
		_spi_transmit_async(ctx, (uint8_t)(sx >> 16));
		_spi_transmit_async(ctx, (uint8_t)(sx >> 24));

		_spi_transmit_async(ctx, (uint8_t)(sy));
		_spi_transmit_async(ctx, (uint8_t)(sy >> 8));
		_spi_transmit_async(ctx, (uint8_t)(sy >> 16));
		_spi_transmit_async(ctx, (uint8_t)(sy >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(sx));
		ctx->impl->spi_transmit(ctx, (uint8_t)(sx >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(sx >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(sx >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(sy));
		ctx->impl->spi_transmit(ctx, (uint8_t)(sy >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(sy >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(sy >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}


void EVE_cmd_rotate(eve_context_t * ctx, int32_t ang)
{
	EVE_start_cmd(ctx, CMD_ROTATE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ang));
		_spi_transmit_async(ctx, (uint8_t)(ang >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ang >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ang >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ang));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ang >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ang >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ang >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


#if defined (BT81X_ENABLE)
void EVE_cmd_rotatearound(eve_context_t * ctx, int32_t x0, int32_t y0, int32_t angle, int32_t scale)
{
	EVE_start_cmd(ctx, CMD_ROTATEAROUND);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 16));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 24));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 16));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 24));

		_spi_transmit_async(ctx, (uint8_t)(angle));
		_spi_transmit_async(ctx, (uint8_t)(angle >> 8));
		_spi_transmit_async(ctx, (uint8_t)(angle >> 16));
		_spi_transmit_async(ctx, (uint8_t)(angle >> 24));

		_spi_transmit_async(ctx, (uint8_t)(scale));
		_spi_transmit_async(ctx, (uint8_t)(scale >> 8));
		_spi_transmit_async(ctx, (uint8_t)(scale >> 16));
		_spi_transmit_async(ctx, (uint8_t)(scale >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(angle));
		ctx->impl->spi_transmit(ctx, (uint8_t)(angle >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(angle >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(angle >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(scale));
		ctx->impl->spi_transmit(ctx, (uint8_t)(scale >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(scale >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(scale >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}
#endif


/*	the description in the programmers guide is strange for this function
	while it is named *get*matrix, parameters 'a' to 'f' are supplied to
	the function and described as "output parameter"
	best guess is that this one allows to setup the matrix coefficients manually
	instead automagically like with _translate, _scale and _rotate
	if this assumption is correct it rather should be named cmd_setupmatrix() */
void EVE_cmd_getmatrix(eve_context_t * ctx, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
	EVE_start_cmd(ctx, CMD_SETMATRIX);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(a));
		_spi_transmit_async(ctx, (uint8_t)(a >> 8));
		_spi_transmit_async(ctx, (uint8_t)(a >> 16));
		_spi_transmit_async(ctx, (uint8_t)(a >> 24));

		_spi_transmit_async(ctx, (uint8_t)(b));
		_spi_transmit_async(ctx, (uint8_t)(b >> 8));
		_spi_transmit_async(ctx, (uint8_t)(b >> 16));
		_spi_transmit_async(ctx, (uint8_t)(b >> 24));

		_spi_transmit_async(ctx, (uint8_t)(c));
		_spi_transmit_async(ctx, (uint8_t)(c >> 8));
		_spi_transmit_async(ctx, (uint8_t)(c >> 16));
		_spi_transmit_async(ctx, (uint8_t)(c >> 24));

		_spi_transmit_async(ctx, (uint8_t)(d));
		_spi_transmit_async(ctx, (uint8_t)(d >> 8));
		_spi_transmit_async(ctx, (uint8_t)(d >> 16));
		_spi_transmit_async(ctx, (uint8_t)(d >> 24));

		_spi_transmit_async(ctx, (uint8_t)(e));
		_spi_transmit_async(ctx, (uint8_t)(e >> 8));
		_spi_transmit_async(ctx, (uint8_t)(e >> 16));
		_spi_transmit_async(ctx, (uint8_t)(e >> 24));

		_spi_transmit_async(ctx, (uint8_t)(f));
		_spi_transmit_async(ctx, (uint8_t)(f >> 8));
		_spi_transmit_async(ctx, (uint8_t)(f >> 16));
		_spi_transmit_async(ctx, (uint8_t)(f >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(a));
		ctx->impl->spi_transmit(ctx, (uint8_t)(a >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(a >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(a >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(b));
		ctx->impl->spi_transmit(ctx, (uint8_t)(b >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(b >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(b >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(c));
		ctx->impl->spi_transmit(ctx, (uint8_t)(c >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(c >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(c >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(d));
		ctx->impl->spi_transmit(ctx, (uint8_t)(d >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(d >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(d >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(e));
		ctx->impl->spi_transmit(ctx, (uint8_t)(e >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(e >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(e >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(f));
		ctx->impl->spi_transmit(ctx, (uint8_t)(f >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(f >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(f >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 24);
}


/* other commands: */

void EVE_cmd_calibrate(eve_context_t * ctx)
{
	EVE_start_cmd(ctx, CMD_CALIBRATE);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, 0);
		_spi_transmit_async(ctx, 0);
		_spi_transmit_async(ctx, 0);
		_spi_transmit_async(ctx, 0);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, 0);
		ctx->impl->spi_transmit(ctx, 0);
		ctx->impl->spi_transmit(ctx, 0);
		ctx->impl->spi_transmit(ctx, 0);

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


#if defined (FT81X_ENABLE)
void EVE_cmd_romfont(eve_context_t * ctx, uint32_t font, uint32_t romslot)
{
	EVE_start_cmd(ctx, CMD_ROMFONT);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(font));
		_spi_transmit_async(ctx, (uint8_t)(font >> 8));
		_spi_transmit_async(ctx, 0x00);
		_spi_transmit_async(ctx, 0x00);

		_spi_transmit_async(ctx, (uint8_t)(romslot));
		_spi_transmit_async(ctx, (uint8_t)(romslot >> 8));
		_spi_transmit_async(ctx, 0x00);
		_spi_transmit_async(ctx, 0x00);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(font));
		ctx->impl->spi_transmit(ctx, (uint8_t)(font >> 8));
		ctx->impl->spi_transmit(ctx, 0x00);
		ctx->impl->spi_transmit(ctx, 0x00);

		ctx->impl->spi_transmit(ctx, (uint8_t)(romslot));
		ctx->impl->spi_transmit(ctx, (uint8_t)(romslot >> 8));
		ctx->impl->spi_transmit(ctx, 0x00);
		ctx->impl->spi_transmit(ctx, 0x00);

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}
#endif


#if defined (FT81X_ENABLE)
void EVE_cmd_setscratch(eve_context_t * ctx, uint32_t handle)
{
	EVE_start_cmd(ctx, CMD_SETSCRATCH);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(handle));
		_spi_transmit_async(ctx, (uint8_t)(handle >> 8));
		_spi_transmit_async(ctx, (uint8_t)(handle >> 16));
		_spi_transmit_async(ctx, (uint8_t)(handle >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(handle));
		ctx->impl->spi_transmit(ctx, (uint8_t)(handle >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(handle >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(handle >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}
#endif


void EVE_cmd_sketch(eve_context_t * ctx, int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format)
{
	EVE_start_cmd(ctx, CMD_SKETCH);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(w0));
		_spi_transmit_async(ctx, (uint8_t)(w0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(h0));
		_spi_transmit_async(ctx, (uint8_t)(h0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(ptr));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 24));

		_spi_transmit_async(ctx, (uint8_t)(format));
		_spi_transmit_async(ctx, (uint8_t)(format >> 8));

		_spi_transmit_async(ctx, 0);
		_spi_transmit_async(ctx, 0);
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(w0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(w0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(h0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(h0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(format));
		ctx->impl->spi_transmit(ctx, (uint8_t)(format >> 8));

		ctx->impl->spi_transmit(ctx, 0);
		ctx->impl->spi_transmit(ctx, 0);

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_spinner(eve_context_t * ctx, int16_t x0, int16_t y0, uint16_t style, uint16_t scale)
{
	EVE_start_cmd(ctx, CMD_SPINNER);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(style));
		_spi_transmit_async(ctx, (uint8_t)(style >> 8));

		_spi_transmit_async(ctx, (uint8_t)(scale));
		_spi_transmit_async(ctx, (uint8_t)(scale >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(style));
		ctx->impl->spi_transmit(ctx, (uint8_t)(style >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(scale));
		ctx->impl->spi_transmit(ctx, (uint8_t)(scale >> 8));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}


/* various commands new for EVE3 */
#if defined (BT81X_ENABLE)

void EVE_cmd_animstart(eve_context_t * ctx, int32_t ch, uint32_t aoptr, uint32_t loop)
{
	EVE_start_cmd(ctx, CMD_ANIMSTART);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ch));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 24));

		_spi_transmit_async(ctx, (uint8_t)(aoptr));
		_spi_transmit_async(ctx, (uint8_t)(aoptr >> 8));
		_spi_transmit_async(ctx, (uint8_t)(aoptr >> 16));
		_spi_transmit_async(ctx, (uint8_t)(aoptr >> 24));

		_spi_transmit_async(ctx, (uint8_t)(loop));
		_spi_transmit_async(ctx, (uint8_t)(loop >> 8));
		_spi_transmit_async(ctx, (uint8_t)(loop >> 16));
		_spi_transmit_async(ctx, (uint8_t)(loop >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr));
		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(loop));
		ctx->impl->spi_transmit(ctx, (uint8_t)(loop >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(loop >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(loop >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 12);
}


void EVE_cmd_animstop(eve_context_t * ctx, int32_t ch)
{
	EVE_start_cmd(ctx, CMD_ANIMSTOP);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ch));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


void EVE_cmd_animxy(eve_context_t * ctx, int32_t ch, int16_t x0, int16_t y0)
{
	EVE_start_cmd(ctx, CMD_ANIMXY);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ch));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 24));

		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}


void EVE_cmd_animdraw(eve_context_t * ctx, int32_t ch)
{
	EVE_start_cmd(ctx, CMD_ANIMDRAW);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ch));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ch >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ch >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


void EVE_cmd_animframe(eve_context_t * ctx, int16_t x0, int16_t y0, uint32_t aoptr, uint32_t frame)
{
	EVE_start_cmd(ctx, CMD_ANIMFRAME);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(aoptr));
		_spi_transmit_async(ctx, (uint8_t)(aoptr >> 8));
		_spi_transmit_async(ctx, (uint8_t)(aoptr >> 16));
		_spi_transmit_async(ctx, (uint8_t)(aoptr >> 24));

		_spi_transmit_async(ctx, (uint8_t)(frame));
		_spi_transmit_async(ctx, (uint8_t)(frame >> 8));
		_spi_transmit_async(ctx, (uint8_t)(frame >> 16));
		_spi_transmit_async(ctx, (uint8_t)(frame >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr));
		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(aoptr >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(frame));
		ctx->impl->spi_transmit(ctx, (uint8_t)(frame >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(frame >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(frame >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 12);
}


void EVE_cmd_gradienta(eve_context_t * ctx, int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1)
{
	EVE_start_cmd(ctx, CMD_GRADIENTA);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(x0));
		_spi_transmit_async(ctx, (uint8_t)(x0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y0));
		_spi_transmit_async(ctx, (uint8_t)(y0 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(argb0));
		_spi_transmit_async(ctx, (uint8_t)(argb0 >> 8));
		_spi_transmit_async(ctx, (uint8_t)(argb0 >> 16));
		_spi_transmit_async(ctx, (uint8_t)(argb0 >> 24));

		_spi_transmit_async(ctx, (uint8_t)(x1));
		_spi_transmit_async(ctx, (uint8_t)(x1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(y1));
		_spi_transmit_async(ctx, (uint8_t)(y1 >> 8));

		_spi_transmit_async(ctx, (uint8_t)(argb1));
		_spi_transmit_async(ctx, (uint8_t)(argb1 >> 8));
		_spi_transmit_async(ctx, (uint8_t)(argb1 >> 16));
		_spi_transmit_async(ctx, (uint8_t)(argb1 >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y0 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(argb0));
		ctx->impl->spi_transmit(ctx, (uint8_t)(argb0 >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(argb0 >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(argb0 >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(x1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(x1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(y1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(y1 >> 8));

		ctx->impl->spi_transmit(ctx, (uint8_t)(argb1));
		ctx->impl->spi_transmit(ctx, (uint8_t)(argb1 >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(argb1 >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(argb1 >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_fillwidth(eve_context_t * ctx, uint32_t s)
{
	EVE_start_cmd(ctx, CMD_FILLWIDTH);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(s));
		_spi_transmit_async(ctx, (uint8_t)(s >> 8));
		_spi_transmit_async(ctx, (uint8_t)(s >> 16));
		_spi_transmit_async(ctx, (uint8_t)(s >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(s));
		ctx->impl->spi_transmit(ctx, (uint8_t)(s >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(s >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(s >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 4);
}


void EVE_cmd_appendf(eve_context_t * ctx, uint32_t ptr, uint32_t num)
{
	EVE_start_cmd(ctx, CMD_APPENDF);

	if(ctx->cmd_burst)
	{
		_spi_transmit_async(ctx, (uint8_t)(ptr));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 8));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 16));
		_spi_transmit_async(ctx, (uint8_t)(ptr >> 24));

		_spi_transmit_async(ctx, (uint8_t)(num));
		_spi_transmit_async(ctx, (uint8_t)(num >> 8));
		_spi_transmit_async(ctx, (uint8_t)(num >> 16));
		_spi_transmit_async(ctx, (uint8_t)(num >> 24));
	}
	else
	{
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(ptr >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(num));
		ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(num >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 8);
}


#endif


/* meta-commands, sequences of several display-list entries condensed into simpler to use functions at the price of some overhead */

void EVE_cmd_point(eve_context_t * ctx, int16_t x0, int16_t y0, uint16_t size)
{
	uint32_t calc;

	EVE_start_cmd(ctx, (DL_BEGIN | EVE_POINTS));

	if(ctx->cmd_burst)
	{
		calc = POINT_SIZE(size*16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x0 * 16, y0 * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		_spi_transmit_async(ctx, (uint8_t)(DL_END));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 8));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 16));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 24));
	}
	else
	{
		calc = POINT_SIZE(size*16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x0 * 16, y0 * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 12);
}


void EVE_cmd_line(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t width)
{
	uint32_t calc;

	EVE_start_cmd(ctx, (DL_BEGIN | EVE_LINES));

	if(ctx->cmd_burst)
	{
		calc = LINE_WIDTH(width * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x0 * 16, y0 * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x1 * 16, y1 * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		_spi_transmit_async(ctx, (uint8_t)(DL_END));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 8));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 16));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 24));
	}
	else
	{
		calc = LINE_WIDTH(width * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x0 * 16, y0 * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x1 * 16, y1 * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}


void EVE_cmd_rect(eve_context_t * ctx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t corner)
{
	uint32_t calc;

	EVE_start_cmd(ctx, (DL_BEGIN | EVE_RECTS));

	if(ctx->cmd_burst)
	{
		calc = LINE_WIDTH(corner * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x0 * 16, y0 * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x1 * 16, y1 * 16);
		_spi_transmit_async(ctx, (uint8_t)(calc));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 8));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 16));
		_spi_transmit_async(ctx, (uint8_t)(calc >> 24));

		_spi_transmit_async(ctx, (uint8_t)(DL_END));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 8));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 16));
		_spi_transmit_async(ctx, (uint8_t)(DL_END >> 24));
	}
	else
	{
		calc = LINE_WIDTH(corner * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x0 * 16, y0 * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		calc = VERTEX2F(x1 * 16, y1 * 16);
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(calc >> 24));

		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 8));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 16));
		ctx->impl->spi_transmit(ctx, (uint8_t)(DL_END >> 24));

		ctx->impl->cs_set(ctx, false);
	}

	EVE_inc_cmdoffset(ctx, 16);
}

void EVE_dma_complete(eve_context_t * ctx)
{
    if (!ctx->dma_flushing) {
        ctx->impl->cs_set(ctx, false);
    }
}

static bool _wait_while_busy(eve_context_t * ctx, uint32_t timeout_us)
{
    bool const do_timeouts = ctx->impl->timeout_start && ctx->impl->timeout_test;
    void * const timeout_state = do_timeouts ?
        ctx->impl->timeout_start(ctx, timeout_us) : NULL;

    while (!do_timeouts || !ctx->impl->timeout_test(ctx, timeout_state, timeout_us)) {
        if (!EVE_busy(ctx)) {
            return true;
        }
    }

    return false;
}

static bool _using_dma(eve_context_t * ctx)
{
    return ctx->use_dma && ctx->impl->start_dma_transfer;
}

static void _spi_transmit_async(eve_context_t * ctx, uint8_t v)
{
    if (!_using_dma(ctx)) {
        ctx->impl->spi_transmit(ctx, v);
        return;
    }

    if (ctx->EVE_dma_buffer_index >= ctx->EVE_dma_buffer_len) {
        ctx->dma_flushing = true;
        ctx->impl->cs_set(ctx, true);
        ctx->impl->start_dma_transfer(ctx);
        while (ctx->EVE_dma_busy);
        ctx->dma_flushing = false;
        ctx->EVE_dma_buffer_index = 0;
    }

    ctx->EVE_dma_buffer[ctx->EVE_dma_buffer_index] = v;
    ctx->EVE_dma_buffer_index += 1;
}
