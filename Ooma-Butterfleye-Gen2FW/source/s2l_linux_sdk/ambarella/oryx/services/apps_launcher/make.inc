##
## oryx/services/apps_launcher/make.inc
##
## History:
##    2014/09/09 - [Louis Sun] Created file
##
## Copyright (c) 2016 Ambarella, Inc.
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


ifeq ($(BUILD_AMBARELLA_ORYX_SERVICE_APPS_LAUNCHER), y)
LOCAL_PATH   := $(call my-dir)
include $(CLEAR_VARS)
ORYX_TEST_VER := $(shell cd $(LOCAL_PATH) && git log -1 2>/dev/null | head -n 1 | awk '{print $$2}')
ORYX_TEST_VER_FILE := $(LOCAL_PATH)/am_oryx_version.h
EXEC_CMD := $(shell echo "/*" > $(ORYX_TEST_VER_FILE))
EXEC_CMD += $(shell echo " * Automatically generated file: don't edit" >> $(ORYX_TEST_VER_FILE))
EXEC_CMD += $(shell echo " * Ambarella Internal Oryx Test Version ID" >> $(ORYX_TEST_VER_FILE))
EXEC_CMD += $(shell echo " */" >> $(ORYX_TEST_VER_FILE))
EXEC_CMD += $(shell echo "\#define ORYX_TEST_VERSION \"$(ORYX_TEST_VER)\"" >> $(ORYX_TEST_VER_FILE))

###############################################################################
LOCAL_TARGET := apps_launcher
LOCAL_SRCS   := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_CFLAGS :=  -I$(ORYX_DIR)/include       \
                 -I$(ORYX_DIR)/include/utility \
                 -I$(ORYX_DIR)/include/ipc    \
                 -I$(ORYX_DIR)/include/services  \
                 -I$(ORYX_DIR)/include/watchdog \
                 -I$(ORYX_DIR)/include/configure \
                 -I$(ORYX_DIR)/include/config_set \
                 -I$(ORYX_DIR)/include/utility/libarchive \
                 -I$(ORYX_DIR)/services/include \
                 -I$(ORYX_DIR)/services/apps_launcher_monitor \
                 -I$(LOCAL_PATH)             \
                 -std=c++11 -Werror

LOCAL_LDFLAGS := -Wl,-rpath-link=$(FAKEROOT_DIR)/usr/lib \
                 -Wl,-rpath-link=$(PREBUILD_3RD_PARTY_DIR)/libarchive/usr/lib \
                 -Wl,-rpath-link=$(PREBUILD_3RD_PARTY_DIR)/xz/usr/lib \
                 -Wl,-rpath-link=$(PREBUILD_3RD_PARTY_DIR)/bzip2/usr/lib \
                 -Wl,-rpath-link=$(PREBUILD_3RD_PARTY_DIR)/zlib/usr/lib \
                 -Wl,-rpath-link=$(PREBUILD_3RD_PARTY_DIR)/7zdec/usr/lib

LOCAL_LIBS    := libamutil.so \
                 libamipc.so  \
                 libamapxy.so \
                 libamservice-manager.so \
                 libamconfig.so\
                 libamcfgset.so \
                 libamosal.so \
                 libamwatchdog.so

LOCAL_EXEC_CMD = $(EXEC_CMD)

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(ORYX_BIN_DIR)/
	@cp -dpRf $< $(ORYX_BIN_DIR)/
	@echo "Build $@ Done."
	@mkdir -p $(FAKEROOT_DIR)/etc/oryx/apps
	@cp -dpRf $(ORYX_DIR)/services/apps_launcher/etc/*.acs  $(FAKEROOT_DIR)/etc/oryx/apps
	@echo "Copy Config scripts Done."

$(call add-target-into-build, $(LOCAL_TARGET))

endif