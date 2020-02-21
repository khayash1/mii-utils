// SPDX-License-Identifier: BSD-3-Clause
/*
 * mii_read - MII register dump utility
 * Copyright (c) 2020 Kunihiko Hayashi
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <linux/mii.h>
#include <linux/sockios.h>

#define MII_PAGSR	0x1f

static __u16 mii_read_reg(int fd, char *ifname, int phy_id, int reg_num)
{
	struct ifreq ifr;
	struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&ifr.ifr_data;
	int ret;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, ifname);

	mii->phy_id  = phy_id;
	mii->reg_num = reg_num;

	ret = ioctl(fd, SIOCGMIIREG, &ifr);
	if (ret < 0) {
		perror("Failed to get MII registers");
	}

	return mii->val_out;
}

static int mii_write_reg(int fd, char *ifname, int phy_id, int reg_num,
			 __u16 val)
{
	struct ifreq ifr;
	struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&ifr.ifr_data;
	int ret;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, ifname);

	mii->phy_id  = phy_id;
	mii->reg_num = reg_num;
	mii->val_in  = val;

	ret = ioctl(fd, SIOCSMIIREG, &ifr);
	if (ret < 0) {
		perror("Failed to set MII registers");
	}

	return ret;
}

int main(int argc, char **argv)
{
	struct ifaddrs *ifa, *ifa_list;
	char *ifname;
	int phy_id = 0;
	int page = 0;
	__u16 val;
	int fd, reg, ret;

	if (argc < 2) {
		printf("usage: %s <ifname> [phy-id] [page no]\n", argv[0]);
		exit(1);
	}

	ifname = argv[1];
	if (argc > 2)
		phy_id = atoi(argv[2]);
	if (argc > 3)
		page = atoi(argv[3]);

	ret = getifaddrs(&ifa_list);
	if (ret < 0) {
		perror("Failed to get interface address");
		exit(2);
	}

	for (ifa = ifa_list; ifa; ifa = ifa->ifa_next) {
		if (!strcmp(ifa->ifa_name, ifname))
			break;
	}
	if (!ifa) {
	        printf("Wrong interface name (%s)\n",ifname);
		exit(3);
	}

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
	        perror("Failed to open socket");
		exit(4);
	}

	if (page)
		mii_write_reg(fd, ifname, phy_id, MII_PAGSR, page);

	for (reg = 0; reg < 32; reg++) {
		val = mii_read_reg(fd, ifname, phy_id, reg);
		printf("0x%04x\n", val);
	}

	if (page)
		mii_write_reg(fd, ifname, phy_id, MII_PAGSR, 0);

	close(fd);

	return 0;
}
