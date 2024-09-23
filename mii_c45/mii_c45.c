// SPDX-License-Identifier: BSD-3-Clause
/*
 * mii_c45 - MII register read/write utility
 * Copyright (c) 2020,2024 Kunihiko Hayashi
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <linux/mdio.h>
#include <linux/mii.h>
#include <linux/sockios.h>

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

int getarg(char *str)
{
	int val;

	if (!strncmp(str, "0x", 2))
		sscanf(str, "0x%x", &val);
	else
		val = atoi(str);

	return val;
}

int main(int argc, char **argv)
{
	struct ifaddrs *ifa, *ifa_list;
	char *ifname;
	int prtad;
	int devad;
	__u16 phyid, addr, val;
	int fd, reg, ret;
	int write_f = 0;

	if (argc < 5) {
		printf("usage: %s <ifname> <prtad> <devad> <addr> [value]\n", argv[0]);
		exit(1);
	}

	ifname = argv[1];
	prtad = getarg(argv[2]);
	devad = getarg(argv[3]);
	addr = getarg(argv[4]);
	if (argc > 5) {
		val = getarg(argv[5]);
		write_f = 1;
	}

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

	phyid = mdio_phy_id_c45(prtad, devad);
	if (write_f)
		mii_write_reg(fd, ifname, phyid, addr, val);
	else
		val = mii_read_reg(fd, ifname, phyid, addr);

	printf("%c: phyid:%04x addr:%04x val:0x%04x\n",
	       (write_f) ? 'W' : 'R', phyid, addr, val);

	close(fd);

	return 0;
}
