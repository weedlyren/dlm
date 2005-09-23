/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004-2005 Red Hat, Inc.  All rights reserved.
**
**  This copyrighted material is made available to anyone wishing to use,
**  modify, copy, or redistribute it subject to the terms and conditions
**  of the GNU General Public License v.2.
**
*******************************************************************************
******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include "fsck_incore.h"

#define do_lseek(fd, off) \
  ((lseek((fd), (off), SEEK_SET) == (off)) ? 0 : -1)

#define do_read(fd, buff, len) \
  ((read((fd), (buff), (len)) == (len)) ? 0 : -1)

#define do_write(fd, buff, len) \
  ((write((fd), (buff), (len)) == (len)) ? 0 : -1)


int compute_height(struct fsck_sb *sdp, uint64_t sz);
int check_range(struct fsck_sb *sdp, uint64_t blkno);
int set_meta(struct buffer_head *bh, int type, int format);
int check_type(struct buffer_head *bh, int type);
int check_meta(struct buffer_head *bh, int type);
int next_rg_meta(struct fsck_rgrp *rgd, uint64_t *block, int first);
int next_rg_meta_free(struct fsck_rgrp *rgd, uint64_t *block, int first, int *free);
int next_rg_metatype(struct fsck_rgrp *rgd, uint64_t *block, uint32_t type, int first);
struct di_info *search_list(osi_list_t *list, uint64_t addr);

#endif /* __UTIL_H__ */