#pragma once

#include <stdint.h>

#include "AP_HAL_Namespace.h"    // groups all the classes under one name AP_HAL
#include "AP_HAL_Boards.h"       // Macros to configure the underlying OS(Chibios/Linux/SITL)
                                 // and features to use for sensors

#include "AP_HAL_Macros.h"       //
#include "AP_HAL_Main.h"         //Macros which calls the main and setup code in application code.

/* HAL Module Classes (all pure virtual) */
#include "UARTDriver.h"
#include "AnalogIn.h"
#include "Storage.h"
#include "GPIO.h"
#include "RCInput.h"
#include "RCOutput.h"
#include "Scheduler.h"
#include "Semaphores.h"
#include "EventHandle.h"
#include "Util.h"
#include "OpticalFlow.h"
#include "Flash.h"
#include "DSP.h"

#include "CANIface.h"

#include "utility/BetterStream.h"

/* HAL Class definition */
#include "HAL.h"

#include "system.h"
