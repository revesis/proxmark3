//-----------------------------------------------------------------------------
// Copyright (C) 2010 iZsh <izsh at fail0verflow.com>
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// Hardware commands
// low-level hardware control
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cmdparser.h"    // command_t
#include "cliparser.h"
#include "comms.h"
#include "usart_defs.h"
#include "ui.h"
#include "cmdhw.h"
#include "cmddata.h"
#include "commonutil.h"
#include "pm3_cmd.h"
#include "pmflash.h"      // rdv40validation_t
#include "cmdflashmem.h"  // get_signature..

static int CmdHelp(const char *Cmd);

static void lookupChipID(uint32_t iChipID, uint32_t mem_used) {
    char asBuff[120];
    memset(asBuff, 0, sizeof(asBuff));
    uint32_t mem_avail = 0;
    PrintAndLogEx(NORMAL, "\n [ " _YELLOW_("Hardware") " ]");

    switch (iChipID) {
        case 0x270B0A40:
            sprintf(asBuff, "AT91SAM7S512 Rev A");
            break;
        case 0x270B0A4F:
            sprintf(asBuff, "AT91SAM7S512 Rev B");
            break;
        case 0x270D0940:
            sprintf(asBuff, "AT91SAM7S256 Rev A");
            break;
        case 0x270B0941:
            sprintf(asBuff, "AT91SAM7S256 Rev B");
            break;
        case 0x270B0942:
            sprintf(asBuff, "AT91SAM7S256 Rev C");
            break;
        case 0x270B0943:
            sprintf(asBuff, "AT91SAM7S256 Rev D");
            break;
        case 0x270C0740:
            sprintf(asBuff, "AT91SAM7S128 Rev A");
            break;
        case 0x270A0741:
            sprintf(asBuff, "AT91SAM7S128 Rev B");
            break;
        case 0x270A0742:
            sprintf(asBuff, "AT91SAM7S128 Rev C");
            break;
        case 0x270A0743:
            sprintf(asBuff, "AT91SAM7S128 Rev D");
            break;
        case 0x27090540:
            sprintf(asBuff, "AT91SAM7S64 Rev A");
            break;
        case 0x27090543:
            sprintf(asBuff, "AT91SAM7S64 Rev B");
            break;
        case 0x27090544:
            sprintf(asBuff, "AT91SAM7S64 Rev C");
            break;
        case 0x27080342:
            sprintf(asBuff, "AT91SAM7S321 Rev A");
            break;
        case 0x27080340:
            sprintf(asBuff, "AT91SAM7S32 Rev A");
            break;
        case 0x27080341:
            sprintf(asBuff, "AT91SAM7S32 Rev B");
            break;
        case 0x27050241:
            sprintf(asBuff, "AT9SAM7S161 Rev A");
            break;
        case 0x27050240:
            sprintf(asBuff, "AT91SAM7S16 Rev A");
            break;
    }
    PrintAndLogEx(NORMAL, "  --= uC: " _YELLOW_("%s"), asBuff);

    switch ((iChipID & 0xE0) >> 5) {
        case 1:
            sprintf(asBuff, "ARM946ES");
            break;
        case 2:
            sprintf(asBuff, "ARM7TDMI");
            break;
        case 4:
            sprintf(asBuff, "ARM920T");
            break;
        case 5:
            sprintf(asBuff, "ARM926EJS");
            break;
    }
    PrintAndLogEx(NORMAL, "  --= Embedded Processor: %s", asBuff);

    switch ((iChipID & 0xF0000) >> 16) {
        case 1:
            sprintf(asBuff, "1K bytes");
            break;
        case 2:
            sprintf(asBuff, "2K bytes");
            break;
        case 3:
            sprintf(asBuff, "6K bytes");
            break;
        case 4:
            sprintf(asBuff, "112K bytes");
            break;
        case 5:
            sprintf(asBuff, "4K bytes");
            break;
        case 6:
            sprintf(asBuff, "80K bytes");
            break;
        case 7:
            sprintf(asBuff, "160K bytes");
            break;
        case 8:
            sprintf(asBuff, "8K bytes");
            break;
        case 9:
            sprintf(asBuff, "16K bytes");
            break;
        case 10:
            sprintf(asBuff, "32K bytes");
            break;
        case 11:
            sprintf(asBuff, "64K bytes");
            break;
        case 12:
            sprintf(asBuff, "128K bytes");
            break;
        case 13:
            sprintf(asBuff, "256K bytes");
            break;
        case 14:
            sprintf(asBuff, "96K bytes");
            break;
        case 15:
            sprintf(asBuff, "512K bytes");
            break;
    }
    PrintAndLogEx(NORMAL, "  --= Internal SRAM size: %s", asBuff);

    switch ((iChipID & 0xFF00000) >> 20) {
        case 0x19:
            sprintf(asBuff, "AT91SAM9xx Series");
            break;
        case 0x29:
            sprintf(asBuff, "AT91SAM9XExx Series");
            break;
        case 0x34:
            sprintf(asBuff, "AT91x34 Series");
            break;
        case 0x37:
            sprintf(asBuff, "CAP7 Series");
            break;
        case 0x39:
            sprintf(asBuff, "CAP9 Series");
            break;
        case 0x3B:
            sprintf(asBuff, "CAP11 Series");
            break;
        case 0x40:
            sprintf(asBuff, "AT91x40 Series");
            break;
        case 0x42:
            sprintf(asBuff, "AT91x42 Series");
            break;
        case 0x55:
            sprintf(asBuff, "AT91x55 Series");
            break;
        case 0x60:
            sprintf(asBuff, "AT91SAM7Axx Series");
            break;
        case 0x61:
            sprintf(asBuff, "AT91SAM7AQxx Series");
            break;
        case 0x63:
            sprintf(asBuff, "AT91x63 Series");
            break;
        case 0x70:
            sprintf(asBuff, "AT91SAM7Sxx Series");
            break;
        case 0x71:
            sprintf(asBuff, "AT91SAM7XCxx Series");
            break;
        case 0x72:
            sprintf(asBuff, "AT91SAM7SExx Series");
            break;
        case 0x73:
            sprintf(asBuff, "AT91SAM7Lxx Series");
            break;
        case 0x75:
            sprintf(asBuff, "AT91SAM7Xxx Series");
            break;
        case 0x92:
            sprintf(asBuff, "AT91x92 Series");
            break;
        case 0xF0:
            sprintf(asBuff, "AT75Cxx Series");
            break;
    }
    PrintAndLogEx(NORMAL, "  --= Architecture identifier: %s", asBuff);

    switch ((iChipID & 0x70000000) >> 28) {
        case 0:
            sprintf(asBuff, "ROM");
            break;
        case 1:
            sprintf(asBuff, "ROMless or on-chip Flash");
            break;
        case 2:
            sprintf(asBuff, "Embedded flash memory");
            break;
        case 3:
            sprintf(asBuff, "ROM and Embedded flash memory\nNVPSIZ is ROM size\nNVPSIZ2 is Flash size");
            break;
        case 4:
            sprintf(asBuff, "SRAM emulating ROM");
            break;
    }
    switch ((iChipID & 0xF00) >> 8) {
        case 0:
            mem_avail = 0;
            break;
        case 1:
            mem_avail = 8;
            break;
        case 2:
            mem_avail = 16;
            break;
        case 3:
            mem_avail = 32;
            break;
        case 5:
            mem_avail = 64;
            break;
        case 7:
            mem_avail = 128;
            break;
        case 9:
            mem_avail = 256;
            break;
        case 10:
            mem_avail = 512;
            break;
        case 12:
            mem_avail = 1024;
            break;
        case 14:
            mem_avail = 2048;
            break;
    }

    PrintAndLogEx(NORMAL, "  --= %s " _YELLOW_("%uK") " bytes ( " _YELLOW_("%2.0f%%") " used )"
                  , asBuff
                  , mem_avail
                  , mem_avail == 0 ? 0.0f : (float)mem_used / (mem_avail * 1024) * 100
                 );

    /*
    switch ((iChipID & 0xF000) >> 12) {
        case 0:
            sprintf(asBuff, "None");
            break;
        case 1:
            sprintf(asBuff, "8K bytes");
            break;
        case 2:
            sprintf(asBuff, "16K bytes");
            break;
        case 3:
            sprintf(asBuff, "32K bytes");
            break;
        case 5:
            sprintf(asBuff, "64K bytes");
            break;
        case 7:
            sprintf(asBuff, "128K bytes");
            break;
        case 9:
            sprintf(asBuff, "256K bytes");
            break;
        case 10:
            sprintf(asBuff, "512K bytes");
            break;
        case 12:
            sprintf(asBuff, "1024K bytes");
            break;
        case 14:
            sprintf(asBuff, "2048K bytes");
            break;
    }
    PrintAndLogEx(NORMAL, "  --= Second nonvolatile program memory size: %s", asBuff);
    */
}

