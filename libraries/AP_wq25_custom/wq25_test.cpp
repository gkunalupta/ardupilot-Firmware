/*
 * wq25_test.cpp
 *
 *  Created on: 26-Aug-2021
 *      Author: kunal
 */



#include <AP_HAL/AP_HAL.h>
#include <AP_Common/AP_Common.h>
#include <stdio.h>

#include "wq25_custom.h"

uint8_t rx_buf[1025];

uint8_t receivedata[25] = {0};

char* tran_buff = "0123/2854/7718/0010/2550/0080/0100/0050/3054/7918\n";


void setup();
void loop();

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

AP_Wq25_custom wq25;

void setup()
{

//    buffer = (uint8_t *)hal.util->malloc_type(page_size_max, AP_HAL::Util::MEM_DMA_SAFE);
//    if (buffer == nullptr) {
//        AP_HAL::panic("Out of DMA memory for logging");
//    }

	wq25.wq25_init();


}

void loop()
{
	  dev_sem = dev->get_semaphore();
	  //  WITH_SEMAPHORE(dev_sem);

	hal.scheduler->delay(100);
	wq25.erase_sector4KB(block0_addr);
	hal.scheduler->delay(100);

	wq25.W25_Write_Data(block0_addr,(uint8_t*)tran_buff,59);

	hal.scheduler->delay(100);
	wq25.W25_Read_Data(block0_addr,rx_buf,59);

	hal.console->printf("%s",rx_buf);
   // hal.console->printf("%u",page_size_max);
    hal.scheduler->delay(1000);
    hal.console->printf("*\n");
}

AP_HAL_MAIN();
