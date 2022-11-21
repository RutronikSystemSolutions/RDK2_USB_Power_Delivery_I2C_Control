/*
 * cypd3177.c
 *
 *  Created on: 2021-06-04
 *      Author: GDR
 */

#include "cypd3177.h"
#include "stdlib.h"

/*Global variables*/
cyhal_i2c_t I2C_scb3;
cyhal_i2c_cfg_t I2C_cfg;

/*Initializes the I2C connected with CYPD3177*/
uint32_t cypd3177_i2c_init(void)
{
	cy_rslt_t result;

    I2C_cfg.is_slave = false;
    I2C_cfg.address = 0;
    I2C_cfg.frequencyhal_hz = 100000UL;
    result = cyhal_i2c_init(&I2C_scb3, ARDU_SDA, ARDU_SCL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
    	return (uint32_t)result;
    }
    result = cyhal_i2c_configure(&I2C_scb3, &I2C_cfg);

	return (uint32_t)result;
}

/*Write to CYPD3177 over the I2C*/
uint32_t cypd3177_write(uint16_t reg, uint8_t* data, uint16_t size)
{
	cy_rslt_t result;
	uint8_t* i2c_data = NULL;

	/*Allocate buffer for transmission*/
    i2c_data = malloc(size+2);
    if(i2c_data == NULL)
    {
    	return 1;
    }

    /*Copy register address and all the data*/
    i2c_data[0] = reg;
    i2c_data[1] = reg >> 8;
    memcpy(&i2c_data[2], data, size);

    /*Execute write command*/
	result = cyhal_i2c_master_write( &I2C_scb3, CYPD3177_ADDR, i2c_data, size+2, 100, true );

	/*Free transmission buffer and exit*/
	free(i2c_data);
	return (uint32_t)result;
}

/*Read from CYPD3177 over the I2C*/
uint32_t cypd3177_read(uint16_t reg, uint8_t* data, uint16_t size)
{
	cy_rslt_t result;
	uint8_t i2c_data[2];

    i2c_data[0] = reg;
    i2c_data[1] = reg >> 8;
	result = cyhal_i2c_master_write( &I2C_scb3, CYPD3177_ADDR, i2c_data, 2, 100, false );
    if (result != CY_RSLT_SUCCESS)
    {
    	return (uint32_t)result;
    }

    result = cyhal_i2c_master_read( &I2C_scb3, CYPD3177_ADDR, data, size, 100, true );

	return (uint32_t)result;
}

/*Check if CYPD3177 device is responsive*/
_Bool cypd3177_online(void)
{
	uint32_t result;
	uint8_t data = 0;

	result = cypd3177_read(CYPD_DEVICE_MODE_REG, &data, 1);
	if(result == 0)
	{
		if(data == CYPD_DEVICE_ACTIVE)
		{
			return true;
		}
	}

	return false;
}

/*Read the CYPD3177 ID register*/
uint32_t cypd3177_id( uint8_t* data)
{
	uint32_t result;

	result = cypd3177_read(CYPD_SILICON_ID_REG, data, 2);

	return result;
}

/*Read the USB PD Bus Voltage*/
uint32_t cypd3177_bus_voltage_mv( uint16_t* voltage)
{
	uint32_t result;
	uint8_t data;

	result = cypd3177_read(CYPD_BUS_VOLTAGE_REG, &data, 1);

	*voltage = data*100;

	return result;
}

/*Read the interrupt source and status*/
uint32_t cypd3177_int_read(cypd3177_int_t* status)
{
	uint32_t result;
	uint8_t data;

	result = cypd3177_read(CYPD_INTERRUPT_REG, &data, 1);

	status->device_int = (_Bool)(data & CYPD_DEVICE_INT);

	status->pd_port_int = (_Bool)((data & CYPD_PD_PORT_INT) >> 1);

	return result;
}

/*Read the Type-C Status Register*/
uint32_t cypd3177_typec_status_read(cypd3177_type_c_status_t* status)
{
	uint32_t result;
	uint8_t data[4]= {0,0,0,0};

	result = cypd3177_read(CYPD_TYPE_C_STATUS_REG, data, 4);

	status->port_partner_conn_status = (_Bool)(data[0] & CYPD_PORT_CONNECTED);

	status->cc_polarity = (cc_pol)((data[0] & CYPD_CC_POLARITY) >> 1);

	status->att_dev_type = (att_dev_type)((data[0] & CYPD_ATT_DEV_TYPE) >> 2);

	status->current_level = (curr_level)((data[0] & CYPD_CURR_LEVEL) >> 6);

	return result;
}

/*Read The PD Status Register*/
uint32_t cypd3177_pd_status_read(cypd3177_pd_status_t* status)
{
	uint32_t result;
	union pd_status_union
	{
		uint8_t data8[4];
		uint32_t data32;
	}pd_status;

	result = cypd3177_read(CYPD_PD_STATUS_REG, pd_status.data8, 4);

	status->explicit_contract = (_Bool)((pd_status.data32 & CYPD_CONTRACT_STATE) >> 10);

	status->SinkTxOk = (_Bool)((pd_status.data32 & CYPD_SINK_TX) >> 14);

	status->PE_SNK_Ready = (_Bool)((pd_status.data32 & CYPD_PE_STATE) >> 15);

	return result;
}

/*Reprogram the PDOs*/
uint32_t cypd3177_change_pdo(uint32_t* pdo)
{
	uint32_t result;
	union pdo_union
	{
		uint8_t data8[12];
		uint32_t data32[3];
	}pdo_data;

	pdo_data.data32[0] = 0x534E4B50;
	pdo_data.data32[1] = pdo[0];
	pdo_data.data32[2] = pdo[1];

	/*Write memory region in 0x1800*/
	result = cypd3177_write(CYPD_WRITE_DATA_MEM_REG, pdo_data.data8, sizeof(pdo_data.data8));
	if(result != 0)
	{
		return result;
	}

	/*Select Sink PDOs*/
	pdo_data.data8[0] = 0x03;
	result = cypd3177_write(CYPD_SELECT_SINK_PDO_CMD, &pdo_data.data8[0], 1);

	return result;
}