static int CmdDbg(const char *Cmd) {

    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw dbg",
                  "Set device side debug level output.\n"
                  "Note: option -4, this option may cause malfunction itself",
                  "hw dbg -1\n"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_lit0("0", NULL, "no debug messages"),
        arg_lit0("1", NULL, "error messages"),
        arg_lit0("2", NULL, "plus information messages"),
        arg_lit0("3", NULL, "plus debug messages"),
        arg_lit0("4", NULL, "print even debug messages in timing critical functions"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    bool lv0 = arg_get_lit(ctx, 1);
    bool lv1 = arg_get_lit(ctx, 2);
    bool lv2 = arg_get_lit(ctx, 3);
    bool lv3 = arg_get_lit(ctx, 4);
    bool lv4 = arg_get_lit(ctx, 5);
    CLIParserFree(ctx);

    if ((lv0 + lv1 + lv2 + lv3 + lv4) > 1) {
        PrintAndLogEx(INFO, "Can only set one debug level");
        return PM3_EINVARG;
    }

    uint8_t dbg = 0;
    if (lv0)
        dbg = 0;
    else if (lv1)
        dbg = 1;
    else if (lv2)
        dbg = 2;
    else if (lv3)
        dbg = 3;
    else if (lv4)
        dbg = 4;

    SendCommandNG(CMD_SET_DBGMODE, &dbg, sizeof(dbg));
    return PM3_SUCCESS;
}

static int CmdDetectReader(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw detectreader",
                  "Start to detect presences of reader field",
                  "hw detectreader -L\n"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_lit0("L", "LF", "detect low frequence 125/134 kHz"),
        arg_lit0("H", "HF", "detect high frequence 13.56 MHZ"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    bool lf = arg_get_lit(ctx, 1);
    bool hf = arg_get_lit(ctx, 2);
    CLIParserFree(ctx);

    if ((lf + hf) > 1) {
        PrintAndLogEx(INFO, "Can only set one frequence");
        return PM3_EINVARG;
    }

    uint8_t arg = 0;
    if (lf)
        arg = 1;
    else if (hf)
        arg = 2;

    PrintAndLogEx(INFO, "press pm3 button to change modes and finally exit");
    clearCommandBuffer();
    SendCommandNG(CMD_LISTEN_READER_FIELD, (uint8_t *)&arg, sizeof(arg));
    return PM3_SUCCESS;
}

// ## FPGA Control
static int CmdFPGAOff(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw fpgaoff",
                  "Turn of fpga and antenna field",
                  "hw fpgaoff\n"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);

    clearCommandBuffer();
    SendCommandNG(CMD_FPGA_MAJOR_MODE_OFF, NULL, 0);
    return PM3_SUCCESS;
}

static int CmdLCD(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw lcd",
                  "Send command/data to LCD",
                  "hw lcd -r AA -c 03    -> sends 0xAA three times"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_int1("r", "raw", "<hex>",  "data "),
        arg_int1("c", "cnt", "<dec>",  "number of times to send"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);

    int r_len = 0;
    uint8_t raw[1] = {0};
    CLIGetHexWithReturn(ctx, 1, raw, &r_len);
    int j = arg_get_int(ctx, 2);
    if (j < 1) {
        PrintAndLogEx(WARNING, "Count must be larger than zero");
        return PM3_EINVARG;
    }

    while (j--) {
        clearCommandBuffer();
        SendCommandMIX(CMD_LCD, raw[0], 0, 0, NULL, 0);
    }
    return PM3_SUCCESS;
}

static int CmdLCDReset(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw lcdreset",
                  "Hardware reset LCD",
                  "hw lcdreset\n"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);
    clearCommandBuffer();
    SendCommandNG(CMD_LCD_RESET, NULL, 0);
    return PM3_SUCCESS;
}

static int CmdReadmem(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw readmem",
                  "Read memory at decimal address from ARM chip flash.",
                  "hw readmem -a 10000"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_u64_1("a", "adr", "<dec>", "address to read"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    uint32_t address = arg_get_u32(ctx, 1);
    CLIParserFree(ctx);
    clearCommandBuffer();
    SendCommandNG(CMD_READ_MEM, (uint8_t *)&address, sizeof(address));
    return PM3_SUCCESS;
}

static int CmdReset(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw reset",
                  "Reset the Proxmark3 device.",
                  "hw reset"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);
    clearCommandBuffer();
    SendCommandNG(CMD_HARDWARE_RESET, NULL, 0);
    PrintAndLogEx(INFO, "Proxmark3 has been reset.");
    return PM3_SUCCESS;
}

/*
 * Sets the divisor for LF frequency clock: lets the user choose any LF frequency below
 * 600kHz.
 */
static int CmdSetDivisor(const char *Cmd) {

    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw setlfdivisor",
                  "Drive LF antenna at 12 MHz / (divisor + 1).",
                  "hw setlfdivisor -d 88"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_u64_1("d", "div", "<dec>", "19 - 255 divisor value (def 95)"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    uint8_t arg = arg_get_u32_def(ctx, 1, 95);
    CLIParserFree(ctx);

    if (arg < 19) {
        PrintAndLogEx(ERR, "Divisor must be between" _YELLOW_("19") " and " _YELLOW_("255"));
        return PM3_EINVARG;
    }
    // 12 000 000 (12MHz)
    clearCommandBuffer();
    SendCommandNG(CMD_LF_SET_DIVISOR, (uint8_t *)&arg, sizeof(arg));
    PrintAndLogEx(SUCCESS, "Divisor set, expected " _YELLOW_("%.1f")" kHz", ((double)12000 / (arg + 1)));
    return PM3_SUCCESS;
}

static int CmdSetMux(const char *Cmd) {

    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw setmux",
                  "Set the ADC mux to a specific value",
                  "hw setmux --hipkd    -> set HIGH PEAK\n"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_lit0(NULL, "lopkd", "low peak"),
        arg_lit0(NULL, "loraw", "low raw"),
        arg_lit0(NULL, "hipkd", "high peak"),
        arg_lit0(NULL, "hiraw", "high raw"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    bool lopkd = arg_get_lit(ctx, 1);
    bool loraw = arg_get_lit(ctx, 2);
    bool hipkd = arg_get_lit(ctx, 3);
    bool hiraw = arg_get_lit(ctx, 4);
    CLIParserFree(ctx);

    if ((lopkd + loraw + hipkd + hiraw) > 1) {
        PrintAndLogEx(INFO, "Can only set one mux");
        return PM3_EINVARG;
    }

#ifdef WITH_FPC_USART
    if (loraw || hiraw) {
        PrintAndLogEx(INFO, "this ADC mux option is unavailable on RDV4 compiled with FPC USART");
        return PM3_EINVARG;
    }
#endif

    uint8_t arg = 0;
    if (lopkd)
        arg = 0;
    else if (loraw)
        arg = 1;
    else if (hipkd)
        arg = 2;
    else if (hiraw)
        arg = 3;

    clearCommandBuffer();
    SendCommandNG(CMD_SET_ADC_MUX, (uint8_t *)&arg, sizeof(arg));
    return PM3_SUCCESS;
}

static int CmdStandalone(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw standalone",
                  "Start standalone mode",
                  "hw standalone       -> start \n"
                  "hw standalone -a 1  -> start and send arg 1"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_u64_0("a", "arg", "<dec>", "argument byte"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    uint8_t arg = arg_get_u32(ctx, 1);
    CLIParserFree(ctx);
    clearCommandBuffer();
    SendCommandNG(CMD_STANDALONE, (uint8_t *)&arg, sizeof(arg));
    return PM3_SUCCESS;
}

static int CmdTune(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw tune",
                  "Measure antenna tuning",
                  "hw tune"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);
    return CmdTuneSamples(Cmd);
}

static int CmdVersion(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw version",
                  "Show version information about the client and the connected Proxmark3",
                  "hw version"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);
    pm3_version(true, false);
    return PM3_SUCCESS;
}

static int CmdStatus(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw status",
                  "Show runtime status information about the connected Proxmark3",
                  "hw status"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);
    clearCommandBuffer();
    PacketResponseNG resp;
    SendCommandNG(CMD_STATUS, NULL, 0);
    if (WaitForResponseTimeout(CMD_STATUS, &resp, 2000) == false) {
        PrintAndLogEx(WARNING, "Status command timeout. Communication speed test timed out");
        return PM3_ETIMEOUT;
    }
    return PM3_SUCCESS;
}

