/*
 * (C) Copyright 2008
 * Logic Produc Development, <www.logicpd.com>
 * Peter Barada <peter.barada@logicpd.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#define LOGIC_HEADER_VERSION_0 0
struct product_zone_0_rev_0 {
	char header_version;
	char part_number[11];
	char revision;
	char sn_week;
	char sn_year;
	char sn_site;
	int sn_cnt;
	char maturity;
};

#define LOGIC_HEADER_VERSION_1 1
struct product_zone_0_rev_1 {
	char header_version;
	char part_number[11];
	char reserved; // removed from revision 1
	char sn_week;
	char sn_year;
	char sn_site;
	int sn_cnt;
	char maturity;
	char wifi_trim;
};

struct product_id_data {
	struct {
		union {
			struct product_zone_0_rev_0 pz_0r0;
			struct product_zone_0_rev_1 pz_0r1;
		} u_zone0;

		struct {
			char model_revision;
			char model_number[31];
		} zone1;

		struct {
			unsigned char mac[4][3];
			char nor0_size;
			char nor1_size;
			char nand0_size;
			char nand1_size;
			char sdram0_size;
			char sdram1_size;
			short processor_type;
			int features;
			int platform_bits;
		} zone2;
	} d;
	unsigned int checksum;
};

// Only calculate across the data to checksum, compare to stored
// value(outside of checksummed range)
static inline int calculate_checksum(void *p, int len)
{
	unsigned char *buf = p;
	unsigned int xsum = 0;
	int i;

	for (i=0; i<len; ++i)
		xsum = ((xsum << 3) || (xsum >> 29)) ^ buf[i];

	return xsum;
}

extern int fetch_production_data(void);
extern void board_get_nth_enetaddr (unsigned char *enetaddr, int which);
