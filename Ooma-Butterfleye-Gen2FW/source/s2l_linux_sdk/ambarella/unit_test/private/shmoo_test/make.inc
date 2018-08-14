##
## unit_test/private/vsync_test/make.inc
##
## History:
##    2013/12/20 - [BingLiang Hu] Created file
##
## Copyright (C) 2015 Ambarella, Inc.
##
## This file and its contents ("Software") are protected by intellectual
## property rights including, without limitation, U.S. and/or foreign
## copyrights. This Software is also the confidential and proprietary
## information of Ambarella, Inc. and its licensors. You may not use, reproduce,
## disclose, distribute, modify, or otherwise prepare derivative works of this
## Software or any portion thereof except pursuant to a signed license agreement
## or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
## In the absence of such an agreement, you agree to promptly notify and return
## this Software to Ambarella, Inc.
##
## THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
## MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
## INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
##

ifeq ($(BUILD_AMBARELLA_UNIT_TESTS_SHMOO), y)

LOCAL_PATH	:= $(call my-dir)

###
include $(CLEAR_VARS)

.PHONY: shmoo.sh

shmoo.sh: PRIVATE_PATH:=$(LOCAL_PATH)
shmoo.sh:
	@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $(PRIVATE_PATH)/shmoo.sh $(UNIT_TEST_PATH)/
	@cp -dpRf $(PRIVATE_PATH)/shmoo_memtester.sh $(UNIT_TEST_PATH)/
	@chmod +x $(FAKEROOT_DIR)/usr/local/bin/shmoo_memtester.sh
	@echo "Copy $@ Done."

$(call add-target-into-build, shmoo.sh)

###
include $(CLEAR_VARS)

LOCAL_TARGET	:= test_shmoo
LOCAL_SRCS	:= $(LOCAL_PATH)/test_shmoo.c
LOCAL_LDFLAGS	:= -lm

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $< $(UNIT_TEST_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

###
endif
