/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_cmd.h
#       @date         :2015/08/21 22:54
#       @algorithm    :
=============================================================================*/
#ifndef __VSR_CMD_H__
#define __VSR_CMD_H__

#include "vty.h"

void vsr_cmdline_init(void);
void vsr_cmd_config_write(struct vty *vty);
#endif
