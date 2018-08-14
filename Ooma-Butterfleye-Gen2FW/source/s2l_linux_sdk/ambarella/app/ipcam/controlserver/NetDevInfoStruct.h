/**********************************************************************
 * NetDevInfoStruct.h
 *
 * Histroy:
 *  2011年03月17日 - [Yupeng Chang] created file
 *
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

#ifndef NETDEV_INFO_STRUCT_H
#define NETDEV_INFO_STRUCT_H
#include <string.h>

struct NetDevInfo {
  unsigned int if_index;
  char       * netdev_name;
  char       * netdev_ipv4;
  char       * netdev_maskv4;
  char       * netdev_ipv6;
  char       * netdev_maskv6;
  char       * netdev_mac;
  NetDevInfo * info_next;
  NetDevInfo ()
    : if_index(-1),
    netdev_name(0),
    netdev_ipv4(0),
    netdev_maskv4(0),
    netdev_ipv6(0),
    netdev_maskv6(0),
    netdev_mac(0),
    info_next(0)
  {}
  ~NetDevInfo ()
  {
    if (netdev_name) {
      delete [] netdev_name;
    }
    if (netdev_ipv4) {
      delete []netdev_ipv4;
    }
    if (netdev_maskv4) {
      delete [] netdev_maskv4;
    }
    if (netdev_ipv6) {
      delete [] netdev_ipv6;
    }
    if (netdev_maskv6) {
      delete [] netdev_maskv6;
    }
    if (netdev_mac) {
      delete [] netdev_mac;
    }
    if (info_next) {
      delete info_next;
    }
  }

  NetDevInfo * insert_netdev_info (NetDevInfo * info)
  {
    if (info && (info != this)) {
      info->info_next = this;
      return info;
    }
    return this;
  }

  NetDevInfo * next ()
  {
    return info_next;
  }

  NetDevInfo * find_netdev_by_name (const char * name)
  {
    if (name) {
      for (NetDevInfo * devInfo = this; devInfo; devInfo = devInfo->next ()) {
        if (devInfo->get_netdev_name () &&
            (strcmp(devInfo->get_netdev_name (), name) == 0)) {
          return devInfo;
        }
      }
    }
    return 0;
  }

  NetDevInfo * find_netdev_by_mac (const char * mac)
  {
    if (mac) {
      for (NetDevInfo * devInfo = this; devInfo; devInfo = devInfo->next ()) {
        if (devInfo->get_netdev_mac () &&
            (strcasecmp(devInfo->get_netdev_mac (), mac) == 0)) {
          return devInfo;
        }
      }
    }
    return 0;
  }

  void set_netdev_ifindex (unsigned int index)
  {
    if_index = index;
  }
  unsigned int get_netdev_ifindex ()
  {
    return if_index;
  }
  void set_netdev_name (const char * name)
  {
    if (netdev_name) {
      delete netdev_name;
      netdev_name = 0;
    }
    netdev_name = strdup (name);
  }
  char * get_netdev_name() {return netdev_name;}
  void set_netdev_ipv4 (const char * ip)
  {
    if (netdev_ipv4) {
      delete netdev_ipv4;
      netdev_ipv4 = 0;
    }
    netdev_ipv4 = strdup (ip);
  }
  char * get_netdev_ipv4() {return netdev_ipv4;}
  void set_netdev_maskv4(const char * mask)
  {
    if (netdev_maskv4) {
      delete netdev_maskv4;
      netdev_maskv4 = 0;
    }
    netdev_maskv4 = strdup (mask);
  }
  char * get_netdev_maskv4() {return netdev_maskv4;}
  void set_netdev_ipv6 (const char * ip)
  {
    if (netdev_ipv6) {
      delete netdev_ipv6;
      netdev_ipv6 = 0;
    }
    netdev_ipv6 = strdup (ip);
  }
  char * get_netdev_ipv6() {return netdev_ipv6;}
  void set_netdev_maskv6(const char * mask)
  {
    if (netdev_maskv6) {
      delete netdev_maskv6;
      netdev_maskv6 = 0;
    }
    netdev_maskv6 = strdup (mask);
  }
  char * get_netdev_maskv6() {return netdev_maskv6;}
  void set_netdev_mac (const char * mac)
  {
    if (netdev_mac) {
      delete netdev_mac;
      netdev_mac = 0;
    }
    netdev_mac = strdup (mac);
  }
  char * get_netdev_mac() {return netdev_mac;}
  bool check_mac (const char *mac)
  {
    if (mac) {
      for (NetDevInfo * devInfo = this; devInfo; devInfo = devInfo->next ()) {
        if (strcasecmp (devInfo->get_netdev_mac (), mac) == 0) {
          return true;
        }
      }
    }
    return false;
  }
};
#endif //NETDEV_INFO_STRUCT_H