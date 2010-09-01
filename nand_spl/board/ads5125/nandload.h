/*
	Provider: LimePC Multimedia Technologies Co., Limited
	Date:01/14/2010
	Copyright note: without provider's written consensus by the provider, any release
	of provider's code could result in infrigement of provider's intellectural properties.
	Autor:Cloudy Chen <chen_yunsong@mtcera.com>
*/
#include <ppc_asm.tmpl>
#include <ppc_defs.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <asm/cache.h>
#include <asm/mmu.h>
#include <mpc5125_nfc.h>
void sram_to_ddr(void);
void nandload(void);