int handle_tearoff(tearoff_params_t *params, bool verbose) {

    if (params == NULL)
        return PM3_EINVARG;

    clearCommandBuffer();
    SendCommandNG(CMD_SET_TEAROFF, (uint8_t *)params, sizeof(tearoff_params_t));
    PacketResponseNG resp;
    if (WaitForResponseTimeout(CMD_SET_TEAROFF, &resp, 500) == false) {
        PrintAndLogEx(WARNING, "Tear-off command timeout.");
        return PM3_ETIMEOUT;
    }

    if (resp.status == PM3_SUCCESS) {
        if (params->delay_us > 0 && verbose)
            PrintAndLogEx(INFO, "Tear-off hook configured with delay of " _GREEN_("%i us"), params->delay_us);

        if (params->on && verbose)
            PrintAndLogEx(INFO, "Tear-off hook " _GREEN_("enabled"));

        if (params->off && verbose)
            PrintAndLogEx(INFO, "Tear-off hook " _RED_("disabled"));
    } else if (verbose)
        PrintAndLogEx(WARNING, "Tear-off command failed.");
    return resp.status;
}

static int CmdTearoff(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw tearoff",
                  "Configure a tear-off hook for the next write command supporting tear-off\n"
                  "After having been triggered by a write command, the tear-off hook is deactivated\n"
                  "Delay (in us) must be between 1 and 43000 (43ms). Precision is about 1/3us.",
                  "hw tearoff --delay 1200 --> define delay of 1200us\n"
                  "hw tearoff --on --> (re)activate a previously defined delay\n"
                  "hw tearoff --off --> deactivate a previously activated but not yet triggered hook\n");

    void *argtable[] = {
        arg_param_begin,
        arg_int0(NULL, "delay", "<dec>", "Delay in us before triggering tear-off, must be between 1 and 43000"),
        arg_lit0(NULL, "on", "Activate tear-off hook"),
        arg_lit0(NULL, "off", "Deactivate tear-off hook"),
        arg_lit0("s", "silent", "less verbose output"),
        arg_param_end
    };

    CLIExecWithReturn(ctx, Cmd, argtable, false);
    tearoff_params_t params;
    int delay = arg_get_int_def(ctx, 1, -1);
    params.on = arg_get_lit(ctx, 2);
    params.off = arg_get_lit(ctx, 3);
    bool silent = arg_get_lit(ctx, 4);
    CLIParserFree(ctx);

    if (delay != -1) {
        if ((delay < 1) || (delay > 43000)) {
            PrintAndLogEx(WARNING, "You can't set delay out of 1..43000 range!");
            return PM3_EINVARG;
        }
    } else {
        delay = 0; // will be ignored by ARM
    }

    params.delay_us = delay;
    if (params.on && params.off) {
        PrintAndLogEx(WARNING, "You can't set both --on and --off!");
        return PM3_EINVARG;
    }

    return handle_tearoff(&params, !silent);
}

static int CmdTia(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw tia",
                  "Trigger a Timing Interval Acquisition to re-adjust the RealTimeCounter divider",
                  "hw tia"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);

    PrintAndLogEx(INFO, "Triggering new Timing Interval Acquisition (TIA)...");
    clearCommandBuffer();
    SendCommandNG(CMD_TIA, NULL, 0);
    PacketResponseNG resp;
    if (WaitForResponseTimeout(CMD_TIA, &resp, 2000) == false) {
        PrintAndLogEx(WARNING, "TIA command timeout. You probably need to unplug the Proxmark3.");
        return PM3_ETIMEOUT;
    }
    PrintAndLogEx(INFO, "TIA done.");
    return PM3_SUCCESS;
}

static int CmdPing(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw ping",
                  "Test if the Proxmark3 is responsive",
                  "hw ping\n"
                  "hw ping --len 32"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_u64_0("l", "len", "<dec>", "length of payload to send"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    uint32_t len = arg_get_u32(ctx, 1);
    CLIParserFree(ctx);

    if (len > PM3_CMD_DATA_SIZE)
        len = PM3_CMD_DATA_SIZE;

    if (len) {
        PrintAndLogEx(INFO, "Ping sent with payload len " _YELLOW_("%d"), len);
    } else {
        PrintAndLogEx(INFO, "Ping sent");
    }

    clearCommandBuffer();
    PacketResponseNG resp;
    uint8_t data[PM3_CMD_DATA_SIZE] = {0};

    for (uint16_t i = 0; i < len; i++)
        data[i] = i & 0xFF;

    SendCommandNG(CMD_PING, data, len);
    if (WaitForResponseTimeout(CMD_PING, &resp, 1000)) {
        if (len) {
            bool error = (memcmp(data, resp.data.asBytes, len) != 0);
            PrintAndLogEx((error) ? ERR : SUCCESS, "Ping response " _GREEN_("received") " and content is %s", error ? _RED_("NOT ok") : _GREEN_("OK"));
        } else {
            PrintAndLogEx(SUCCESS, "Ping response " _GREEN_("received"));
        }
    } else
        PrintAndLogEx(WARNING, "Ping response " _RED_("timeout"));
    return PM3_SUCCESS;
}

static int CmdConnect(const char *Cmd) {

    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw connect",
                  "Connects to a Proxmark3 device via specified serial port.\n"
                  "Baudrate here is only for physical UART or UART-BT, NOT for USB-CDC or blue shark add-on",
                  "hw connect -p "SERIAL_PORT_EXAMPLE_H"\n"
                  "hw connect -p "SERIAL_PORT_EXAMPLE_H" -b 115200"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_str0("p", "port", NULL, "Serial port to connect to, else retry the last used one"),
        arg_u64_0("b", "baud", "<dec>", "Baudrate"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);

    int p_len = FILE_PATH_SIZE;
    char port[FILE_PATH_SIZE] = {0};
    CLIGetStrWithReturn(ctx, 1, (uint8_t *)port, &p_len);
    uint32_t baudrate = arg_get_u32_def(ctx, 2, USART_BAUD_RATE);
    CLIParserFree(ctx);

    if (baudrate == 0) {
        PrintAndLogEx(WARNING, "Baudrate can't be zero");
        return PM3_EINVARG;
    }

    // default back to previous used serial port
    if (strlen(port) == 0) {
        if (strlen(g_conn.serial_port_name) == 0) {
            PrintAndLogEx(WARNING, "Must specify a serial port");
            return PM3_EINVARG;
        }
        memcpy(port, g_conn.serial_port_name, sizeof(port));
    }

    if (g_session.pm3_present) {
        CloseProxmark(g_session.current_device);
    }

    // 10 second timeout
    OpenProxmark(&g_session.current_device, port, false, 10, false, baudrate);

    if (g_session.pm3_present && (TestProxmark(g_session.current_device) != PM3_SUCCESS)) {
        PrintAndLogEx(ERR, _RED_("ERROR:") " cannot communicate with the Proxmark3\n");
        CloseProxmark(g_session.current_device);
        return PM3_ENOTTY;
    }
    return PM3_SUCCESS;
}

static int CmdBreak(const char *Cmd) {

    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw break",
                  "send break loop package",
                  "hw break\n"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);
    clearCommandBuffer();
    SendCommandNG(CMD_BREAK_LOOP, NULL, 0);
    return PM3_SUCCESS;
}


// Stage markers the firmware leaves in the common area (g_common_area.arg2).
static const char *fw_stage_name(uint32_t stage) {
    switch (stage) {
        case 1:  return "idle, top of main loop";
        case 2:  return "waiting for a USB packet";
        case 3:  return "inside a command handler";
        case 4:  return "button-hold check";
        case 10: return "14a reader: entered";
        case 11: return "14a reader: field set up";
        case 12: return "14a reader: card select done";
        case 13: return "14a reader: select reply sent";
        case 14: return "14a reader: turning field off";
        case 15: return "14a reader: done";
        case 20: return "version: entered";
        case 21: return "version: bootrom info formatted";
        case 22: return "version: os info formatted";
        case 23: return "version: sending reply";
        case 24: return "version: reply sent";
        case 30: return "felica: entered";
        case 31: return "felica: bitstream loaded";
        case 32: return "felica: ssp clock synced";
        case 33: return "felica: setup done";
        case 34: return "felica: card select done";
        case 35: return "felica: select reply sent";
        default: return "unknown/stale";
    }
}

