# PSOC6-GENERIC BSP

## Overview

This board support package is intended for creating custom PSoC™ 6 BSPs.



To use code from the BSP, simply include a reference to `cybsp.h`.

## Features

### Kit Features:

* This is a generic template, there is no corresponding physical board and hence no board-specific macros. The user is expected to create a custom BSP with various pin/hardware details - Refer to KBA230822. Code examples using kit/board resources will not be shown for this BSP until the manifest data for the BSP is updated to include additional capabilities. Refer to ModusToolbox user guide for creating custom manifests.
* This manifest can also be used to allow the board to show up in the ModusToolbox tools

### Kit Contents:

* NA

## BSP Configuration

The BSP has a few hooks that allow its behavior to be configured. Some of these items are enabled by default while others must be explicitly enabled. Items enabled by default are specified in the PSOC6-GENERIC.mk file. The items that are enabled can be changed by creating a custom BSP or by editing the application makefile.

Components:
* Device specific category reference (e.g.: CAT1) - This component, enabled by default, pulls in any device specific code for this board.

Defines:
* CYBSP_WIFI_CAPABLE - This define, disabled by default, causes the BSP to initialize the interface to an onboard wireless chip if it has one.
* CY_USING_HAL - This define, enabled by default, specifies that the HAL is intended to be used by the application. This will cause the BSP to include the applicable header file and to initialize the system level drivers.
* CYBSP_CUSTOM_SYSCLK_PM_CALLBACK - This define, disabled by default, causes the BSP to skip registering its default SysClk Power Management callback, if any, and instead to invoke the application-defined function `cybsp_register_custom_sysclk_pm_callback` to register an application-specific callback.

### Clock Configuration

| Clock    | Source    | Output Frequency |
|----------|-----------|------------------|
| FLL      | IMO       | 48.0 MHz         |
| PLL      | IMO       | 144.0 MHz        |
| CLK_HF0  | CLK_PATH1 | 144 MHz          |

### Power Configuration

* System Active Power Mode: LP
* System Idle Power Mode: Deep Sleep
* VDDA Voltage: 3300 mV
* VDDD Voltage: 3300 mV

See the [BSP Setttings][settings] for additional board specific configuration settings.

## API Reference Manual

The PSOC6-GENERIC Board Support Package provides a set of APIs to configure, initialize and use the board resources.

See the [BSP API Reference Manual][api] for the complete list of the provided interfaces.

## More information
* [PSOC6-GENERIC BSP API Reference Manual][api]
* [PSOC6-GENERIC Documentation](https://github.com/infineon/TARGET_PSOC6-GENERIC)
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Infineon GitHub](https://github.com/infineon)
* [ModusToolbox™](https://www.cypress.com/products/modustoolbox-software-environment)

[api]: https://infineon.github.io/TARGET_PSOC6-GENERIC/html/modules.html
[settings]: https://infineon.github.io/TARGET_PSOC6-GENERIC/html/md_bsp_settings.html

---
© Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation, 2019-2022.