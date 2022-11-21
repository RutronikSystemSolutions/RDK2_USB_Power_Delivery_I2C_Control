/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the RutDevKit-PSoC62_USB_PD_Control
*              Application for ModusToolbox.
*
* Related Document: See README.md
*
*
*  Created on: 2021-06-04
*  Company: Rutronik Elektronische Bauelemente GmbH
*  Address: Jonavos g. 30, Kaunas 44262, Lithuania
*  Author: GDR
*
*******************************************************************************
* (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*
* Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
* including the software is for testing purposes only and,
* because it has limited functions and limited resilience, is not suitable
* for permanent use under real conditions. If the evaluation board is
* nevertheless used under real conditions, this is done at one’s responsibility;
* any liability of Rutronik is insofar excluded
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cypd3177.h"

/* Custom PDOs: */
#define PDO0 0x0201905A /* PDO 0 : 5V, 900mA*/
#define PDO1 0x0002D0C8 /*PDO 1 : 9V, 2000mA*/
//#define PDO1 0x00064064 /*PDO 1 : 20V, 1000mA*/

void handle_error(void);

int main(void)
{
    cy_rslt_t result;
    uint8_t i2c_data[16];
    uint16_t reg16;
    cypd3177_type_c_status_t c_status;
    cypd3177_pd_status_t pd_status;
    uint32_t pdo[2];

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    __enable_irq();

    /*Enable debug output via KitProg UART*/
    result = cy_retarget_io_init( KITPROG_TX, KITPROG_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }
    printf("\"RutDevKit-PSoc62 USB PD Control\" application is running.\r\n");
    printf("\x1b[2J\x1b[;H");

    /*Initialize LEDs*/
    result = cyhal_gpio_init( LED1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}
    result = cyhal_gpio_init( LED2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*I2C Peripheral Configuration*/
    result = cypd3177_i2c_init();
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }
    printf("I2C peripheral ready.\r\n");

    for (;;)
    {
    	/*Enter if CYPD3177 is active*/
        if(cypd3177_online())
        {
        	printf("CYPD3177 Is online.\r\n");

        	/*Read the Silicon ID*/
        	memset(i2c_data, 0x00, sizeof(i2c_data));
        	result = (cy_rslt_t)cypd3177_id( i2c_data);
        	if (result == CY_RSLT_SUCCESS)
        	{
            	reg16 = i2c_data[0] << 8;
            	reg16 |= i2c_data[1];
            	printf("CYPD3177 Silicon ID: 0x%04X\r\n", (unsigned int)(reg16));
        	}

        	/*Read the USB Bus Voltage*/
        	result = (cy_rslt_t)cypd3177_bus_voltage_mv( &reg16);
        	if (result == CY_RSLT_SUCCESS)
        	{
        		printf("USB Bus Voltage: %u mV\r\n", (unsigned int)(reg16));
        	}

        	/*Read the USB Type-C Status*/
        	result =  (cy_rslt_t)cypd3177_typec_status_read(&c_status);
            if (result == CY_RSLT_SUCCESS)
            {
            	if(c_status.port_partner_conn_status)
            	{
            		printf("Port is connected to partner.\r\n");
            		cyhal_gpio_write(LED1, CYBSP_LED_STATE_ON);
            	}
            	else
            	{
            		printf("Port is not connected to partner.\r\n");
            		cyhal_gpio_write(LED1, CYBSP_LED_STATE_OFF);
            	}
            }

            /*Read the USB Power Delivery Status*/
            result = (cy_rslt_t)cypd3177_pd_status_read(&pd_status);
            if (result == CY_RSLT_SUCCESS)
            {
            	/*Enter if explicit contract has been established*/
            	if(pd_status.explicit_contract)
            	{
            		printf("Explicit contract has been established with a power source.\r\n");

            		/*Rewrite and enable custom PDOs*/
            		pdo[0] = PDO0;
            		pdo[1] = PDO1;
            		result =  cypd3177_change_pdo(pdo);
            		if (result == CY_RSLT_SUCCESS)
            		{
            			CyDelay(1000);
            			printf("PDOs have been changed\r\n");

            			/*Report USB Bus voltage while explicit contract is valid*/
                    	while(pd_status.explicit_contract)
                    	{
                    		(cy_rslt_t)cypd3177_pd_status_read(&pd_status);
                        	result = (cy_rslt_t)cypd3177_bus_voltage_mv( &reg16);
                        	if (result == CY_RSLT_SUCCESS)
                        	{
                        		printf("USB Bus Voltage: %u mV\r\n", (unsigned int)(reg16));
                        	}
                        	CyDelay(1000);
                    	}
            		}
            	}
            }
        }
        else
        {
        	printf("The CYPD3177 USB PD device is not active. Please connect the USB-C cable.\r\n");
        	cyhal_gpio_write(LED1, CYBSP_LED_STATE_OFF);
        }
        CyDelay(1000);
    }
}

void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();
    cyhal_gpio_write(LED1, CYBSP_LED_STATE_OFF);
    cyhal_gpio_write(LED2, CYBSP_LED_STATE_ON);
    CY_ASSERT(0);
}

/* [] END OF FILE */