// Bounded waits that gave up instead of spinning forever (g_diag_abort_site).
static const char *fw_abort_site_name(uint32_t site) {
    switch (site) {
        case 51: return "usb_write: 1st packet TXPKTRDY";
        case 52: return "usb_write: chunk TXCOMP set";
        case 53: return "usb_write: chunk TXCOMP clear";
        case 54: return "usb_write: chunk TXPKTRDY";
        case 55: return "usb_write: end TXCOMP set";
        case 56: return "usb_write: end TXCOMP clear";
        case 57: return "usb_write: ZLP TXCOMP set";
        case 58: return "usb_write: ZLP TXCOMP clear";
        case 61: return "FPGA SPI: TXEMPTY";
        case 62: return "FPGA SPI: RDRF";
        case 63: return "FPGA config: DONE stayed low (wrong bitstream for this FPGA?)";
        case 70: return "StartCountSspClk: SSC frame/clk pins";
        case 80: return "14a tx: ssp clock (timed)";
        case 81: return "14a tx: ssp clock";
        case 82: return "14a tx: SSC TXRDY";
        case 90: return "felica tx: ssp clock";
        case 91: return "felica tx: preamble TXRDY";
        case 92: return "felica tx: data TXRDY";
        case 93: return "felica tx: tail TXRDY #1";
        case 94: return "felica tx: tail TXRDY #2";
        case 95: return "felica rx: SSC delivered nothing";
        default: return "unknown";
    }
}

static const char *pio_name(int i) {
    static const char *n[32] = {
        "LED_A", "PA1", "LED_D", "NVDD_ON", "FPGA_NINIT", "PA5", "PCK0", "LRST",
        "LED_B", "LED_C", "NCS2", "NCS0", "MISO", "MOSI", "SPCK", "SSC_FRAME",
        "SSC_CLK", "SSC_DOUT", "SSC_DIN", "MUX_HIPKD", "MUX_LOPKD", "MUX_HIRAW", "MUX_LORAW", "BUTTON",
        "USB_PU", "RELAY", "FPGA_ON", "FPGA_DONE", "FPGA_NPROG", "FPGA_CCLK", "FPGA_DIN", "FPGA_DOUT"
    };
    return n[i & 31];
}

static void pio_print_bits(const char *label, uint32_t mask) {
    char buf[400] = {0};
    for (int i = 0; i < 32; i++) {
        if (mask & (1u << i)) {
            char t[24];
            snprintf(t, sizeof(t), " PA%d(%s)", i, pio_name(i));
            strncat(buf, t, sizeof(buf) - strlen(buf) - 1);
        }
    }
    PrintAndLogEx(SUCCESS, "  %-16s%s", label, mask ? buf : " -");
}

static int pio_probe_once(uint8_t pin, uint8_t level, bool drive, uint32_t hold, uint32_t win, pio_probe_resp_t *out) {
    pio_probe_req_t req = { .pin = pin, .level = level, .drive = drive, .hold_us = hold, .win_us = win };
    PacketResponseNG resp;
    clearCommandBuffer();
    SendCommandNG(CMD_PIO_PROBE, (uint8_t *)&req, sizeof(req));
    if (WaitForResponseTimeout(CMD_PIO_PROBE, &resp, 2000 + (hold + win) / 1000) == false) {
        PrintAndLogEx(WARNING, "pio probe timeout");
        return PM3_ETIMEOUT;
    }
    if (resp.status != PM3_SUCCESS || resp.length < sizeof(pio_probe_resp_t)) {
        PrintAndLogEx(ERR, "pio probe failed (status %d, length %u)", resp.status, resp.length);
        return PM3_ESOFT;
    }
    memcpy(out, resp.data.asBytes, sizeof(*out));
    return PM3_SUCCESS;
}

static int CmdPioProbe(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw pioprobe",
                  "Hardware probe: drive one PIOA pin, then watch all PIOA inputs.\n"
                  "Without -p it only watches. --scan pulses every pin (except USB_PU) and\n"
                  "reports which other inputs follow, e.g. which pin really drives FPGA NPROGRAM.",
                  "hw pioprobe --win 200000          -> just watch for 200ms\n"
                  "hw pioprobe -p 28 -l 0            -> pulse NPROGRAM low, watch NINIT/DONE\n"
                  "hw pioprobe --scan -l 0"
                 );
    void *argtable[] = {
        arg_param_begin,
        arg_int0("p", "pin", "<0-31>", "PAx to drive"),
        arg_int0("l", "level", "<0|1>", "level while driven (def 0)"),
        arg_u64_0(NULL, "hold", "<us>", "drive time (def 1000)"),
        arg_u64_0(NULL, "win", "<us>", "watch time after release (def 20000)"),
        arg_lit0(NULL, "scan", "pulse every pin in turn"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    int pin = arg_get_int_def(ctx, 1, -1);
    uint8_t level = arg_get_int_def(ctx, 2, 0) ? 1 : 0;
    uint32_t hold = arg_get_u32_def(ctx, 3, 1000);
    uint32_t win = arg_get_u32_def(ctx, 4, 20000);
    bool scan = arg_get_lit(ctx, 5);
    CLIParserFree(ctx);

    pio_probe_resp_t r;
    if (scan) {
        if (pio_probe_once(0, 0, false, 0, win, &r) != PM3_SUCCESS) return PM3_ETIMEOUT;
        // PCK0 is a clock we output ourselves, it always looks like it moved
        uint32_t noisy = r.tog_after | (r.before ^ r.after_last) | (1u << 6);
        PrintAndLogEx(INFO, "PDSR idle %08x, toggling on its own:", r.before);
        pio_print_bits("free running", noisy);
        for (int i = 0; i < 32; i++) {
            if (i == 24) continue; // USB_PU
            if (pio_probe_once(i, level, true, hold, win, &r) != PM3_SUCCESS) return PM3_ETIMEOUT;
            uint32_t self = 1u << i;
            // what other inputs moved while/after we drove PAi, ignoring free running ones
            uint32_t react = ((r.before ^ r.during) | r.tog_during | (r.during ^ r.after_first) | r.tog_after) & ~self & ~noisy;
            if (react == 0) continue;
            PrintAndLogEx(SUCCESS, "PA%-2d (%s) %s -> before %08x during %08x after %08x/%08x  NINIT high after %d us",
                          i, pio_name(i), level ? "high" : "low", r.before, r.during, r.after_first, r.after_last,
                          r.init_high_us == 0xffffffff ? -1 : (int)r.init_high_us);
            pio_print_bits("reacted", react);
        }
        PrintAndLogEx(INFO, "scan done");
        return PM3_SUCCESS;
    }

    if (pio_probe_once(pin < 0 ? 0 : pin, level, pin >= 0, hold, win, &r) != PM3_SUCCESS)
        return PM3_ETIMEOUT;
    PrintAndLogEx(SUCCESS, "PSR %08x OSR %08x ODSR %08x PUSR %08x", r.psr, r.osr, r.odsr, r.pusr);
    PrintAndLogEx(SUCCESS, "PDSR before %08x  during %08x  after %08x  end %08x  (%u samples)",
                  r.before, r.during, r.after_first, r.after_last, r.samples);
    pio_print_bits("low before", ~r.before);
    if (pin >= 0) {
        pio_print_bits("changed driving", (r.before ^ r.during) | r.tog_during);
        pio_print_bits("changed release", r.during ^ r.after_first);
    }
    pio_print_bits("toggled in win", r.tog_after);
    for (int i = 0; i < 32; i++)
        if (r.edges_after[i])
            PrintAndLogEx(SUCCESS, "    PA%-2d %-10s %u edges", i, pio_name(i), r.edges_after[i]);
    if (r.init_high_us == 0xffffffff)
        PrintAndLogEx(SUCCESS, "  NINIT never high in window");
    else
        PrintAndLogEx(SUCCESS, "  NINIT high after %u us", r.init_high_us);
    return PM3_SUCCESS;
}

static int CmdPioAdc(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw pioadc",
                  "Read the ARM ADC on all channels. AD0..AD3 (PA17..PA20) are also read as plain\n"
                  "inputs with the internal pull-up on/off: a pin that stays near a diode drop with\n"
                  "the pull-up on is clamped into an unpowered rail (e.g. an FPGA without supply).",
                  "hw pioadc"
                 );
    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);

    PacketResponseNG resp;
    clearCommandBuffer();
    SendCommandNG(CMD_PIO_ADC, NULL, 0);
    if (WaitForResponseTimeout(CMD_PIO_ADC, &resp, 3000) == false) {
        PrintAndLogEx(WARNING, "pio adc timeout");
        return PM3_ETIMEOUT;
    }
    if (resp.status != PM3_SUCCESS || resp.length < 3 * 8 * sizeof(uint16_t)) {
        PrintAndLogEx(ERR, "pio adc failed (status %d, length %u)", resp.status, resp.length);
        return PM3_ESOFT;
    }
    uint16_t v[3][8];
    memcpy(v, resp.data.asBytes, sizeof(v));
    static const char *what[8] = {
        "PA17 SSC_DOUT (to FPGA)", "PA18 SSC_DIN (from FPGA)", "PA19 MUX_HIPKD", "PA20 MUX_LOPKD",
        "AD4", "AD5", "AD6", "AD7"
    };
    // 10 bit, ADVREF = 3.3V
    PrintAndLogEx(INFO, "ch  pin                        as-is    pull-up   floating");
    for (int ch = 0; ch < 8; ch++) {
        if (ch < 4)
            PrintAndLogEx(SUCCESS, "AD%d %-26s %4u mV  %4u mV  %4u mV", ch, what[ch],
                          v[0][ch] * 3300 / 1023, v[1][ch] * 3300 / 1023, v[2][ch] * 3300 / 1023);
        else
            PrintAndLogEx(SUCCESS, "AD%d %-26s %4u mV", ch, what[ch], v[0][ch] * 3300 / 1023);
    }
    return PM3_SUCCESS;
}

