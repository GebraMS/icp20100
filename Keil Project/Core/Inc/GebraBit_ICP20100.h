/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2020 GebraBit Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively “Software”) is subject
 * to GebraBit and its licensors' intellectual property rights under U.S. and international copyright
 * and other intellectual property rights laws. 
 *
 * GebraBit and its licensors retain all intellectual property and proprietary rights in and to the Software
 * and any use, reproduction, disclosure or distribution of the Software without an express license agreement
 * from GebraBit is strictly prohibited.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT IN  
 * NO EVENT SHALL GebraBit BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, 
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 * @Author       	: Mehrdad Zeinali
 * ________________________________________________________________________________________________________
 */
#ifndef	__ICP_20100_H__
#define	__ICP_20100_H__

#include "main.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "i2c.h"
#include "spi.h" 
#ifdef __cplusplus
extern "C" {
#endif
/***********************************************
 *              Register Address Begin         *
 ***********************************************/
#define TRIM1_MSB						      0X05
#define TRIM2_LSB						      0X06
#define TRIM2_MSB					      	0X07
#define DEVICE_ID					      	0X0C
#define IO_DRIVE_STRENGTH					0X0D
#define OTP_CONFIG1				      	0XAC
#define OTP_MR_LSB					      0XAD
#define OTP_MR_MSB				    	  0XAE
#define OTP_MRA_LSB				    	  0XAF
#define OTP_MRA_MSB			    		  0XB0
#define OTP_MRB_LSB					      0XB1
#define OTP_MRB_MSB					      0XB2
#define OTP_ADDRESS_REG				   	0XB5
#define OTP_COMMAND_REG					  0XB6
#define OTP_RDDATA					      0XB8
#define OTP_STATUS				        0xB9
#define OTP_DBG2						      0XBC
#define MASTER_LOCK						    0XBE
#define OTP_STATUS2				        0XBF
#define MODE_SELECT			          0xC0
#define INTERRUPT_STATUS          0xC1
#define INTERRUPT_MASK		        0xC2
#define FIFO_CONFIG			          0xC3
#define FIFO_FILL			            0xC4
#define SPI_MODE				          0xC5
#define PRESS_ABS_LSB		          0xC7
#define PRESS_ABS_MSB		          0xC8
#define PRESS_DELTA_LSB           0xC9
#define PRESS_DELTA_MSB           0xCA
#define DEVICE_STATUS		          0xCD
#define I3C_INFO				          0xCE
#define VERSION   			          0XD3
#define FIFO_BASE                 0XFA
//#define PRESS_DATA_0            0XFA
//#define PRESS_DATA_1            0XFB
//#define PRESS_DATA_2            0XFC
//#define TEMP_DATA_0             0XFD
//#define TEMP_DATA_1             0XFE
//#define TEMP_DATA_2             0XFF
/*----------------------------------------------*
 *            Register Address End              *
 *----------------------------------------------*/
 /***********************************************
 *              SPI Command Begin               *
 ***********************************************/ 
#define ICP20100_SPI_WRITE_CMD   0X33
#define ICP20100_SPI_READ_CMD    0X3C
/*----------------------------------------------*
 *               SPI Command End                *
 *----------------------------------------------*/ 
/************************************************
 *          Data Buffer Size Begin              *
 ***********************************************/ 
#define  FIFO_DATA_BUFFER_SIZE               200
#define  PRESS_TEMP_DATA_BUFFER_SIZE         12
/*----------------------------------------------*
 *          Data Buffer Size End                *
 *----------------------------------------------*/ 
/*
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */

/*************************************************
 * Values For MEAS_CONFIG in MODE_SELECT Register *
 **************************************************/ 
typedef enum measurement_configuration
{  
	ICP20100_OP_MODE0 = 0 ,                      /* Mode 0: Bw:6.25 Hz ODR: 25Hz FIR FILTER:YES IIR FILTER:NO */
	ICP20100_OP_MODE1     ,                      /* Mode 1: Bw:30 Hz ODR: 120Hz  FIR FILTER:YES IIR FILTER:NO */
	ICP20100_OP_MODE2     ,                      /* Mode 2: Bw:10 Hz ODR: 40Hz   FIR FILTER:YES IIR FILTER:NO */
	ICP20100_OP_MODE3     ,                      /* Mode 3: Bw:0.5 Hz ODR: 2Hz   FIR FILTER:YES IIR FILTER:NO */ 
	ICP20100_OP_MODE4     ,                      /* Mode 4: User configurable Mode */ 
	ICP20100_OP_MODE_RES
}ICP20100_Measurement_Configuration;
 /*************************************************
 * Values For FORCED_MEAS_TRIGGER in MODE_SELECT Register *
 **************************************************/
typedef enum forced_measure_trig {
	ICP20100_FORCE_MEAS_STANDBY = 0,			       /* Stay in Stand by */
	ICP20100_FORCE_MEAS_TRIGGER = 1	             /* Trigger for forced measurements (only supported for Mode4) */
}ICP20100_Forced_Measure_Trig;
 /*************************************************
 * Values For MEAS_MODE in MODE_SELECT Register   *
 **************************************************/ 
typedef enum measure_mode
{
	ICP20100_MEAS_MODE_FORCED_TRIGGER = 0,      /* Standby or trigger forced measurement based on the field FORCED_MEAS_TRIGGER **/
	ICP20100_MEAS_MODE_CONTINUOUS     = 1       /* Continuous Measurements (duty cycled): Measurements are started based on the selected mode ODR_REG*/
}ICP20100_Measure_Mode;
 /*************************************************
 * Values For POWER_MODE in MODE_SELECT Register  *
 **************************************************/
typedef enum power_mode
{
	ICP20100_POWER_NORMAL_MODE = 0,             /* Normal Mode: Device is in standby and goes to active mode during the execution of a measurement */
	ICP20100_POWER_ACTIVE_MODE = 1              /* Active Mode: Power on DVDD and enable the high frequency clock */
}ICP20100_Power_Mode;
 /*************************************************
 *       Values For MODE_SELECT Register          *
 **************************************************/
typedef enum FIFO_readout_mode
{
	ICP20100_FIFO_READOUT_MODE_PRES_TEMP = 0,   /* Pressure and temperature as pair and address wraps to the start address of the Pressure value ( pressure first ) :When you start reading from address 0xFA with address increment, you will read out press(n), temp(n), press(n+1), temp(n+1), … */
	ICP20100_FIFO_READOUT_MODE_TEMP_ONLY = 1,   /* Temperature only reporting :When you start reading from address 0xFD with address increment, you will read out temp(n), temp(n+1), … */
	ICP20100_FIFO_READOUT_MODE_TEMP_PRES = 2,   /* Pressure and temperature as pair and address wraps to the start address of the Temperature value ( Temperature first ) : When you start reading from address 0xFA with address increment, you will read out temp(n), press(n), temp(n+1), press(n+1), … */
	ICP20100_FIFO_READOUT_MODE_PRES_ONLY = 3    /* Pressure only reporting : When you start reading from address 0xFD with address increment, you will read out press(n), press(n+1), … */
}ICP20100_FIFO_Readout_Mode;
 /*************************************************
 *       Values For INTERRUPT_STATUS Register     *
 **************************************************/
typedef enum interrupt_source
{  
	FIFO_OVERFLOW_INT  = 1  ,             /* A new pressure/temperature pair was written to the FIFO while it was full. The interrupt has triggered */
	FIFO_UNDERFLOW_INT = 2  ,             /* A new pressure/temperature pair was fetched from the FIFO while it was empty. The interrupt has triggered */
	FIFO_WMK_HIGH_INT  = 4  ,             /* The FIFO fill level reached in upward direction the programmed watermark high value. The interrupt has triggered */
	FIFO_WMK_LOW_INT   = 8  ,             /* The FIFO fill level reached in downward direction the programmed watermark low value. The interrupt has triggered */ 
	PRESS_ABS_INT      = 32 ,             /* The pressure value crossed the programmed pressure underrun/overrun value. The interrupt has triggered */ 
	PRESS_DELTA_INT    = 64 ,             /* The difference between 2 consecutive pressure measurements after filtering exceeded the programmed delta pressure overrun value. The interrupt has triggered */
	ALL_INT_SOURCE     = 111
}ICP20100_Interrupt_Source;
 /*************************************************
 *       Values For INTERRUPT_MASK Register       *
 **************************************************/
typedef enum interrupt_mask
{  
	FIFO_OVERFLOW_NOT_MASK  = 0xEE ,      /* All Interrupt Sources Are Masked But FIFO_OVERFLOW interrupt is not masked  */
	FIFO_UNDERFLOW_NOT_MASK = 0xED ,      /* All Interrupt Sources Are Masked But FIFO_UNDERFLOW interrupt is not masked */
	FIFO_WMK_HIGH_NOT_MASK  = 0xEB ,      /* All Interrupt Sources Are Masked But FIFO_WMK_HIGH interrupt is not masked */
	FIFO_WMK_LOW_NOT_MASK   = 0xE7 ,      /* All Interrupt Sources Are Masked But FIFO_WMK_LOW interrupt is not masked */ 
	PRESS_ABS_NOT_MASK      = 0xCF ,      /* All Interrupt Sources Are Masked But PRESS_ABS interrupt is not masked */
	PRESS_DELTA_NOT_MASK    = 0xAF ,      /* All Interrupt Sources Are Masked But PRESS_DELTA interrupt is not masked */
	ALL_INT_MASK            = 0xEF ,      /* All Interrupt Sources Are Masked */
	ALL_INT_NOT_MASK        = 0
}ICP20100_Interrupt_Mask;
 /*************************************************
 * Values For FIFO_EMPTY in FIFO_FILL Register    *
 **************************************************/

typedef enum FIFO_empty_flag
{
	NOT_EMPTY_FIFO = 0,                   /* 0: The FIFO level is above 0 */
	FIFO_IS_EMPTY  = 1                    /* 1: The FIFO level is at 0*/
}ICP20100_FIFO_Empty_Flag;
 /*************************************************
 * Values For FIFO_FULL in FIFO_FILL Register     *
 **************************************************/
typedef enum FIFO_full_flag
{
	NOT_FULL_FIFO = 0,                    /* 0: The FIFO level is below the FIFO size */
	FIFO_IS_FULL  = 1                     /* 1: The FIFO level has reached the FIFO size*/
}ICP20100_FIFO_Full_Flag;
 /*************************************************
 * Values For FIFO_LEVEL in FIFO_FILL Register     *
 **************************************************/
typedef enum FIFO_fill_level
{
	ICP20100_FIFO_FILL_LEVEL_Empty = 0,   /* 00000: Empty      */
	ICP20100_FIFO_FILL_LEVEL_1_16  = 1,   /* 00001: 1/16 full  */
	ICP20100_FIFO_FILL_LEVEL_2_16  = 2,   /* 00010: 2/16 full  */
	ICP20100_FIFO_FILL_LEVEL_3_16  = 3,   /* 00011: 3/16 full  */
	ICP20100_FIFO_FILL_LEVEL_4_16  = 4,   /* 00100: 4/16 full  */
	ICP20100_FIFO_FILL_LEVEL_5_16  = 5,   /* 00101: 5/16 full  */
	ICP20100_FIFO_FILL_LEVEL_6_16  = 6,   /* 00110: 6/16 full  */
	ICP20100_FIFO_FILL_LEVEL_7_16  = 7,   /* 00111: 7/16 full  */
	ICP20100_FIFO_FILL_LEVEL_8_16  = 8,   /* 01000: 8/16 full  */
	ICP20100_FIFO_FILL_LEVEL_9_16  = 9,   /* 01001: 9/16 full  */
	ICP20100_FIFO_FILL_LEVEL_10_16 = 10,  /* 01010: 10/16 full */
	ICP20100_FIFO_FILL_LEVEL_11_16 = 11,  /* 01011: 11/16 full */
	ICP20100_FIFO_FILL_LEVEL_12_16 = 12,  /* 01100: 12/16 full */
	ICP20100_FIFO_FILL_LEVEL_13_16 = 13,  /* 01101: 13/16 full */
	ICP20100_FIFO_FILL_LEVEL_14_16 = 14,  /* 01110: 14/16 full */
	ICP20100_FIFO_FILL_LEVEL_15_16 = 15,  /* 01111: 15/16 full */
	ICP20100_FIFO_FILL_LEVEL_FULL  = 16   /* 10000: 16/16 full */
}ICP20100_FIFO_Fill_Level;
 /*************************************************
 *       Values For DEVICE_STATUS Register        *
 **************************************************/
typedef enum mode_sync_status
{
	NOT_SYNC_TO_CLK = 0,                  /* 0: Synchronization of the selected mode to the internal clock domain is ongoing. MODE_SELECT register is not accessible by the user.*/
	SYNC_TO_CLK     = 1                   /* 1: Synchronization of the selected mode to the internal clock domain is finished. MODE_SELECT register is accessible by the user.*/
}ICP20100_Mode_Sync_Status;
 /*************************************************
 *       Values For VERSION Register              *
 **************************************************/
typedef enum device_version
{
	VERSION_A  = 0x00,                    /* ICP20100 OTP Config need    */
	VERSION_B  = 0xB2                     /* ICP20100 OTP Config not need */
}ICP20100_Device_Version;

typedef enum otp_config
{
	NEED_OTP_CONFIG     = 0,   
	NO_NEED_OTP_CONFIG  = 1   
}ICP20100_OTP_Config;
 /*************************************************
 *   Defining ICP20100 Register&Data As Struct    *
 **************************************************/
typedef	struct icp20100
{
	ICP20100_Measurement_Configuration MEAS_CONFIG;
  ICP20100_Forced_Measure_Trig FORCED_MEAS_TRIGGER;
  ICP20100_Measure_Mode MEAS_MODE;
  ICP20100_Power_Mode POWER_MODE;
  ICP20100_FIFO_Readout_Mode FIFO_READOUT_MODE;
  ICP20100_Interrupt_Source INTERUPT_STATUS;
  ICP20100_Interrupt_Mask INTERUPT_MASK;
	ICP20100_FIFO_Fill_Level FIFO_LEVEL;
	ICP20100_Mode_Sync_Status Mode_Sync_Status;
	ICP20100_Device_Version Version;
	ICP20100_OTP_Config OTP_Config;
	ICP20100_FIFO_Empty_Flag FIFO_Empty;
	ICP20100_FIFO_Full_Flag  FIFO_Full;
	uint8_t Device_ID;
	uint8_t Mode_Select;
	uint8_t FIFO_Fill;
	uint8_t FIFO_Congig;
	uint8_t FIFO_WM_HIGH;
	uint8_t FIFO_WM_LOW;
	int16_t PRESS_DELTA;
	int16_t PRESS_ABS;
	uint8_t FIFO_Packet_Qty;
	uint8_t FIFO_Byte_Qty;
	uint8_t FIFO_OFFSET_Address;
	uint8_t FIFO_Data[FIFO_DATA_BUFFER_SIZE];
  int32_t Raw_Temp_Data[PRESS_TEMP_DATA_BUFFER_SIZE];
	int32_t Raw_Press_Data[PRESS_TEMP_DATA_BUFFER_SIZE];
	float   Valid_Press_Data[PRESS_TEMP_DATA_BUFFER_SIZE];
  float   Valid_Temp_Data[PRESS_TEMP_DATA_BUFFER_SIZE];
}GebraBit_ICP20100;
/*
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
 
/********************************************************
 *Declare Read&Write ICP20100 Register Vlalues Functions*
 ********************************************************/
extern	uint8_t	GB_ICP20100_Read_Reg_Data                  (uint8_t regAddr, uint8_t* data);
extern  uint8_t GB_ICP20100_Burst_Read                     (uint8_t regAddr,uint8_t *data, uint8_t bytepcs);
extern	uint8_t GB_ICP20100_Read_Reg_Bits                  (uint8_t regAddr, uint8_t start_bit, uint8_t len, uint8_t* data);
extern	uint8_t	GB_ICP20100_Write_Reg_Data                 (uint8_t regAddr, uint8_t data);
extern  uint8_t GB_ICP20100_Write_Reg_Bits                 (uint8_t regAddr, uint8_t start_bit, uint8_t len, uint8_t data);
/********************************************************
 *       Declare ICP20100 Configuration Functions       *
 ********************************************************/
extern	void	  GB_ICP20100_Get_Device_ID 							   (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_Get_Device_Version   		 			 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_Mode_Select     					 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_Mode_Select  		  		   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_Meas_Config     					 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_Meas_Config     		     (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_Forced_Measure_Trig 			 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_Forced_Measure_Trig			 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_Measure_Mode      			   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_Measure_Mode       			 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_Power_Mode 	  	  		 	 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_Power_Mode			 				 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_PRESS_ABS  						 	 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_PRESS_ABS 						 	 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_PRESS_DELTA 						   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_PRESS_DELTA 						 (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_MODE_SYNC_STATUS_Check 				 (GebraBit_ICP20100 * icp20100);
/********************************************************
 *         Declare ICP20100 Interrupt Functions         *
 ********************************************************/
extern	void	  GB_ICP20100_Get_Interrupt_Triggered_Source (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_Write_Interrupt_Status 				 (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_Clear_Interrupt_Source_Bit 		 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_Interrupt_Mask 				   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_Interrupt_Mask 				   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Mask_All_Interrupt		  	     (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_NOT_Mask_Source_Interrupt 	 	 (GebraBit_ICP20100 * icp20100);
/********************************************************
 *          Declare ICP20100 FIFO Functions             *
 ********************************************************/
extern  void	  GB_ICP20100_Read_FIFO 									   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_FIFO_Readout_Mode  	     (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_FIFO_Readout_Mode  			 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_FIFO_Congig       				 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_FIFO_Congig 						 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_FIFO_WM_HIGH				 		   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_FIFO_WM_LOW 						 	 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_FIFO_WM_HIGH  					 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_FIFO_WM_LOW 					   (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Read_FIFO_Fill 								 (GebraBit_ICP20100 * icp20100);
extern  void	  GB_ICP20100_Write_FIFO_Fill  							 (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_FIFO_Empty_Check 							 (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_FIFO_Full_Check 		           (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_FIFO_FILL_LEVEL 			 			 	 (GebraBit_ICP20100 * icp20100);
extern	void	  GB_ICP20100_FIFO_Flush								   	 (void);
/********************************************************
 *        Declare ICP20100 High Level Functions         *
 ********************************************************/
extern void	GB_ICP20100_OTP_Bootup_Check         (GebraBit_ICP20100 * icp20100);
extern void	GB_ICP20100_Soft_Reset               (GebraBit_ICP20100 * icp20100);
extern void	GB_ICP20100_To_Standby               (GebraBit_ICP20100 * icp20100);
extern void GB_ICP20100_Config                   (GebraBit_ICP20100 * icp20100);
extern void	GB_ICP20100_Get_FIFO_Data            (GebraBit_ICP20100 * icp20100);
extern void GB_ICP20100_Raw_Data_Partition       (GebraBit_ICP20100 * icp20100);
extern void GB_ICP20100_Valid_Temp_Press_Data    (GebraBit_ICP20100 * icp20100);
#endif /* __ICP_20100_H__ */
