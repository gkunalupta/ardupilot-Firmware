/*
 * wq25_custom.h
 *
 *  Created on: 25-Aug-2021
 *      Author: kunal
 */
#pragma once

#include <AP_HAL/AP_HAL.h>
#include <AP_Common/AP_Common.h>
#include <stdio.h>


class AP_spiFlash {
public:
AP_spiFlash() { }
/* Do not allow copies */
AP_spiFlash(const AP_spiFlash &other) = delete;
AP_spiFlash &operator=(const AP_spiFlash&) = delete;

// get singleton instance
static AP_spiFlash *get_singleton(void) {
return _singleton;
}

	bool wq25_getSectorCount();
        void wq25_init();
        //bool wq25_getSectorCount();

	void W25_Reset ();
	void WriteEnable_flash();
	void W25_Read_Data(uint32_t addr, uint8_t* data, uint32_t sz);
	void W25_Write_Data(uint32_t addr, uint8_t* data, uint32_t sz);
	uint32_t W25_Read_ID();
	void erase_sector4KB(uint32_t addr);
	void erase_sector32KB(uint32_t addr);
	void erase_sector64KB(uint32_t addr);
	void chip_erase();
	void WriteSR(uint8_t SR_address, uint8_t SR_data);
	uint8_t ReadSR(uint8_t SR_address);
protected:
	  uint32_t df_PageSize;
	    // number of pages in a (generally 64k) block
	    uint16_t df_PagePerBlock;
	    // number of pages in a (generally 4k) sector
	    uint16_t df_PagePerSector;
	    // number of pages on the chip
	    uint32_t df_NumPages;

	    static const uint16_t page_size_max = 256;
	    uint8_t *buffer;

private:
 
        static AP_spiFlash *_singleton;

	void SPI2_Recv(uint8_t* recv, uint32_t recv_data );
	void SPI2_Send(uint8_t* tran, uint32_t tran_data);

	AP_HAL::OwnPtr<AP_HAL::SPIDevice> dev;
	AP_HAL::Semaphore *dev_sem;

	bool flash_died;
	uint32_t erase_start_ms;
	uint8_t erase_cmd;
	bool use_32bit_address;


};

namespace AP {
    AP_spiFlash &spiflash();
};