static int CmdHfAdcDump(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw hfadcdump",
                  "Put the FPGA in HF reader mode (field ON, 848kHz subcarrier) and dump the raw\n"
                  "16-bit SSC amplitude words coming from the external ADC. Use to check the\n"
                  "external-ADC -> FPGA -> SSC receive path. Place a card on the antenna.",
                  "hw hfadcdump\n"
                  "hw hfadcdump --mode 0   -> RECEIVE_IQ\n"
                  "hw hfadcdump --mode 1   -> RECEIVE_AMPLITUDE"
                 );
    void *argtable[] = {
        arg_param_begin,
        arg_int0(NULL, "mode", "<n>", "FPGA reader receive mode low-nibble (0=IQ, 1=AMPLITUDE, default 1)"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    int mode = arg_get_int_def(ctx, 1, 1);
    CLIParserFree(ctx);

    hf_adc_dump_req_t req = { .mode = (uint8_t)mode, .pad = 0, .nwords = HF_ADC_DUMP_WORDS };
    PacketResponseNG resp;
    clearCommandBuffer();
    SendCommandNG(CMD_HF_ADC_DUMP, (uint8_t *)&req, sizeof(req));
    if (WaitForResponseTimeout(CMD_HF_ADC_DUMP, &resp, 3000) == false) {
        PrintAndLogEx(WARNING, "hf adc dump timeout");
        return PM3_ETIMEOUT;
    }
    if (resp.status != PM3_SUCCESS || resp.length < 2) {
        PrintAndLogEx(ERR, "hf adc dump failed (status %d, length %u)", resp.status, resp.length);
        return PM3_ESOFT;
    }
    if (mode == 5) {
        uint16_t nn = resp.length;
        uint8_t *dd = resp.data.asBytes;
        uint16_t nz = 0, first = 0xffff;
        for (uint16_t i = 0; i < nn; i++)
            if (dd[i] != 0x00) { nz++; if (first == 0xffff) first = i; }
        PrintAndLogEx(INFO, "FeliCa demod bits: bytes %u  non-zero %u  first nz @%d",
                      nn, nz, (first == 0xffff) ? -1 : (int)first);
        if (nz == 0)
            PrintAndLogEx(WARNING, _RED_("  all zero -> curbit never set (decode/sync not triggering)"));
        char l5[160]; int q = 0;
        for (uint16_t i = 0; i < nn; i++) {
            q += snprintf(l5 + q, sizeof(l5) - q, "%02X", dd[i]);
            if ((i & 31) == 31) { PrintAndLogEx(INFO, "D %04d %s", i - 31, l5); q = 0; }
        }
        if (q) PrintAndLogEx(INFO, "D %s", l5);
        return PM3_SUCCESS;
    }
    if (mode == 4) {
        // each byte = one raw adc_d sample (37.7us apart); find the modulation swing
        uint16_t nn = resp.length;
        uint8_t *dd = resp.data.asBytes;
        uint8_t mn = 255, mx = 0; uint32_t sum = 0;
        for (uint16_t i = 0; i < nn; i++) {
            if (dd[i] < mn) mn = dd[i];
            if (dd[i] > mx) mx = dd[i];
            sum += dd[i];
        }
        unsigned mean = nn ? sum / nn : 0;
        PrintAndLogEx(INFO, "FeliCa raw ADC: samples %u  min %u  max %u  mean %u  swing %u",
                      nn, mn, mx, mean, mx - mn);
        // count samples far from mean (modulation activity)
        uint16_t lo = 0, hi = 0;
        for (uint16_t i = 0; i < nn; i++) {
            if (dd[i] + 20 < mean) lo++;
            if (dd[i] > mean + 20) hi++;
        }
        PrintAndLogEx(INFO, "  samples >20 below mean: %u   >20 above: %u", lo, hi);
        char l2[200]; int q = 0;
        for (uint16_t i = 0; i < nn; i++) {
            q += snprintf(l2 + q, sizeof(l2) - q, "%02X", dd[i]);
            if ((i & 31) == 31) { PrintAndLogEx(INFO, "R %04d %s", i - 31, l2); q = 0; }
        }
        if (q) PrintAndLogEx(INFO, "R %s", l2);
        return PM3_SUCCESS;
    }
    if (mode == 3) {
        if (resp.length < 12) { PrintAndLogEx(ERR, "tx test short resp"); return PM3_ESOFT; }
        uint16_t *v = (uint16_t *)resp.data.asBytes;
        PrintAndLogEx(INFO, "Field vs ADC-mux (ARM HF peak-detector):");
        PrintAndLogEx(SUCCESS, "  HF_READER, no mux set ........ %u mV", v[0]);
        PrintAndLogEx(SUCCESS, "  HF_READER, mux HIPKD ......... %u mV", v[1]);
        PrintAndLogEx(SUCCESS, "  HF_READER, mux HIRAW ......... %u mV", v[2]);
        PrintAndLogEx(INFO, "TX modulation test (ISO14443A):");
        PrintAndLogEx(SUCCESS, "  READER_LISTEN (carrier on) ... %u mV", v[3]);
        PrintAndLogEx(SUCCESS, "  READER_MOD ssp_dout=0 (on) ... %u mV", v[4]);
        PrintAndLogEx(SUCCESS, "  READER_MOD ssp_dout=1 (pause). %u mV", v[5]);
        if (v[5] < v[4] / 2)
            PrintAndLogEx(SUCCESS, _GREEN_("  => field pauses: TX modulation path WORKS"));
        else
            PrintAndLogEx(WARNING, _RED_("  => field does NOT pause: TX modulation path BROKEN"));
        return PM3_SUCCESS;
    }
    uint16_t n = resp.length / 2;
    uint16_t *w = (uint16_t *)resp.data.asBytes;

    // mode 2: each word is {window_max(hi), window_min(lo)}; otherwise two raw bytes
    uint16_t gmin = 0xffff, gmax = 0, maxspan = 0;
    uint32_t spansum = 0;
    for (uint16_t i = 0; i < n; i++) {
        uint8_t hi = (uint8_t)(w[i] >> 8), lo = (uint8_t)(w[i] & 0xff);
        uint8_t span = (hi >= lo) ? (hi - lo) : (lo - hi);
        spansum += span;
        if (span > maxspan) maxspan = span;
        if (lo < gmin) gmin = lo;
        if (hi > gmax) gmax = hi;
    }
    PrintAndLogEx(INFO, "words %u   overall min %u  max %u   per-window span avg %u  max %u",
                  n, gmin, gmax, n ? (unsigned)(spansum / n) : 0, maxspan);
    PrintAndLogEx(INFO, "(mode 2: large per-window span => raw carrier at ADC; ~0 => DC envelope)");
    PrintAndLogEx(INFO, "first 32 words (hex):");
    char line[128]; int p = 0;
    for (uint16_t i = 0; i < n && i < 32; i++) {
        p += snprintf(line + p, sizeof(line) - p, "%04X ", w[i]);
        if ((i & 7) == 7) { PrintAndLogEx(INFO, "  %s", line); p = 0; }
    }
    if (p) PrintAndLogEx(INFO, "  %s", line);
    return PM3_SUCCESS;
}

static int CmdHf14aRxDump(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw rxdump14a",
                  "Send a WUPA and dump the raw FPGA demodulator byte stream, to see whether the\n"
                  "card's reply appears as modulation activity. Place an ISO14443A card on the antenna.",
                  "hw rxdump14a");
    void *argtable[] = { arg_param_begin, arg_param_end };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);

    PacketResponseNG resp;
    clearCommandBuffer();
    SendCommandNG(CMD_HF_14A_RXDUMP, NULL, 0);
    if (WaitForResponseTimeout(CMD_HF_14A_RXDUMP, &resp, 3000) == false) {
        PrintAndLogEx(WARNING, "rxdump timeout");
        return PM3_ETIMEOUT;
    }
    uint16_t n = resp.length;
    uint8_t *d = resp.data.asBytes;
    uint16_t nz = 0, first = 0xffff, last = 0;
    for (uint16_t i = 0; i < n; i++) {
        if (d[i] != 0x00) { nz++; if (first == 0xffff) first = i; last = i; }
    }
    PrintAndLogEx(INFO, "bytes %u   non-zero %u   first nz @%d  last nz @%d", n, nz,
                  (first == 0xffff) ? -1 : (int)first, (int)last);
    if (nz == 0)
        PrintAndLogEx(WARNING, _RED_("  all zero -> demod saw no modulation (no reply detected)"));
    else
        PrintAndLogEx(SUCCESS, _GREEN_("  modulation present -> card replied; decode/timing is the issue"));
    // print the window around first activity
    uint16_t start = (first == 0xffff) ? 0 : (first > 8 ? first - 8 : 0);
    char line[128]; int p = 0;
    for (uint16_t i = start; i < n && i < start + 64; i++) {
        p += snprintf(line + p, sizeof(line) - p, "%02X", d[i]);
        if (((i - start) & 15) == 15) { PrintAndLogEx(INFO, "  %04d: %s", start + ((i - start) & ~15), line); p = 0; }
        else p += snprintf(line + p, sizeof(line) - p, " ");
    }
    if (p) PrintAndLogEx(INFO, "  %s", line);
    return PM3_SUCCESS;
}

