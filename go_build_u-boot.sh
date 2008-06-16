#!/bin/bash

ALL_AT91_BOARD="at91sam9260ek at91sam9261ek at91sam9263ek at91sam9xeek at91sam9rlek at91sam9g20ek"
MAKE=/usr/local/bin/make-3.80
X_COMPILE=/opt/codesourcery/arm-2007q1/bin/arm-none-linux-gnueabi-

#########################################################
if [ "$2" == "" ]; then
	echo "syntax go_build_u-boot.sh [board:all] [directory]"
	exit 0
fi

mkdir -p $2
> $2/build.log
> $2/configure.log 

if [ "$1" == "all" ]; then
	AT91_BOARD=${ALL_AT91_BOARD}
else
	AT91_BOARD=$1
fi


for board in ${AT91_BOARD}; do
	echo -n "building for ${board} :"
	for media in dataflash nandflash default; do
		echo -n " ${media}"
		${MAKE} CROSS_COMPILE=${X_COMPILE} mrproper >> /dev/null
		if [ "${media}" == "default" ]; then 
			${MAKE} CROSS_COMPILE=${X_COMPILE} ${board}_config  >> configure.log 2>&1
		else
			${MAKE} CROSS_COMPILE=${X_COMPILE} ${board}_${media}_config  >> configure.log 2>&1
		fi
		if [ "$?" -ne 0 ]; then
			echo " error."
			exit 1
		fi
		${MAKE} CROSS_COMPILE=${X_COMPILE} >> build.log 2>&1
		if [ "$?" -ne 0 ]; then
			echo " error."
			exit 1
		fi
		mv -f u-boot.bin $2/u-boot-${board}-${media}.bin
	done
	echo "."
done
