/*
 * cypd3177.h
 *
 *  Created on: 2021-06-04
 *      Author: GDR
 */

#ifndef CYPD3177_H_
#define CYPD3177_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define CYPD3177_ADDR				0x08
#define CYPD_SILICON_ID				0x0420

#define CYPD_DEVICE_MODE_REG		0x0000
#define CYPD_SILICON_ID_REG			0x0002
#define CYPD_INTERRUPT_REG			0x0006
#define CYPD_PD_STATUS_REG			0x1008
#define CYPD_TYPE_C_STATUS_REG		0x100C
#define CYPD_BUS_VOLTAGE_REG		0x100D
#define CYPD_CURRENT_PDO_REG		0x1010
#define CYPD_CURRENT_RDO_REG		0x1014
#define CYPD_SWAP_RESPONSE_REG		0x1028
#define CYPD_EVENT_STATUS_REG		0x1044
#define CYPD_READ_GPIO_LEVEL_REG	0x0082
#define CYPD_SAMPLE_GPIO_REG		0x0083
#define CYPD_WRITE_DATA_MEM_REG		0x1800

#define CYPD_RESET_CMD				0x0008
#define CYPD_EVENT_MASK_CMD			0x1024
#define CYPD_DM_CONTROL_CMD			0x1000
#define CYPD_SELECT_SINK_PDO_CMD	0x1005
#define CYPD_PD_CONTROL_CMD			0x1006
#define CYPD_REQUEST_CMD			0x1050
#define CYPD_SET_GPIO_MODE_CMD		0x0080
#define CYPD_SET_GPIO_LEVEL_CMD		0x0081

#define CYPD_DEV_RESPONSE_CMD		0x007E
#define CYPD_PD_RESPONSE_CMD		0x1400

#define CYPD_DEVICE_ACTIVE			0x95
#define CYPD_DEVICE_INT				0x01
#define CYPD_PD_PORT_INT			0x02
#define CYPD_PORT_CONNECTED			0x01
#define CYPD_CC_POLARITY			0x02
#define CYPD_ATT_DEV_TYPE			0x1C
#define CYPD_CURR_LEVEL				0xC0
#define CYPD_CONTRACT_STATE			0x400
#define CYPD_SINK_TX				0x4000
#define CYPD_PE_STATE				0x8000

/*Exported Global Variables*/
extern cyhal_i2c_t I2C_scb3;
extern cyhal_i2c_cfg_t I2C_cfg;

typedef enum
{
	NO_ATT = 0x00,
	SOURCE_ATT = 0x02,
	DEBUG_ACC_ATT = 0x03
}att_dev_type;

typedef enum
{
	LEVEL_900,
	LEVEL_1500,
	LEVEL_3000,
	LEVEL_RES
}curr_level;

typedef enum
{
	POL_CC1,
	POL_CC2
}cc_pol;

typedef struct
{
	_Bool device_int;
	_Bool pd_port_int;
}cypd3177_int_t;

typedef struct
{
	_Bool port_partner_conn_status;
	cc_pol cc_polarity;
	att_dev_type att_dev_type;
	curr_level current_level;

}cypd3177_type_c_status_t;

typedef struct
{
	_Bool explicit_contract;
	_Bool SinkTxOk;
	_Bool PE_SNK_Ready;

}cypd3177_pd_status_t;

/*Exported functions*/
uint32_t cypd3177_i2c_init(void);
uint32_t cypd3177_write(uint16_t reg, uint8_t* data, uint16_t size);
uint32_t cypd3177_read(uint16_t reg, uint8_t* data, uint16_t size);
_Bool cypd3177_online(void);
uint32_t cypd3177_id( uint8_t* data);
uint32_t cypd3177_bus_voltage_mv( uint16_t* voltage);
uint32_t cypd3177_int_read(cypd3177_int_t* status);
uint32_t cypd3177_typec_status_read(cypd3177_type_c_status_t* status);
uint32_t cypd3177_pd_status_read(cypd3177_pd_status_t* status);
uint32_t cypd3177_change_pdo(uint32_t* pdo);

#endif /* CYPD3177_H_ */