static int fpga_rawload_send(fpga_rawload_req_t *req, size_t len, fpga_rawload_resp_t *out, uint32_t timeout) {
    PacketResponseNG resp;
    clearCommandBuffer();
    SendCommandNG(CMD_FPGA_RAWLOAD, (uint8_t *)req, len);
    if (WaitForResponseTimeout(CMD_FPGA_RAWLOAD, &resp, timeout) == false) {
        PrintAndLogEx(WARNING, "fpga rawload timeout (phase %u)", req->phase);
        return PM3_ETIMEOUT;
    }
    if (resp.status != PM3_SUCCESS || resp.length < sizeof(fpga_rawload_resp_t)) {
        PrintAndLogEx(ERR, "fpga rawload failed (status %d, length %u)", resp.status, resp.length);
        return PM3_ESOFT;
    }
    memcpy(out, resp.data.asBytes, sizeof(*out));
    return PM3_SUCCESS;
}

// .bit: fixed 13 byte preamble, then sections a..d (16 bit length) and e (32 bit length) = config data
static const uint8_t *bit_find_data(const uint8_t *d, size_t n, size_t *len) {
    static const uint8_t pre[13] = {0x00, 0x09, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x00, 0x00, 0x01};
    if (n < sizeof(pre) || memcmp(d, pre, sizeof(pre)) != 0) {
        *len = n;
        return d;
    }
    size_t p = sizeof(pre);
    while (p + 3 <= n) {
        uint8_t sec = d[p];
        if (sec == 'e') {
            if (p + 5 > n) break;
            uint32_t l = ((uint32_t)d[p + 1] << 24) | (d[p + 2] << 16) | (d[p + 3] << 8) | d[p + 4];
            if (p + 5 + l > n) break;
            *len = l;
            return d + p + 5;
        }
        uint16_t l = (d[p + 1] << 8) | d[p + 2];
        if (sec >= 'a' && sec <= 'd')
            PrintAndLogEx(INFO, "  bit header %c: %.*s", sec, (int)(l ? l - 1 : 0), d + p + 3);
        p += 3 + l;
    }
    return NULL;
}

static int CmdFpgaLoad(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw fpgaload",
                  "Load a Xilinx bitstream (.bit or raw .bin) into the FPGA in slave serial mode,\n"
                  "streamed from the client, without reflashing the ARM. Config pins can be overridden\n"
                  "for boards with unknown wiring. Reports whether DONE went high.",
                  "hw fpgaload -f test_3s50a.bit\n"
                  "hw fpgaload -f test.bit --cclk 29 --din 30 --prog 28 --done 27"
                 );
    void *argtable[] = {
        arg_param_begin,
        arg_str1("f", "file", "<fn>", "bitstream file"),
        arg_int0(NULL, "prog", "<0-31>", "PAx to PROG_B (def 28)"),
        arg_int0(NULL, "cclk", "<0-31>", "PAx to CCLK (def 29)"),
        arg_int0(NULL, "din", "<0-31>", "PAx to DIN (def 30)"),
        arg_int0(NULL, "done", "<0-31>", "PAx from DONE (def 27)"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, false);
    int fnlen = 0;
    char fn[FILE_PATH_SIZE] = {0};
    CLIParamStrToBuf(arg_get_str(ctx, 1), (uint8_t *)fn, FILE_PATH_SIZE, &fnlen);
    fpga_rawload_req_t req;
    memset(&req, 0, sizeof(req));
    req.pin_prog = arg_get_int_def(ctx, 2, 28);
    req.pin_cclk = arg_get_int_def(ctx, 3, 29);
    req.pin_din = arg_get_int_def(ctx, 4, 30);
    req.pin_done = arg_get_int_def(ctx, 5, 27);
    CLIParserFree(ctx);

    FILE *f = fopen(fn, "rb");
    if (f == NULL) {
        PrintAndLogEx(ERR, "can't open %s", fn);
        return PM3_EFILE;
    }
    fseek(f, 0, SEEK_END);
    long fsz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsz <= 0) {
        fclose(f);
        return PM3_EFILE;
    }
    uint8_t *buf = calloc(fsz, 1);
    if (buf == NULL) {
        fclose(f);
        return PM3_EMALLOC;
    }
    size_t rd = fread(buf, 1, fsz, f);
    fclose(f);

    size_t len = 0;
    const uint8_t *data = bit_find_data(buf, rd, &len);
    if (data == NULL) {
        PrintAndLogEx(ERR, "no config data section in %s", fn);
        free(buf);
        return PM3_EFILE;
    }
    PrintAndLogEx(INFO, "%zu bytes of config data, PROG PA%u CCLK PA%u DIN PA%u DONE PA%u",
                  len, req.pin_prog, req.pin_cclk, req.pin_din, req.pin_done);

    fpga_rawload_resp_t r;
    req.phase = FPGA_RAWLOAD_START;
    int res = fpga_rawload_send(&req, sizeof(req), &r, 2000);
    if (res != PM3_SUCCESS) {
        free(buf);
        return res;
    }
    PrintAndLogEx(INFO, "PDSR idle %08x  PROG low %08x  released %08x", r.pdsr_idle, r.pdsr_prog, r.pdsr_after);

    req.phase = FPGA_RAWLOAD_DATA;
    for (size_t off = 0; off < len; off += FPGA_RAWLOAD_CHUNK) {
        req.len = MIN(FPGA_RAWLOAD_CHUNK, len - off);
        memcpy(req.data, data + off, req.len);
        res = fpga_rawload_send(&req, sizeof(req), &r, 2000);
        if (res != PM3_SUCCESS) {
            free(buf);
            return res;
        }
    }
    free(buf);

    req.phase = FPGA_RAWLOAD_FINISH;
    res = fpga_rawload_send(&req, 8, &r, 5000);
    if (res != PM3_SUCCESS)
        return res;
    PrintAndLogEx(INFO, "%u bytes shifted, PDSR at end %08x", r.bytes, r.pdsr_after);
    if (r.done_clocks == 0xffffffff)
        PrintAndLogEx(FAILED, "DONE (PA%u) never went high", req.pin_done);
    else
        PrintAndLogEx(SUCCESS, "DONE (PA%u) high after %u extra clocks", req.pin_done, r.done_clocks);
    return PM3_SUCCESS;
}

