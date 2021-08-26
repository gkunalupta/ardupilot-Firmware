/*
 * wq25_custom.cpp
 *
 *  Created on: 25-Aug-2021
 *      Author: kunal
 */

#include <AP_HAL/AP_HAL.h>
#include "wq25_custom.h"

 const AP_HAL::HAL& hal;



#define JEDEC_WRITE_ENABLE           0x06
#define JEDEC_WRITE_DISABLE          0x04
#define JEDEC_READ_STATUS            0x05
#define JEDEC_WRITE_STATUS           0x01
#define JEDEC_READ_DATA              0x03
#define JEDEC_FAST_READ              0x0b
#define JEDEC_DEVICE_ID              0x9F
#define JEDEC_PAGE_WRITE             0x02

#define JEDEC_BULK_ERASE             0xC7
#define JEDEC_SECTOR4_ERASE          0x20 // 4k erase
#define JEDEC_BLOCK32_ERASE          0x52 // 32K erase
#define JEDEC_BLOCK64_ERASE          0xD8 // 64K erase

#define JEDEC_STATUS_BUSY            0x01
#define JEDEC_STATUS_WRITEPROTECT    0x02
#define JEDEC_STATUS_BP0             0x04
#define JEDEC_STATUS_BP1             0x08
#define JEDEC_STATUS_BP2             0x10
#define JEDEC_STATUS_TP              0x20
#define JEDEC_STATUS_SEC             0x40
#define JEDEC_STATUS_SRP0            0x80

/*
  flash device IDs taken from betaflight flash_m25p16.c

  Format is manufacturer, memory type, then capacity
*/
#define JEDEC_ID_MACRONIX_MX25L3206E   0xC22016
#define JEDEC_ID_MACRONIX_MX25L6406E   0xC22017
#define JEDEC_ID_MACRONIX_MX25L25635E  0xC22019
#define JEDEC_ID_MICRON_M25P16         0x202015
#define JEDEC_ID_MICRON_N25Q064        0x20BA17
#define JEDEC_ID_MICRON_N25Q128        0x20ba18
#define JEDEC_ID_WINBOND_W25Q16        0xEF4015
#define JEDEC_ID_WINBOND_W25Q32        0xEF4016
#define JEDEC_ID_WINBOND_W25Q64        0xEF4017
#define JEDEC_ID_WINBOND_W25Q128       0xEF4018
#define JEDEC_ID_WINBOND_W25Q256       0xEF4019
#define JEDEC_ID_CYPRESS_S25FL128L     0x016018
#define reset1 0x66
#define reset2 0x99


#define block0_addr 0x000000
#define block1_addr 0x020000
#define block2_addr 0x030000
#define block3_addr 0x040000
#define block4_addr 0x050000
#define block5_addr 0x060000
#define block6_addr 0x070000
#define block7_addr 0x080000
#define block8_addr 0x090000
#define block9_addr 0x0A0000
#define block10_addr 0x0B0000



void AP_Wq25_custom::SPI2_Recv( uint8_t* recv, uint16_t recv_data )
{
	WITH_SEMAPHORE(dev_sem);
   dev->transfer(nullptr, 0, recv, recv_data);
}

void AP_Wq25_custom::SPI2_Send( uint8_t* tran, uint16_t tran_data)
{
	WITH_SEMAPHORE(dev_sem);
  dev->transfer(tran, tran_data, nullptr, 0);
}

void AP_Wq25_custom:: W25_Reset (void)
{

	uint8_t tx_buf[10];
  dev-> set_chip_select(true);
  tx_buf[0] = reset1;
  tx_buf[1] = reset2;
  SPI2_Send(tx_buf, 2);
  dev-> set_chip_select(false);
}


void AP_Wq25_custom::WriteEnable_flash(void)
{

	uint8_t tx_buf[10];
   dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_WRITE_ENABLE;
    SPI2_Send(tx_buf,1);
      dev-> set_chip_select(false);
}

void AP_Wq25_custom::W25_Read_Data(uint32_t addr, uint8_t* data, uint32_t sz)
{
	WITH_SEMAPHORE(dev_sem);
	uint8_t tx_buf[10];
	   printf("Kunal\n");
dev-> set_chip_select(true);
  tx_buf[0] =JEDEC_READ_DATA;
  tx_buf[1] = (addr >> 16) & 0xFF;
  tx_buf[2] = (addr >> 8) & 0xFF;
  tx_buf[3] = addr & 0xFF;
  SPI2_Send(tx_buf, 4);
 printf(" Gupta\n");
  SPI2_Recv(data, sz);
  printf("Botlab");

   dev-> set_chip_select(false);
}

void AP_Wq25_custom::W25_Write_Data(uint32_t addr, uint8_t* data, uint32_t sz)
{
	uint8_t tx_buf[10];
  WriteEnable_flash();
  dev-> set_chip_select(true);
  tx_buf[0] = JEDEC_PAGE_WRITE;
  tx_buf[1] = (addr >> 16) & 0xFF;
  tx_buf[2] = (addr >> 8) & 0xFF;
  tx_buf[3] = addr & 0xFF;
  SPI2_Send(tx_buf, 4);
  SPI2_Send(data, sz);
  dev-> set_chip_select(false);
}
uint32_t AP_Wq25_custom::W25_Read_ID(void)
{
	uint8_t tx_buf[10];
  uint8_t dt[4];
  tx_buf[0] = JEDEC_DEVICE_ID;
   dev-> set_chip_select(true);
  SPI2_Send(tx_buf, 1);
  SPI2_Recv(dt,3);
  dev-> set_chip_select(false);
  return (dt[0] << 16) | (dt[1] << 8) | dt[2];
}

