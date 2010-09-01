/*
	Provider: LimePC Multimedia Technologies Co., Limited
	Date:04/15/2010
	Copyright note: without provider's written consensus by the provider, any release
	of provider's code could result in infrigement of provider's intellectural properties.
	Autor:Cloudy Chen <chen_yunsong@mtcera.com>
*/
#ifndef MPC5125_NFC_STRUCT_H
#define MPC5125_NFC_STRUCT_H
#define IS_2K_PAGE_NAND		(priv->writesize == 2048)
#define IS_4K_PAGE_NAND		(priv->writesize == 4096)
#define IS_LARGE_PAGE_NAND	(priv->writesize > 512)

#define	PAGES_PER_BLOCK		0x100
#define MPC5125_NFC_ECC_STATUS_ADD (NFC_SPARE_AREA(0)+0xf0)
#define PAGE_virtual_2K	0x0840

#define ioctl 		CONFIG_SYS_IOCTRL_ADDR


struct mpc5125_nfc_save_struct{
	u32 nfc_sectsize;
	u32 nfc_config;
	u32 nfc_status;
};
#endif

