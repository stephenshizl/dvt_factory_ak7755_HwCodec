/**
 * @file system/include/flash/slcnand/hy27uf084g2b.h
 *
 * History:
 *    2009/11/10 - [Chien-Yang Chen] created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __HY27UF084G2B_H__
#define __HY27UF084G2B_H__

#define NAND_NAME		"HYNIX HY27UF084G2B_512MB_PG2K"

#define NAND_MANID		0xad
#define NAND_DEVID		0xdc
#define NAND_ID3		0x10
#define NAND_ID4		0x95

/**
 * define for device info
 */
#define NAND_MAIN_SIZE		2048
#define NAND_SPARE_SIZE		64
#define NAND_PAGE_SIZE		2112
#define NAND_PAGES_PER_BLOCK	64
#define NAND_BLOCKS_PER_BANK	4096

/**
 * timing parameter in ns
 */
#define NAND_TCLS		15
#define NAND_TALS		15
#define NAND_TCS		25
#define NAND_TDS		15
#define NAND_TCLH		5
#define NAND_TALH		5
#define NAND_TCH		5
#define NAND_TDH		5
#define NAND_TWP		15
#define NAND_TWH		10
#define NAND_TWB		100
#define NAND_TRR		20
#define NAND_TRP		15
#define NAND_TREH		10
#define NAND_TRB		100
#define NAND_TCEH		30
#define NAND_TRDELAY		25	/* trea*/
#define NAND_TCLR		15
#define NAND_TWHR		60
#define NAND_TIR		0
#define NAND_TWW		100	/* not define */
#define NAND_TRHZ		50	/* use max value */
#define NAND_TAR		15

#endif