void AP_Wq25_custom::erase_sector4KB(uint32_t addr)
{
	uint8_t tx_buf[10];
    WriteEnable_flash();
    dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_SECTOR4_ERASE;
    tx_buf[1] = (addr >> 16) & 0xFF;
    tx_buf[2] = (addr >> 8) & 0xFF;
    tx_buf[3] = addr & 0xFF;
    SPI2_Send(tx_buf,4);
    dev-> set_chip_select(false);
}
void AP_Wq25_custom::erase_sector32KB(uint32_t addr)
{
	uint8_t tx_buf[10];
    WriteEnable_flash();
     dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_BLOCK32_ERASE;
    tx_buf[1] = (addr >> 16) & 0xFF;
    tx_buf[2] = (addr >> 8) & 0xFF;
    tx_buf[3] = addr & 0xFF;
    SPI2_Send(tx_buf,4);
    dev-> set_chip_select(false);
}
void AP_Wq25_custom::erase_sector64KB(uint32_t addr)
{
	uint8_t tx_buf[10];
    WriteEnable_flash();
 dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_BLOCK64_ERASE;
    tx_buf[1] = (addr >> 16) & 0xFF;
    tx_buf[2] = (addr >> 8) & 0xFF;
    tx_buf[3] = addr & 0xFF;
    SPI2_Send(tx_buf,4);
   dev-> set_chip_select(false);
}
void AP_Wq25_custom::chip_erase(void)
{
	uint8_t tx_buf[10];
    WriteEnable_flash();
    dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_BULK_ERASE;
    SPI2_Send(tx_buf,1);
    dev-> set_chip_select(false);
}


void AP_Wq25_custom::WriteSR(uint8_t SR_address, uint8_t SR_data)
{
	uint8_t tx_buf[10];
    WriteEnable_flash();
    dev-> set_chip_select(true);
    tx_buf[0] = SR_address;
    tx_buf[1] = SR_data;
    SPI2_Send(tx_buf,2);
    dev-> set_chip_select(false);

}
uint8_t AP_Wq25_custom::ReadSR(uint8_t SR_address)
{
	uint8_t tx_buf[10];
    uint8_t RSR[1] = {0};
    dev-> set_chip_select(true);
    tx_buf[0] =  SR_address;
    SPI2_Send(tx_buf,1);
    SPI2_Recv(RSR,1);
   dev-> set_chip_select(false);
    return RSR[0];
}

void AP_Wq25_custom::wq25_init(void)
{
	dev = hal.spi->get_device("dataflash");
	    if (!dev) {
	        AP_HAL::panic("PANIC: AP_Logger SPIDeviceDriver not found");
	        return;
	    }

	    dev_sem = dev->get_semaphore();

	    if (!wq25_getSectorCount()) {
	        flash_died = true;
	        return;
	    }
	    flash_died = false;

}

bool AP_Wq25_custom::wq25_getSectorCount(void)
{
	 //WaitReady();
	 uint8_t tx_buf[10];
	    WITH_SEMAPHORE(dev_sem);

	    // Read manufacturer ID
	    tx_buf[0] = JEDEC_DEVICE_ID;
	    SPI2_Send(tx_buf,1);
	    SPI2_Recv(buffer,4);
	    //dev->transfer(&cmd, 1, buffer, 4);

	    uint32_t id = buffer[0] << 16 | buffer[1] << 8 | buffer[2];

	    uint32_t blocks = 0;

	    switch (id) {
	    case JEDEC_ID_WINBOND_W25Q16:
	    case JEDEC_ID_MICRON_M25P16:
	        blocks = 32;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        break;
	    case JEDEC_ID_WINBOND_W25Q32:
	    case JEDEC_ID_MACRONIX_MX25L3206E:
	        blocks = 64;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        break;
	    case JEDEC_ID_MICRON_N25Q064:
	    case JEDEC_ID_WINBOND_W25Q64:
	    case JEDEC_ID_MACRONIX_MX25L6406E:
	        blocks = 128;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        break;
	    case JEDEC_ID_MICRON_N25Q128:
	    case JEDEC_ID_WINBOND_W25Q128:
	    case JEDEC_ID_CYPRESS_S25FL128L:
	        blocks = 256;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        break;
	    case JEDEC_ID_WINBOND_W25Q256:
	    case JEDEC_ID_MACRONIX_MX25L25635E:
	        blocks = 512;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        use_32bit_address = true;
	        break;
	    default:
	        hal.scheduler->delay(2000);
	        printf("Unknown SPI Flash 0x%08x\n", id);
	        return false;
	    }

	    df_PageSize = 256;
	    df_NumPages = blocks * df_PagePerBlock;
	    erase_cmd = JEDEC_BLOCK64_ERASE;

	    printf("SPI Flash 0x%08x found pages=%u erase=%uk\n",
	           id, df_NumPages, (df_PagePerBlock * (uint32_t)df_PageSize)/1024);
	    return true;
}
