/*
  simple hello world sketch
  Andrew Tridgell September 2011
*/

#include <AP_HAL/AP_HAL.h>
#include <AP_Common/AP_Common.h>
#include <stdio.h>

void setup();
void loop();

const AP_HAL::HAL& hal = AP_HAL::get_HAL();


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



AP_HAL::OwnPtr<AP_HAL::SPIDevice> dev;
AP_HAL::Semaphore *dev_sem;

uint8_t tx_buf[10];
uint8_t rx_buf[1025];

void SPI2_Recv( uint8_t* recv, uint16_t recv_data );
void SPI2_Send( uint8_t* tran, uint16_t tran_data);

void W25_Reset (void);
void WriteEnable_flash(void);
void W25_Read_Data(uint32_t addr, uint8_t* data, uint32_t sz);
void W25_Write_Data(uint32_t addr, uint8_t* data, uint32_t sz);
uint32_t W25_Read_ID(void);
void W25_Ini(void);
void erase_sector4KB(uint32_t addr);
void erase_sector32KB(uint32_t addr);
void erase_sector64KB(uint32_t addr);
void chip_erase(void);
void WriteSR(uint8_t SR_address, uint8_t SR_data);
uint8_t ReadSR(uint8_t SR_address);
void SPI2_Recv( uint8_t* recv, uint32_t recv_data )
{
   dev->transfer(nullptr, 0, recv, recv_data);
}

void SPI2_Send( uint8_t* tran, uint32_t tran_data)
{
  dev->transfer(tran, tran_data, nullptr, 0);
}

void W25_Reset (void)
{
  dev-> set_chip_select(true);
  tx_buf[0] = reset1;
  tx_buf[1] = reset2;
  SPI2_Send(tx_buf, 2);
  dev-> set_chip_select(false);
}


void WriteEnable_flash(void)
{
   dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_WRITE_ENABLE;
    SPI2_Send(tx_buf,1);
      dev-> set_chip_select(false);
}

