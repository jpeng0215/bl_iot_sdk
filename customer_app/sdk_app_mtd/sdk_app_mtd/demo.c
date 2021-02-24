/*
 * Copyright (c) 2020 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <cli.h>
#include <bl_mtd.h>
#include <FreeRTOS.h>
#include <task.h>
#include "demo.h"

#define BL_MTD_PARTITION_NAME_USER      "user"
#define BUFFERSIZE                      (1024)

void blmtd(char *buf, int len, int argc, char **argv)
{
    bl_mtd_handle_t handle;
    bl_mtd_info_t info;
    uint8_t *wdata;
    uint8_t *rdata;
    int ret;

    if ((ret = bl_mtd_open(BL_MTD_PARTITION_NAME_USER, &handle, BL_MTD_OPEN_FLAG_BUSADDR))) {
        printf("error when get user partition %d\r\n", ret);
        return;
    }

    wdata = pvPortMalloc(BUFFERSIZE);
    rdata = pvPortMalloc(BUFFERSIZE);

    if (!wdata || !rdata) {
        printf("have no enough memory!\r\n");
        return;
    }
    memset(wdata, 0x55 ,BUFFERSIZE);
    memset(rdata, 0 ,BUFFERSIZE);
    memset(&info, 0, sizeof(info));

    bl_mtd_info(handle, &info);

    bl_mtd_write(handle, 0, BUFFERSIZE, wdata);
    vTaskDelay(100);
    bl_mtd_read(handle, 0, BUFFERSIZE, rdata);

    if (memcmp(wdata, rdata, BUFFERSIZE)) {
        printf("write and read failed\r\n");
    }

    memset(wdata, 0xFF ,BUFFERSIZE);
    bl_mtd_erase_all(handle);
    bl_mtd_read(handle, 0, BUFFERSIZE, rdata);

    if (memcmp(wdata, rdata, BUFFERSIZE)) {
        printf("erase failed\r\n");
    }
    
    vPortFree(wdata);
    vPortFree(rdata);

    bl_mtd_close(handle);
    printf("finshed\r\n");
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "mtd", "mtd demo", blmtd}
};

int mtd_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}