static int CmdFpgaProbe(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw fpgaprobe",
                  "Read raw frames from a wiring-probe bitstream loaded with `hw fpgaload`:\n"
                  "DIN (PA30) is made an input and clocked by CCLK (PA29). Optionally holds one\n"
                  "PIOA pin high/low meanwhile, to see which FPGA pin follows it.",
                  "hw fpgaprobe\n"
                  "hw fpgaprobe -p 15 -l 1"
                 );
    void *argtable[] = {
        arg_param_begin,
        arg_int0("p", "pin", "<0-31>", "PAx to hold while reading"),
        arg_int0("l", "level", "<0|1>", "level to hold (def 0)"),
        arg_int0(NULL, "bits", "<n>", "bits to read (def 1216)"),
        arg_int0(NULL, "arm", "<n>", "CCLK edges before reading (def 1100)"),
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    fpga_probe_req_t req;
    memset(&req, 0, sizeof(req));
    int pin = arg_get_int_def(ctx, 1, -1);
    req.pin = pin < 0 ? 0 : pin;
    req.drive = pin >= 0;
    req.level = arg_get_int_def(ctx, 2, 0) ? 1 : 0;
    req.nbits = arg_get_int_def(ctx, 3, 1216);
    req.arm_clocks = arg_get_int_def(ctx, 4, 1100);
    CLIParserFree(ctx);

    PacketResponseNG resp;
    clearCommandBuffer();
    SendCommandNG(CMD_FPGA_PROBE_READ, (uint8_t *)&req, sizeof(req));
    if (WaitForResponseTimeout(CMD_FPGA_PROBE_READ, &resp, 3000) == false) {
        PrintAndLogEx(WARNING, "fpga probe timeout");
        return PM3_ETIMEOUT;
    }
    if (resp.status != PM3_SUCCESS) {
        PrintAndLogEx(ERR, "fpga probe failed (status %d)", resp.status);
        return PM3_ESOFT;
    }
    PrintAndLogEx(SUCCESS, "probe %s %s", req.drive ? "hold" : "idle",
                  sprint_hex_inrow(resp.data.asBytes, resp.length));
    return PM3_SUCCESS;
}

static int CmdResetCause(const char *Cmd) {
    CLIParserContext *ctx;
    CLIParserInit(&ctx, "hw resetcause",
                  "Show the AT91 reset status register (RSTC_RSR) of the last reset.\n"
                  "RSTTYP keeps the cause until the next reset, so read it right after a reconnect.\n"
                  "URSTS/BODSTS are cleared by the first read, which is done by the bootrom.",
                  "hw resetcause"
                 );

    void *argtable[] = {
        arg_param_begin,
        arg_param_end
    };
    CLIExecWithReturn(ctx, Cmd, argtable, true);
    CLIParserFree(ctx);

    clearCommandBuffer();
    PacketResponseNG resp;
    SendCommandNG(CMD_RESET_CAUSE, NULL, 0);
    if (WaitForResponseTimeout(CMD_RESET_CAUSE, &resp, 2000) == false) {
        PrintAndLogEx(WARNING, "reset cause command timeout");
        return PM3_ETIMEOUT;
    }
    if (resp.status != PM3_SUCCESS || resp.length < 3 * sizeof(uint32_t)) {
        PrintAndLogEx(ERR, "reset cause command failed (status %d, length %u)", resp.status, resp.length);
        return resp.status;
    }

    uint32_t rsr = resp.data.asDwords[0];
    uint32_t stage = resp.data.asDwords[1];
    uint32_t live = resp.data.asDwords[2];
    const char *typ;
    // RSTTYP field values, see at91sam7s512.h
    switch (rsr & 0x700) {
        case 0x000:
            typ = "power-up (POR)";
            break;
        case 0x100:
            typ = "wake-up";
            break;
        case 0x200:
            typ = "watchdog";
            break;
        case 0x300:
            typ = "software";
            break;
        case 0x400:
            typ = "user (NRST pin pulled low)";
            break;
        case 0x500:
            typ = "brownout (supply dipped)";
            break;
        default:
            typ = "unknown";
            break;
    }

    PrintAndLogEx(SUCCESS, "RSTC_RSR................... %08x", rsr);
    PrintAndLogEx(SUCCESS, "  reset type............... %s", typ);
    PrintAndLogEx(SUCCESS, "  URSTS (NRST low)......... %d", (rsr & 0x1) ? 1 : 0);
    PrintAndLogEx(SUCCESS, "  BODSTS (brownout seen)... %d", (rsr & 0x2) ? 1 : 0);
    PrintAndLogEx(SUCCESS, "  NRSTL (pin level)........ %d", (rsr & 0x10000) ? 1 : 0);

    PrintAndLogEx(SUCCESS, "  last fw stage............ %u (%s)", stage, fw_stage_name(stage));
    PrintAndLogEx(SUCCESS, "  current fw stage......... %u", live);

    if (resp.length >= 5 * sizeof(uint32_t)) {
        uint32_t site = resp.data.asDwords[3];
        uint32_t count = resp.data.asDwords[4];
        PrintAndLogEx(SUCCESS, "  bounded-wait give-ups.... %u", count);
        if (count)
            PrintAndLogEx(SUCCESS, "  last give-up site........ %u (%s)", site, fw_abort_site_name(site));
    }
    return PM3_SUCCESS;
}

static command_t CommandTable[] = {
    {"-------------", CmdHelp,         AlwaysAvailable, "----------------------- " _CYAN_("Hardware") " -----------------------"},
    {"help",          CmdHelp,         AlwaysAvailable, "This help"},
    {"break",         CmdBreak,        IfPm3Present,    "Send break loop usb command"},
    {"connect",       CmdConnect,      AlwaysAvailable, "Connect Proxmark3 to serial port"},
    {"dbg",           CmdDbg,          IfPm3Present,    "Set Proxmark3 debug level"},
    {"detectreader",  CmdDetectReader, IfPm3Present,    "Detect external reader field"},
    {"fpgaoff",       CmdFPGAOff,      IfPm3Present,    "Set FPGA off"},
    {"lcd",           CmdLCD,          IfPm3Lcd,        "Send command/data to LCD"},
    {"lcdreset",      CmdLCDReset,     IfPm3Lcd,        "Hardware reset LCD"},
    {"ping",          CmdPing,         IfPm3Present,    "Test if the Proxmark3 is responsive"},
    {"fpgaload",      CmdFpgaLoad,     IfPm3Present,    "Stream a .bit/.bin into the FPGA (slave serial), report DONE"},
    {"fpgaprobe",     CmdFpgaProbe,    IfPm3Present,    "Read raw frames from a wiring-probe bitstream"},
    {"pioadc",        CmdPioAdc,       IfPm3Present,    "Analog read of AD0..AD7, pins with pull-up on/off (supply check)"},
    {"hfadcdump",     CmdHfAdcDump,    IfPm3Present,    "Dump raw SSC amplitude words from the FPGA HF reader (external ADC check)"},
    {"rxdump14a",     CmdHf14aRxDump,  IfPm3Present,    "Send WUPA and dump raw FPGA demod stream (14443A RX check)"},
    {"pioprobe",      CmdPioProbe,     IfPm3Present,    "Drive a PIOA pin and watch which inputs react (board wiring probe)"},
    {"readmem",       CmdReadmem,      IfPm3Present,    "Read memory at decimal address from flash"},
    {"resetcause",    CmdResetCause,   IfPm3Present,    "Show the AT91 reset status register (cause of the last reset)"},
    {"reset",         CmdReset,        IfPm3Present,    "Reset the Proxmark3"},
    {"setlfdivisor",  CmdSetDivisor,   IfPm3Present,    "Drive LF antenna at 12MHz / (divisor + 1)"},
    {"setmux",        CmdSetMux,       IfPm3Present,    "Set the ADC mux to a specific value"},
    {"standalone",    CmdStandalone,   IfPm3Present,    "Jump to the standalone mode"},
    {"status",        CmdStatus,       IfPm3Present,    "Show runtime status information about the connected Proxmark3"},
    {"tearoff",       CmdTearoff,      IfPm3Present,    "Program a tearoff hook for the next command supporting tearoff"},
    {"tia",           CmdTia,          IfPm3Present,    "Trigger a Timing Interval Acquisition to re-adjust the RealTimeCounter divider"},
    {"tune",          CmdTune,         IfPm3Present,    "Measure antenna tuning"},
    {"version",       CmdVersion,      AlwaysAvailable, "Show version information about the client and the connected Proxmark3, if any"},
    {NULL, NULL, NULL, NULL}
};

