/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004 Red Hat, Inc.  All rights reserved.
**
**  This copyrighted material is made available to anyone wishing to use,
**  modify, copy, or redistribute it subject to the terms and conditions
**  of the GNU General Public License v.2.
**
*******************************************************************************
******************************************************************************/

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fs_dir.h"

#include "lost_n_found.h"
#include "link.h"
#include "fs_inode.h"
#include "bio.h"
#include "inode.h"
/* add_inode_to_lf - Add dir entry to lost+found for the inode
 * @ip: inode to add to lost + found
 *
 * This function adds an entry into the lost and found dir
 * for the given inode.  The name of the entry will be
 * "lost_<ip->i_num.no_addr>".
 *
 * Returns: 0 on success, -1 on failure.
 */
int add_inode_to_lf(struct fsck_inode *ip){
	char tmp_name[256];
	struct fsck_inode *lf_ip, *ri;
	osi_filename_t filename;
	struct block_query q;

	if(!ip->i_sbd->lf_dip) {
		log_info("Locating/Creating lost and found directory\n");

		load_inode(ip->i_sbd, ip->i_sbd->md.rooti->i_num.no_addr, &ri);

		if(fs_mkdir(ri, "l+f", 00700, &lf_ip)){
			log_err("Unable to create/locate l+f directory.\n");
		}
		free_inode(&ri);
		if(!lf_ip){
			log_warn("No l+f directory, can not add inode.\n");
			return -1;
		}
		log_notice("l+f directory at %"PRIu64"\n", lf_ip->i_num.no_addr);

		ip->i_sbd->lf_dip = lf_ip;
		if(block_check(ip->i_sbd->bl, lf_ip->i_num.no_addr, &q)) {
			stack;
			return -1;
		}
		if(q.block_type != inode_dir) {
			/* This is a new l+f directory, so set its
			 * block type and increment link counts for
			 * the directories */
			/* FIXME: i'd feel better about this if
			 * fs_mkdir returned whether it created a new
			 * directory or just found an old one, and we
			 * used that instead of the block_type to run
			 * this */
			block_set(ip->i_sbd->bl, lf_ip->i_num.no_addr, inode_dir);
			increment_link(ip->i_sbd, ip->i_sbd->md.rooti->i_num.no_addr);
			increment_link(ip->i_sbd, lf_ip->i_num.no_addr);
			increment_link(ip->i_sbd, lf_ip->i_num.no_addr);
		}
	} else {
		lf_ip = ip->i_sbd->lf_dip;
	}

	if(ip->i_num.no_addr == lf_ip->i_num.no_addr) {
		log_err("Trying to add l+f to itself...skipping");
		return 0;
	}
	switch(ip->i_di.di_mode & S_IFMT){
	case S_IFDIR:
		log_info("Adding .. entry pointing to l+f for %"PRIu64"\n",
			 ip->i_num.no_addr);
		sprintf(tmp_name, "..");
		filename.len = strlen(tmp_name);  /* no trailing NULL */
		if(!(filename.name = malloc(sizeof(char) * filename.len))) {
			log_err("Unable to allocate name\n");
			stack;
			return -1;
		}
		if(!memset(filename.name, 0, sizeof(char) * filename.len)) {
			log_err("Unable to zero name\n");
			stack;
			return -1;
		}
		memcpy(filename.name, tmp_name, filename.len);

		if(fs_dirent_del(ip, NULL, &filename)){
			log_warn("add_inode_to_lf:  "
				"Unable to remove \"..\" directory entry.\n");
		}

		if(fs_dir_add(ip, &filename, &(lf_ip->i_num),
			      lf_ip->i_di.di_mode)){
			log_err("Failed to link \"..\" entry to l+f directory.\n");
			block_set(ip->i_sbd->bl, ip->i_num.no_addr, meta_inval);
			return 0;
		}

		free(filename.name);
		sprintf(tmp_name, "lost_dir_%"PRIu64, ip->i_num.no_addr);
		break;
	case S_IFREG:
		sprintf(tmp_name, "lost_file_%"PRIu64, ip->i_num.no_addr);
		break;
	default:
		sprintf(tmp_name, "lost_%"PRIu64, ip->i_num.no_addr);
		break;
	}
	filename.len = strlen(tmp_name);  /* no trailing NULL */
	if(!(filename.name = malloc(sizeof(char) * filename.len))) {
		log_err("Unable to allocate name\n");
			stack;
			return -1;
		}
	if(!memset(filename.name, 0, sizeof(char) * filename.len)) {
		log_err("Unable to zero name\n");
		stack;
		return -1;
	}
	memcpy(filename.name, tmp_name, filename.len);

	if(fs_dir_add(lf_ip, &filename, &(ip->i_num), ip->i_di.di_mode)){
		log_err("Failed to add inode #%"PRIu64" to l+f dir.\n",
			ip->i_num.no_addr);
		/* FIXME: don't return -1 here, just mark the inode bad */
		free(filename.name);
		block_set(ip->i_sbd->bl, ip->i_num.no_addr, meta_inval);
		return 0;
	}
  	increment_link(ip->i_sbd, ip->i_num.no_addr);
	if(S_ISDIR(ip->i_di.di_mode)) {
		increment_link(ip->i_sbd, lf_ip->i_num.no_addr);
	}
	free(filename.name);
	log_notice("Added inode #%"PRIu64" to l+f dir\n", ip->i_num.no_addr);
	return 0;
}