void W25_Read_Data(uint32_t addr, uint8_t* data, uint32_t sz)
{
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

void W25_Write_Data(uint32_t addr, uint8_t* data, uint32_t sz)
{
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
uint32_t W25_Read_ID(void)
{
  uint8_t dt[4];
  tx_buf[0] = JEDEC_DEVICE_ID;
   dev-> set_chip_select(true);
  SPI2_Send(tx_buf, 1);
  SPI2_Recv(dt,3);
  dev-> set_chip_select(false);
  return (dt[0] << 16) | (dt[1] << 8) | dt[2];
}
void W25_Ini(void)
{
 // chThdSleepMilliseconds(100);
  W25_Reset();
 // chThdSleepMilliseconds(100);
 // unsigned int id = W25_Read_ID();

}
void erase_sector4KB(uint32_t addr)
{
    WriteEnable_flash();
    dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_SECTOR4_ERASE;
    tx_buf[1] = (addr >> 16) & 0xFF;
    tx_buf[2] = (addr >> 8) & 0xFF;
    tx_buf[3] = addr & 0xFF;
    SPI2_Send(tx_buf,4);
    dev-> set_chip_select(false);
}
void erase_sector32KB(uint32_t addr)
{
    WriteEnable_flash();
     dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_BLOCK32_ERASE;
    tx_buf[1] = (addr >> 16) & 0xFF;
    tx_buf[2] = (addr >> 8) & 0xFF;
    tx_buf[3] = addr & 0xFF;
    SPI2_Send(tx_buf,4);
    dev-> set_chip_select(false);
}
void erase_sector64KB(uint32_t addr)
{
    WriteEnable_flash();
 dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_BLOCK64_ERASE;
    tx_buf[1] = (addr >> 16) & 0xFF;
    tx_buf[2] = (addr >> 8) & 0xFF;
    tx_buf[3] = addr & 0xFF;
    SPI2_Send(tx_buf,4);
   dev-> set_chip_select(false);
}
void chip_erase(void)
{
    WriteEnable_flash();
    dev-> set_chip_select(true);
    tx_buf[0] = JEDEC_BULK_ERASE;
    SPI2_Send(tx_buf,1);
    dev-> set_chip_select(false);
}


void WriteSR(uint8_t SR_address, uint8_t SR_data)
{
    WriteEnable_flash();
    dev-> set_chip_select(true);
    tx_buf[0] = SR_address;
    tx_buf[1] = SR_data;
    SPI2_Send(tx_buf,2);
    dev-> set_chip_select(false);

}
uint8_t ReadSR(uint8_t SR_address)
{
    uint8_t RSR[1] = {0};
    dev-> set_chip_select(true);
    tx_buf[0] =  SR_address;
    SPI2_Send(tx_buf,1);
    SPI2_Recv(RSR,1);
   dev-> set_chip_select(false);
    return RSR[0];
}


static const uint16_t page_size_max = 256;
uint8_t *buffer;

    // number of bytes in a page
    uint32_t df_PageSize;
    // number of pages in a (generally 64k) block
    uint16_t df_PagePerBlock;
    // number of pages in a (generally 4k) sector
    uint16_t df_PagePerSector;
    // number of pages on the chip
    uint32_t df_NumPages;


uint8_t receivedata[25] = {0};

char* tran_buff = "0123/2854/7718/0010/2550/0080/0100/0050/3054/7918\n";


void setup()
{

    buffer = (uint8_t *)hal.util->malloc_type(page_size_max, AP_HAL::Util::MEM_DMA_SAFE);
    if (buffer == nullptr) {
        AP_HAL::panic("Out of DMA memory for logging");
    }

    dev = hal.spi->get_device("dataflash");
    if (!dev) {
        AP_HAL::panic("PANIC: AP_Logger SPIDeviceDriver not found");
        return;
    }

    dev_sem = dev->get_semaphore();

/*
    uint32_t t = AP_HAL::millis();
    while ((ReadStatusReg() & (JEDEC_STATUS_BUSY | JEDEC_STATUS_SRP0)) != 0) {
        hal.scheduler->delay_microseconds(100);
        if (AP_HAL::millis() - t > 5000) {
            printf("DataFlash: flash_died\n");
            break;
        }
    }
*/

    WITH_SEMAPHORE(dev_sem);
    //hal.console->printf("%s",(char*)buffer);
    // Read manufacturer ID
    uint8_t cmd = JEDEC_DEVICE_ID;
    dev->transfer(&cmd, 1, buffer, 4);
    printf("HELLO4\n");

    uint32_t id = buffer[0] << 16 | buffer[1] << 8 | buffer[2];
    //printf("HELLO5\n");
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
        //use_32bit_address = true;
        break;
    default:
        hal.scheduler->delay(2000);
        printf("Unknown SPI Flash 0x%08x\n", id);
        //return false;
    }


    hal.scheduler->delay(2000);
    df_PageSize = 256;
    df_NumPages = blocks * df_PagePerBlock;
    printf("SPI Flash 0x%08x found pages=%u erase=%uk\n",id, df_NumPages, (df_PagePerBlock * (uint32_t)df_PageSize)/1024);
   // hal.console->printf("SPI Flash 0x%08x found pages=%u erase=%uk\n",id, df_NumPages, (df_PagePerBlock * (uint32_t)df_PageSize)/1024);


hal.scheduler->delay(2000);

//erase_sector4KB(block0_addr);
//hal.scheduler->delay(100);
//
//W25_Write_Data(block0_addr,(uint8_t*)tran_buff,59);
//
//hal.scheduler->delay(100);
//W25_Read_Data(block0_addr,rx_buf,59);
//hal.scheduler->delay(100);
//hal.console->printf("%s",rx_buf);
//hal.scheduler->delay(100);


}

void loop()
{
	  dev_sem = dev->get_semaphore();
	    WITH_SEMAPHORE(dev_sem);

	hal.scheduler->delay(100);
	erase_sector4KB(block0_addr);
	hal.scheduler->delay(100);

	W25_Write_Data(block0_addr,(uint8_t*)tran_buff,59);

	hal.scheduler->delay(100);
	W25_Read_Data(block0_addr,rx_buf,59);

	hal.console->printf("%s",rx_buf);
    hal.console->printf("%u",page_size_max);
    hal.scheduler->delay(1000);
    hal.console->printf("*\n");
}

AP_HAL_MAIN();