static int CmdHelp(const char *Cmd) {
    (void)Cmd; // Cmd is not used so far
    CmdsHelp(CommandTable);
    return PM3_SUCCESS;
}

int CmdHW(const char *Cmd) {
    clearCommandBuffer();
    return CmdsParse(CommandTable, Cmd);
}

void pm3_version(bool verbose, bool oneliner) {

#if defined(__MINGW64__)
# define PM3CLIENTCOMPILER "MinGW-w64 "
#elif defined(__MINGW32__)
# define PM3CLIENTCOMPILER "MinGW "
#elif defined(__clang__)
# define PM3CLIENTCOMPILER "Clang/LLVM "
#elif defined(__GNUC__) || defined(__GNUG__)
# define PM3CLIENTCOMPILER "GCC "
#else
# define PM3CLIENTCOMPILER "unknown compiler "
#endif

#if defined(__APPLE__) || defined(__MACH__)
# define PM3HOSTOS "OSX"
#elif defined(__ANDROID__) || defined(ANDROID)
// must be tested before __linux__
# define PM3HOSTOS "Android"
#elif defined(__linux__)
# define PM3HOSTOS "Linux"
#elif defined(__FreeBSD__)
# define PM3HOSTOS "FreeBSD"
#elif defined(__NetBSD__)
# define PM3HOSTOS "NetBSD"
#elif defined(__OpenBSD__)
# define PM3HOSTOS "OpenBSD"
#elif defined(__CYGWIN__)
# define PM3HOSTOS "Cygwin"
#elif defined(_WIN64) || defined(__WIN64__)
// must be tested before _WIN32
# define PM3HOSTOS "Windows (64b)"
#elif defined(_WIN32) || defined(__WIN32__)
# define PM3HOSTOS "Windows (32b)"
#else
# define PM3HOSTOS "unknown"
#endif

#if defined(__x86_64__)
# define PM3HOSTARCH "x86_64"
#elif defined(__i386__)
# define PM3HOSTARCH "x86"
#elif defined(__aarch64__)
# define PM3HOSTARCH "aarch64"
#elif defined(__arm__)
# define PM3HOSTARCH "arm"
#elif defined(__powerpc64__)
# define PM3HOSTARCH "powerpc64"
#elif defined(__mips__)
# define PM3HOSTARCH "mips"
#else
# define PM3HOSTARCH "unknown"
#endif

    char temp[PM3_CMD_DATA_SIZE - 12]; // same limit as for ARM image

    if (oneliner) {
        // For "proxmark3 -v", simple printf, avoid logging
        FormatVersionInformation(temp, sizeof(temp), "Client: ", &g_version_information);
        PrintAndLogEx(NORMAL, "%s compiled with " PM3CLIENTCOMPILER __VERSION__ " OS:" PM3HOSTOS " ARCH:" PM3HOSTARCH "\n", temp);
        return;
    }

    if (!verbose)
        return;

    PrintAndLogEx(NORMAL, "\n [ " _CYAN_("Proxmark3 RFID instrument") " ]");
    PrintAndLogEx(NORMAL, "\n [ " _YELLOW_("CLIENT") " ]");
    FormatVersionInformation(temp, sizeof(temp), "  ", &g_version_information);
    PrintAndLogEx(NORMAL, "%s", temp);
    PrintAndLogEx(NORMAL, "  compiled with............. " PM3CLIENTCOMPILER __VERSION__);
    PrintAndLogEx(NORMAL, "  platform.................. " PM3HOSTOS " / " PM3HOSTARCH);
#ifdef HAVE_READLINE
    PrintAndLogEx(NORMAL, "  Readline support.......... " _GREEN_("present"));
#else
    PrintAndLogEx(NORMAL, "  Readline support.......... " _YELLOW_("absent"));
#endif
#ifdef HAVE_GUI
    PrintAndLogEx(NORMAL, "  QT GUI support............ " _GREEN_("present"));
#else
    PrintAndLogEx(NORMAL, "  QT GUI support............ " _YELLOW_("absent"));
#endif
#ifdef HAVE_BLUEZ
    PrintAndLogEx(NORMAL, "  native BT support......... " _GREEN_("present"));
#else
    PrintAndLogEx(NORMAL, "  native BT support......... " _YELLOW_("absent"));
#endif
#ifdef HAVE_PYTHON
    PrintAndLogEx(NORMAL, "  Python script support..... " _GREEN_("present"));
#else
    PrintAndLogEx(NORMAL, "  Python script support..... " _YELLOW_("absent"));
#endif
#ifdef HAVE_LUA_SWIG
    PrintAndLogEx(NORMAL, "  Lua SWIG support.......... " _GREEN_("present"));
#else
    PrintAndLogEx(NORMAL, "  Lua SWIG support.......... " _YELLOW_("absent"));
#endif
#ifdef HAVE_PYTHON_SWIG
    PrintAndLogEx(NORMAL, "  Python SWIG support....... " _GREEN_("present"));
#else
    PrintAndLogEx(NORMAL, "  Python SWIG support....... " _YELLOW_("absent"));
#endif

    if (g_session.pm3_present) {
        PrintAndLogEx(NORMAL, "\n [ " _YELLOW_("PROXMARK3") " ]");

        PacketResponseNG resp;
        clearCommandBuffer();
        SendCommandNG(CMD_VERSION, NULL, 0);

        if (WaitForResponseTimeout(CMD_VERSION, &resp, 1000)) {
            if (IfPm3Rdv4Fw()) {

                bool is_genuine_rdv4 = false;
                // validate signature data
                rdv40_validation_t mem;
                if (rdv4_get_signature(&mem) == PM3_SUCCESS) {
                    if (rdv4_validate(&mem) == PM3_SUCCESS) {
                        is_genuine_rdv4 = true;
                    }
                }

                PrintAndLogEx(NORMAL, "  device.................... %s", (is_genuine_rdv4) ? _GREEN_("RDV4") : _RED_("device / fw mismatch"));
                PrintAndLogEx(NORMAL, "  firmware.................. %s", (is_genuine_rdv4) ? _GREEN_("RDV4") : _YELLOW_("RDV4"));
                PrintAndLogEx(NORMAL, "  external flash............ %s", IfPm3Flash() ? _GREEN_("present") : _YELLOW_("absent"));
                PrintAndLogEx(NORMAL, "  smartcard reader.......... %s", IfPm3Smartcard() ? _GREEN_("present") : _YELLOW_("absent"));
                PrintAndLogEx(NORMAL, "  FPC USART for BT add-on... %s", IfPm3FpcUsartHost() ? _GREEN_("present") : _YELLOW_("absent"));
            } else {
                PrintAndLogEx(NORMAL, "  firmware.................. %s", _YELLOW_("PM3 GENERIC"));
                if (IfPm3FpcUsartHost()) {
                    PrintAndLogEx(NORMAL, "  FPC USART for BT add-on... %s", _GREEN_("present"));
                }
            }

            if (IfPm3FpcUsartDevFromUsb()) {
                PrintAndLogEx(NORMAL, "  FPC USART for developer... %s", _GREEN_("present"));
            }

            PrintAndLogEx(NORMAL, "");

            struct p {
                uint32_t id;
                uint32_t section_size;
                uint32_t versionstr_len;
                char versionstr[PM3_CMD_DATA_SIZE - 12];
            } PACKED;

            struct p *payload = (struct p *)&resp.data.asBytes;

            PrintAndLogEx(NORMAL,  payload->versionstr);
            if (strstr(payload->versionstr, "2s30vq100") == NULL) {
                PrintAndLogEx(NORMAL, "  FPGA firmware... %s", _RED_("chip mismatch"));
            }

            lookupChipID(payload->id, payload->section_size);
        }
    }
    PrintAndLogEx(NORMAL, "");
}
