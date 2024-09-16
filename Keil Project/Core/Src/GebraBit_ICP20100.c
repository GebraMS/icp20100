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

#include "GebraBit_ICP20100.h"
#define ICP20100_FLUSH_FIFO                   1
/************************************************
 *         MSB Bit Start Location Begin         *
 ***********************************************/ 
#define START_MSB_BIT_AT_0                    0
#define START_MSB_BIT_AT_1                    1
#define START_MSB_BIT_AT_2                    2
#define START_MSB_BIT_AT_3                    3
#define START_MSB_BIT_AT_4                    4
#define START_MSB_BIT_AT_5                    5
#define START_MSB_BIT_AT_6                    6
#define START_MSB_BIT_AT_7                    7
/*----------------------------------------------*
 *        MSB Bit Start Location End            *
 *----------------------------------------------*/ 
/************************************************
 *          Bit Field Length Begin              *
 ***********************************************/ 
#define BIT_LENGTH_1                          1
#define BIT_LENGTH_2                          2
#define BIT_LENGTH_3                          3
#define BIT_LENGTH_4                          4
#define BIT_LENGTH_5                          5
#define BIT_LENGTH_6                          6
#define BIT_LENGTH_7                          7
#define BIT_LENGTH_8                          8
/*----------------------------------------------*
 *          Bit Field Length End                *
 *----------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
/*=========================================================================================================================================
 * @brief     Read data from spacial register.
 * @param     regAddr Register Address of ICP20100
 * @param     data    Pointer to Variable that register value is saved .
 * @return    stat    Return status
 ========================================================================================================================================*/
	uint8_t	GB_ICP20100_Read_Reg_Data (uint8_t regAddr, uint8_t* data)
{	
	uint8_t txBuf[4] = {ICP20100_SPI_READ_CMD ,regAddr , 0x00}; 
	uint8_t rxBuf[4]={0 ,0 , 0};
	HAL_StatusTypeDef stat = HAL_ERROR ;


	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	stat = (HAL_SPI_TransmitReceive(&hspi1, txBuf, rxBuf, 3, HAL_MAX_DELAY));
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	if (stat == HAL_OK)
	{
		*data = rxBuf[2];
	}
	return stat;
}
/*========================================================================================================================================= 
 * @brief     Read multiple data from first spacial register address.
 * @param     regAddr First Register Address of ICP20100 that reading multiple data start from this address
 * @param     data    Pointer to Variable that multiple data is saved .
 * @param     bytepcs Quantity of data that we want to read .
 * @return    stat    Return status
 ========================================================================================================================================*/
uint8_t GB_ICP20100_Burst_Read(uint8_t regAddr,uint8_t *data, uint8_t bytepcs)
{
	uint8_t *pTxBuf;
	uint8_t *pRxBuf;
	uint8_t status = HAL_ERROR;
	pTxBuf = ( uint8_t * )malloc(sizeof(uint8_t) * (bytepcs + 2)); // reason of "+2" is ICP20100_SPI_READ_CMD and register address
	pRxBuf = ( uint8_t * )malloc(sizeof(uint8_t) * (bytepcs + 2));
	memset(pTxBuf, 0, (bytepcs + 2)*sizeof(uint8_t));

	pTxBuf[0] = ICP20100_SPI_READ_CMD;
  pTxBuf[1] = regAddr;

	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	status = (HAL_SPI_TransmitReceive(&hspi1, pTxBuf, pRxBuf, bytepcs+2, HAL_MAX_DELAY));
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	
	if (status == HAL_OK)
	{
		memcpy(data, &pRxBuf[2], bytepcs*sizeof(uint8_t)); 
	}
	free(pTxBuf);
	free(pRxBuf);
	return status;
}
/*========================================================================================================================================= 
 * @brief     Read data from spacial bits of a register.
 * @param     regAddr     Register Address of ICP20100 .
 * @param     start_bit   Start Bit location .(0 to 7)
 * @param     len         Quantity of Bits want to read(1 to 8) 
 * @param     data        Pointer to Variable that register Bits value is saved .
 * @return    status      Return status
 ========================================================================================================================================*/
uint8_t GB_ICP20100_Read_Reg_Bits (uint8_t regAddr, uint8_t start_bit, uint8_t len, uint8_t* data)
{
	uint8_t status = HAL_ERROR;
	uint8_t tempData = 0;

	if (len>8 || start_bit>7)
	{
		return HAL_ERROR;
	}

	if (GB_ICP20100_Read_Reg_Data(regAddr, &tempData) == HAL_OK)
	{
		uint8_t mask = ((1 << len) - 1) << (start_bit - len + 1); //formula for making a broom of 1&0 for gathering desired bits
		tempData &= mask; // zero all non-important bits in data
		tempData >>= (start_bit - len + 1); //shift data to zero position
		*data = tempData;
		status = HAL_OK;
	}
	else
	{
		status = HAL_ERROR;
		*data = 0;
	}
	return status;
}
/*=========================================================================================================================================
 * @brief     Write data to spacial register.
 * @param     regAddr Register Address of ICP20100
 * @param     data    Value that will be writen to register .
 * @return    stat    Return status
 ========================================================================================================================================*/
uint8_t	GB_ICP20100_Write_Reg_Data (uint8_t regAddr, uint8_t data)
{	
	uint8_t txxBuf[4] = {ICP20100_SPI_WRITE_CMD ,regAddr , data}; 
	uint8_t rxxBuf[4]={0 ,0 , 0};
	HAL_StatusTypeDef stat = HAL_ERROR ;
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	stat = (HAL_SPI_TransmitReceive(&hspi1, txxBuf, rxxBuf, 3, HAL_MAX_DELAY));
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	return stat;	
}
/*=========================================================================================================================================
 * @brief     Write data to spacial bits of a register.
 * @param     regAddr     Register Address of ICP20100 .
 * @param     start_bit   Start Bit location .(0 to 7)
 * @param     len         Quantity of Bits want to write(1 to 8) 
 * @param     data        Value that will be writen to register bits .
 * @return    status      Return status
 ========================================================================================================================================*/
uint8_t GB_ICP20100_Write_Reg_Bits(uint8_t regAddr, uint8_t start_bit, uint8_t len, uint8_t data)
{
	uint8_t txBuf[4];
	uint8_t rxBuf[4]={0 ,0 , 0};
	HAL_StatusTypeDef status = HAL_ERROR ;
	uint8_t tempData = 0;

	if (len>8 || start_bit>7)
	{
		return HAL_ERROR;
	}

	if (GB_ICP20100_Read_Reg_Data(regAddr, &tempData) == HAL_OK)
	{
		uint8_t mask = ((1 << len) - 1) << (start_bit - len + 1);
		data <<= (start_bit - len + 1); // shift data into correct position
		data &= mask; // zero all non-important bits in data
		tempData &= ~(mask); // zero all important bits in existing byte
		tempData |= data; // combine data with existing byte
    txBuf[0] = ICP20100_SPI_WRITE_CMD;
		txBuf[1] = regAddr;
		txBuf[2] = tempData;
	
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
		status = (HAL_SPI_TransmitReceive(&hspi1, txBuf, rxBuf, 3, HAL_MAX_DELAY));
		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	}
	return status;
}
/*=========================================================================================================================================
 * @brief     Get Device ID.
 * @param     icp20100 ICP20100 Struct Device_ID variable
 * @return    Nothing
 ========================================================================================================================================*/
void	GB_ICP20100_Get_Device_ID (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Read_Reg_Data (DEVICE_ID, &icp20100->Device_ID);
}
/*=========================================================================================================================================
 * @brief     Get Device Version
 * @param     icp20100   ICP20100 Struct Version variable
 * @return    Nothing
 ========================================================================================================================================*/
void	GB_ICP20100_Get_Device_Version (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Read_Reg_Data (VERSION,&icp20100->Version);
}
/*=========================================================================================================================================
 * @brief     Read Mode_Select register
 * @param     icp20100   ICP20100 Struct Mode_Select variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_Mode_Select (GebraBit_ICP20100 * icp20100)
{

  GB_ICP20100_Read_Reg_Data (MODE_SELECT, &icp20100->Mode_Select);	
	
}
/*=========================================================================================================================================
 * @brief     Write Mode_Select register
 * @param     icp20100   ICP20100 Struct Mode_Select variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_Mode_Select (GebraBit_ICP20100 * icp20100)
{
  do 
	 {
		GB_ICP20100_MODE_SYNC_STATUS_Check(icp20100); 
		if ( icp20100->Mode_Sync_Status== SYNC_TO_CLK )
			break;
		HAL_Delay(1);
	 }while(1);
  GB_ICP20100_Write_Reg_Data(MODE_SELECT, icp20100->Mode_Select );	
}
/*=========================================================================================================================================
 * @brief     Read Meas Config Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct MEAS_CONFIG variable
 * @return    Nothing
 ========================================================================================================================================*/
void	GB_ICP20100_Read_Meas_Config (GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_7,BIT_LENGTH_3,&icp20100->MEAS_CONFIG);
}
/*=========================================================================================================================================
 * @brief     Write Meas Config Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct MEAS_CONFIG variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_Meas_Config ( GebraBit_ICP20100 * icp20100)
{
	do 
	 {
		GB_ICP20100_MODE_SYNC_STATUS_Check(icp20100); 
		if ( icp20100->Mode_Sync_Status== SYNC_TO_CLK )
			break;
		HAL_Delay(1);
	 }while(1);
	GB_ICP20100_Write_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_7,BIT_LENGTH_3,icp20100->MEAS_CONFIG); 
}	
/*=========================================================================================================================================
 * @brief     Read Forced_Measure_Trig Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct FORCED_MEAS_TRIGGER variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_Forced_Measure_Trig (GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_4,BIT_LENGTH_1,&icp20100->FORCED_MEAS_TRIGGER);
}
/*=========================================================================================================================================
 * @brief     Write Forced_Measure_Trig Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct FORCED_MEAS_TRIGGER variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_Forced_Measure_Trig ( GebraBit_ICP20100 * icp20100)
{
 do 
	 {
		GB_ICP20100_MODE_SYNC_STATUS_Check(icp20100); 
		if ( icp20100->Mode_Sync_Status == SYNC_TO_CLK )
			break;
		HAL_Delay(1);
	 }while(1);
	GB_ICP20100_Write_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_4,BIT_LENGTH_1,icp20100->FORCED_MEAS_TRIGGER); 
}
/*=========================================================================================================================================
 * @brief     Read Measure_Mode Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct MEAS_MODE variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_Measure_Mode (GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_3,BIT_LENGTH_1,&icp20100->MEAS_MODE);
}
/*=========================================================================================================================================
 * @brief     Write Measure_Mode Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct MEAS_MODE variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_Measure_Mode (GebraBit_ICP20100 * icp20100)
{
 do 
	 {
		GB_ICP20100_MODE_SYNC_STATUS_Check(icp20100);
		if ( icp20100->Mode_Sync_Status == SYNC_TO_CLK )
			break;
		HAL_Delay(1);
	 }while(1);
	GB_ICP20100_Write_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_3,BIT_LENGTH_1,icp20100->MEAS_MODE); 
}
/*=========================================================================================================================================
 * @brief     Read Power_Mode Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct POWER_MODE variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_Power_Mode (GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_2,BIT_LENGTH_1,&icp20100->POWER_MODE);
}
/*=========================================================================================================================================
 * @brief     Write Power_Mode Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct POWER_MODE variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_Power_Mode (GebraBit_ICP20100 * icp20100)
{
 do 
	 {
		GB_ICP20100_MODE_SYNC_STATUS_Check(icp20100);
		if ( icp20100->Mode_Sync_Status == SYNC_TO_CLK )
			break;
		HAL_Delay(1);
	 }while(1);
	 
	GB_ICP20100_Write_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_2,BIT_LENGTH_1,icp20100->POWER_MODE); 
}
/*=========================================================================================================================================
 * @brief     Read FIFO_Readout_Mode Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct FIFO_READOUT_MODE variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_FIFO_Readout_Mode (GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_1,BIT_LENGTH_2,&icp20100->FIFO_READOUT_MODE);
}
/*=========================================================================================================================================
 * @brief     Write FIFO_Readout_Mode Bits of Mode_Select register
 * @param     icp20100   ICP20100 Struct FIFO_READOUT_MODE variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_FIFO_Readout_Mode (GebraBit_ICP20100 * icp20100)
{
 do 
	 {
		GB_ICP20100_MODE_SYNC_STATUS_Check(icp20100); 
		if ( icp20100->Mode_Sync_Status== SYNC_TO_CLK )
			break;
		HAL_Delay(1);
	 }while(1);
 
	GB_ICP20100_Write_Reg_Bits(MODE_SELECT,START_MSB_BIT_AT_1,BIT_LENGTH_2,icp20100->FIFO_READOUT_MODE); 
}
/*=========================================================================================================================================
 * @brief     Write Interrupt_Status register
 * @param     icp20100   ICP20100 Struct INTERUPT_STATUS variable
 * @return    Nothing
 ========================================================================================================================================*/
/*
M403Z 
*/
void	  GB_ICP20100_Write_Interrupt_Status (GebraBit_ICP20100 * icp20100)
{
	GB_ICP20100_Write_Reg_Data(INTERRUPT_STATUS,icp20100->INTERUPT_STATUS );
}
/*=========================================================================================================================================
 * @brief     Read Interrupt_Status register
 * @param     icp20100   ICP20100 Struct INTERUPT_STATUS variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Get_Interrupt_Triggered_Source (GebraBit_ICP20100 * icp20100)
{
	GB_ICP20100_Read_Reg_Data (INTERRUPT_STATUS, &icp20100->INTERUPT_STATUS);
}
/*=========================================================================================================================================
 * @brief     Clear Interrupt_Status register first by reading it and when get triggered source clear it by writing "1" to them
 * @param     icp20100   ICP20100 Struct INTERUPT_STATUS variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Clear_Interrupt_Source_Bit ( GebraBit_ICP20100 * icp20100)
{
  
  GB_ICP20100_Get_Interrupt_Triggered_Source(icp20100) ;	
	GB_ICP20100_Write_Interrupt_Status(icp20100);
}
/*=========================================================================================================================================
 * @brief     Read Interrupt_Mask register
 * @param     icp20100   ICP20100 Struct INTERUPT_MASK variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_Interrupt_Mask (GebraBit_ICP20100 * icp20100)
{	
	GB_ICP20100_Read_Reg_Data (INTERRUPT_MASK, &icp20100->INTERUPT_MASK);		
}
/*=========================================================================================================================================
 * @brief     Write Interrupt_Mask register
 * @param     icp20100   ICP20100 Struct INTERUPT_MASK variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_Interrupt_Mask (GebraBit_ICP20100 * icp20100)
{
	GB_ICP20100_Write_Reg_Data(INTERRUPT_MASK,icp20100->INTERUPT_MASK );
}
/*=========================================================================================================================================
 * @brief     Disable all Interrupt sources
 * @param     icp20100   ICP20100 Struct INTERUPT_MASK variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Mask_All_Interrupt (GebraBit_ICP20100 * icp20100)
{
  icp20100->INTERUPT_MASK=ALL_INT_MASK;
	GB_ICP20100_Write_Interrupt_Mask (icp20100);
}
/*=========================================================================================================================================
 * @brief     Enable Specific Interrupt sources
 * @param     icp20100   ICP20100 Struct INTERUPT_MASK variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_NOT_Mask_Source_Interrupt (GebraBit_ICP20100 * icp20100)
{

 uint8_t nmask = 0;	
 switch(icp20100->INTERUPT_MASK)
	 {
	  case FIFO_OVERFLOW_NOT_MASK:
		nmask=0xEE;
    break;
		case FIFO_UNDERFLOW_NOT_MASK:
		nmask=0xED;
    break;	
		case FIFO_WMK_HIGH_NOT_MASK:
		nmask=0xEB;
    break;	
		case FIFO_WMK_LOW_NOT_MASK:
		nmask=0xE7;
    break;	
		case PRESS_ABS_NOT_MASK:
		nmask=0xCF;
    break;	
		case PRESS_DELTA_NOT_MASK:
		nmask=0xAF;
    break;		
		default:
		nmask=0;			
	 }
	GB_ICP20100_Write_Interrupt_Mask(icp20100);
	
}
/*=========================================================================================================================================
 * @brief     Read FIFO_Congig register
 * @param     icp20100   ICP20100 Struct FIFO_Congig variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_FIFO_Congig (GebraBit_ICP20100 * icp20100)
{

  GB_ICP20100_Read_Reg_Data (FIFO_CONFIG, &icp20100->FIFO_Congig);
	GB_ICP20100_Read_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_7,BIT_LENGTH_4,&icp20100->FIFO_WM_HIGH);
	GB_ICP20100_Read_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_3,BIT_LENGTH_4,&icp20100->FIFO_WM_LOW);	
	
}
/*=========================================================================================================================================
 * @brief     Write FIFO_Congig register
 * @param     icp20100   ICP20100 Struct FIFO_Congig variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_FIFO_Congig (GebraBit_ICP20100 * icp20100)
{
  //GB_ICP20100_Write_Reg_Data(FIFO_CONFIG, icp20100->FIFO_Congig );
	GB_ICP20100_Write_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_7,BIT_LENGTH_4,icp20100->FIFO_WM_HIGH);
	GB_ICP20100_Write_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_3,BIT_LENGTH_4,icp20100->FIFO_WM_LOW);
}
/*=========================================================================================================================================
 * @brief     Read FIFO_WM_HIGH register
 * @param     icp20100   ICP20100 Struct FIFO_WM_HIGH variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_FIFO_WM_HIGH (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Read_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_7,BIT_LENGTH_4,&icp20100->FIFO_WM_HIGH);
}
/*=========================================================================================================================================
 * @brief     Read FIFO_WM_LOW register
 * @param     icp20100   ICP20100 Struct FIFO_WM_LOW variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_FIFO_WM_LOW (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Read_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_3,BIT_LENGTH_4,&icp20100->FIFO_WM_LOW);
}
/*=========================================================================================================================================
 * @brief     Write FIFO_WM_HIGH register
 * @param     icp20100   ICP20100 Struct FIFO_WM_HIGH variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_FIFO_WM_HIGH (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Write_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_7,BIT_LENGTH_4,icp20100->FIFO_WM_HIGH);
}
/*=========================================================================================================================================
 * @brief     Write FIFO_WM_LOW register
 * @param     icp20100   ICP20100 Struct FIFO_WM_LOW variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_FIFO_WM_LOW (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Write_Reg_Bits(FIFO_CONFIG,START_MSB_BIT_AT_3,BIT_LENGTH_4,icp20100->FIFO_WM_LOW);
}
/*=========================================================================================================================================
 * @brief     Read FIFO_Fill register
 * @param     icp20100   ICP20100 Struct FIFO_Fill variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_FIFO_Fill (GebraBit_ICP20100 * icp20100)
{

  GB_ICP20100_Read_Reg_Data (FIFO_FILL, &icp20100->FIFO_Fill);

}
/*=========================================================================================================================================
 * @brief     Write FIFO_Fill register
 * @param     icp20100   ICP20100 Struct FIFO_Fill variable
 * @return    Nothing
 ========================================================================================================================================*/
 void	  GB_ICP20100_Write_FIFO_Fill (GebraBit_ICP20100 * icp20100)
 {
 
  GB_ICP20100_Write_Reg_Data(FIFO_FILL, icp20100->FIFO_Fill );
 
 }
/*=========================================================================================================================================
 * @brief     Clear all fifo datas
 * @return    Nothing
 ========================================================================================================================================*/
void GB_ICP20100_FIFO_Flush (void)
{
 GB_ICP20100_Write_Reg_Bits(FIFO_FILL,START_MSB_BIT_AT_7,BIT_LENGTH_1,ICP20100_FLUSH_FIFO);	
}
/*=========================================================================================================================================
 * @brief     Check if FIFO is Empty
 * @param     icp20100   ICP20100 Struct FIFO_Empty variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_FIFO_Empty_Check ( GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(FIFO_FILL,START_MSB_BIT_AT_6,BIT_LENGTH_1,&icp20100->FIFO_Empty);
}
/*=========================================================================================================================================
 * @brief     Check if FIFO is Full
 * @param     icp20100   ICP20100 Struct FIFO_Full variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_FIFO_Full_Check ( GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(FIFO_FILL,START_MSB_BIT_AT_5,BIT_LENGTH_1,&icp20100->FIFO_Full);
}
/*=========================================================================================================================================
 * @brief     Get how many packets is stored in FIFO (0 to 16) 
 * @param     icp20100   ICP20100 Struct FIFO_LEVEL variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_FIFO_FILL_LEVEL ( GebraBit_ICP20100 * icp20100)
{
 GB_ICP20100_Read_Reg_Bits(FIFO_FILL,START_MSB_BIT_AT_4,BIT_LENGTH_5,&icp20100->FIFO_LEVEL);
}
/*=========================================================================================================================================
 * @brief     Read PRESS_ABS lsb and msb register
 * @param     icp20100   ICP20100 Struct PRESS_ABS variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_PRESS_ABS (GebraBit_ICP20100 * icp20100)
{
	uint8_t lsb=0,msb=0;
	GB_ICP20100_Read_Reg_Data (PRESS_ABS_LSB, &lsb);
	GB_ICP20100_Read_Reg_Data (PRESS_ABS_MSB, &msb);
  icp20100->PRESS_ABS = (msb << 8) | lsb ;
}
/*=========================================================================================================================================
 * @brief     Write PRESS_ABS lsb and msb register
 * @param     icp20100   ICP20100 Struct PRESS_ABS variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_PRESS_ABS (GebraBit_ICP20100 * icp20100)
{	
  GB_ICP20100_Write_Reg_Data (PRESS_ABS_LSB, (uint8_t) (icp20100->PRESS_ABS & 0xff));
	GB_ICP20100_Write_Reg_Data (PRESS_ABS_MSB, (uint8_t) ((icp20100->PRESS_ABS >> 8) & 0xff));
}
/*=========================================================================================================================================
 * @brief     Read PRESS_DELTA lsb and msb register
 * @param     icp20100   ICP20100 Struct PRESS_DELTA variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_PRESS_DELTA (GebraBit_ICP20100 * icp20100)
{
	uint8_t lsb=0,msb=0;
	GB_ICP20100_Read_Reg_Data (PRESS_DELTA_LSB, &lsb);
	GB_ICP20100_Read_Reg_Data (PRESS_DELTA_MSB, &msb);
  icp20100->PRESS_DELTA = (msb << 8) | lsb ;
}
/*=========================================================================================================================================
 * @brief     Write PRESS_DELTA lsb and msb register
 * @param     icp20100   ICP20100 Struct PRESS_DELTA variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Write_PRESS_DELTA (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Write_Reg_Data (PRESS_DELTA_LSB, (uint8_t) (icp20100->PRESS_ABS & 0xff));
	GB_ICP20100_Write_Reg_Data (PRESS_DELTA_MSB, (uint8_t) ((icp20100->PRESS_ABS >> 8) & 0xff));
}
/*=========================================================================================================================================
 * @brief     Making sure the previous mode is selected by reading the register field MODE_SYNC_STATUS of register
   DEVICE_STATUS until it is set to ‘1’.
 * @param     icp20100   ICP20100 Struct Mode_Sync_Status variable
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_MODE_SYNC_STATUS_Check ( GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Read_Reg_Bits(DEVICE_STATUS,START_MSB_BIT_AT_0,BIT_LENGTH_1,&icp20100->Mode_Sync_Status);
}
/*=========================================================================================================================================
 * @brief     Read FIFO data.Start address of FIFO depends on Mode of FIFO readout
 * @param     icp20100   ICP20100 Struct FIFO_OFFSET_Address variable.Determine start address of FIFO
 *                       ICP20100 Struct FIFO_Data variable that all data store here.
 *                       ICP20100 Struct FIFO_Byte_Qty variable.how many Bytes must read from FIFO
 * @return    Nothing
 ========================================================================================================================================*/
void	  GB_ICP20100_Read_FIFO (GebraBit_ICP20100 * icp20100 )
{
  GB_ICP20100_Burst_Read((FIFO_BASE+icp20100->FIFO_OFFSET_Address), icp20100->FIFO_Data, icp20100->FIFO_Byte_Qty);
}
/*========================================================================================================================================= 
 * @brief     Configure ICP20100 according to GebraBit_ICP20100 Structure values .
 * @param     icp20100  ICP20100 Structure important variable
 * @return    Nothing
 ========================================================================================================================================*/
void   GB_ICP20100_Config(GebraBit_ICP20100 * icp20100)
{
	GB_ICP20100_Get_Device_ID (icp20100);
	GB_ICP20100_Get_Device_Version (icp20100);
	GB_ICP20100_Write_FIFO_Congig (icp20100);
	GB_ICP20100_FIFO_Flush();
	GB_ICP20100_Write_Interrupt_Mask(icp20100);
  GB_ICP20100_Write_Mode_Select(icp20100);
	GB_ICP20100_Write_Meas_Config (icp20100);
	GB_ICP20100_Write_Forced_Measure_Trig (icp20100);
	GB_ICP20100_Write_Power_Mode (icp20100);
	GB_ICP20100_Write_FIFO_Readout_Mode(icp20100);
	GB_ICP20100_Write_Measure_Mode(icp20100);	
}

/*========================================================================================================================================= 
 * @brief     Check if ICP20100 needs OTP Configuration
 * @param     icp20100   ICP20100 Struct OTP_Config variable
 * @return    Nothing
 ========================================================================================================================================*/
void	GB_ICP20100_OTP_Bootup_Check ( GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Get_Device_Version(icp20100);
	if(icp20100->Version==VERSION_B)
		icp20100->OTP_Config = NO_NEED_OTP_CONFIG;
	else
    icp20100->OTP_Config = NEED_OTP_CONFIG;		
}
/*========================================================================================================================================= 
 * @brief     Reset ICP20100 GebraBit_ICP20100 Structure values .
 * @param     icp20100  ICP20100 Structure important variable
 * @return    Nothing
 ========================================================================================================================================*/
void	GB_ICP20100_Soft_Reset (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Write_Mode_Select (icp20100);
	HAL_Delay(2);
	GB_ICP20100_FIFO_Flush();
	GB_ICP20100_Write_FIFO_Congig (icp20100);
	GB_ICP20100_Mask_All_Interrupt (icp20100);
	GB_ICP20100_Clear_Interrupt_Source_Bit(icp20100);	
}
/*========================================================================================================================================= 
 * @brief     Set ICP20100 to standby.
 * @param     icp20100  ICP20100 Structure 
 * @return    Nothing
 ========================================================================================================================================*/	
void	GB_ICP20100_To_Standby (GebraBit_ICP20100 * icp20100)
{
  GB_ICP20100_Write_Forced_Measure_Trig(icp20100);
	GB_ICP20100_Write_Power_Mode(icp20100);
	GB_ICP20100_Write_Measure_Mode(icp20100);
	GB_ICP20100_FIFO_Flush();	
}
/*========================================================================================================================================= 
 * @brief     Get ICP20100 data From FIFO.
 * @param     icp20100  ICP20100 Structure FIFO_Data Variable.
 * @return    Nothing
 ========================================================================================================================================*/	
void	  GB_ICP20100_Get_FIFO_Data (GebraBit_ICP20100 * icp20100)
{
  icp20100->FIFO_OFFSET_Address = (( icp20100->FIFO_READOUT_MODE == ICP20100_FIFO_READOUT_MODE_PRES_ONLY )|| (icp20100->FIFO_READOUT_MODE == ICP20100_FIFO_READOUT_MODE_TEMP_ONLY)) ? 3 : 0;
  icp20100->FIFO_Byte_Qty= icp20100->FIFO_Packet_Qty *2*3;///6 Byte(press0+press1+press2+temp0+temp1+temp2)
	GB_ICP20100_Read_FIFO (icp20100);
}
/*========================================================================================================================================= 
 * @brief     Divide ICP20100 FIFO data to temperature and Pressure data according to FIFO Readout mode .
 * @param     icp20100  ICP20100 Structure Raw_Temp_Data and Raw_Press_Data Variable.
 * @return    Nothing
 ========================================================================================================================================*/	
void GB_ICP20100_Raw_Data_Partition(GebraBit_ICP20100 * icp20100)
{
	uint8_t i,offset=0;

	for ( i = 0 ; i < icp20100->FIFO_Packet_Qty ; i++ )
	{
		if ( icp20100->FIFO_READOUT_MODE == ICP20100_FIFO_READOUT_MODE_PRES_TEMP)
		{
			icp20100->Raw_Press_Data[i] = (int32_t)((( icp20100->FIFO_Data[offset+2] & 0x0f) << 16) | (icp20100->FIFO_Data[offset+1] << 8) | icp20100->FIFO_Data[offset]) ;
			offset += 3;// each complete pressure and temperature data has 3 byte 
			icp20100->Raw_Temp_Data[i] = (int32_t)(((icp20100->FIFO_Data[offset+2] & 0x0f) << 16) | (icp20100->FIFO_Data[offset+1] << 8) | icp20100->FIFO_Data[offset]) ;
			offset += 3;
		} else if ( icp20100->FIFO_READOUT_MODE == ICP20100_FIFO_READOUT_MODE_TEMP_ONLY) {
			icp20100->Raw_Temp_Data[i] = (int32_t)(((icp20100->FIFO_Data[offset+2] & 0x0f) << 16) | (icp20100->FIFO_Data[offset+1] << 8) | icp20100->FIFO_Data[offset]) ;
			offset += 3;
		} else if( icp20100->FIFO_READOUT_MODE == ICP20100_FIFO_READOUT_MODE_TEMP_PRES) {
			icp20100->Raw_Temp_Data[i] = (int32_t)(((icp20100->FIFO_Data[offset+2] & 0x0f) << 16) | (icp20100->FIFO_Data[offset+1] << 8) | icp20100->FIFO_Data[offset]) ;
			offset += 3;
			icp20100->Raw_Press_Data[i] = (int32_t)((( icp20100->FIFO_Data[offset+2] & 0x0f) << 16) | (icp20100->FIFO_Data[offset+1] << 8) | icp20100->FIFO_Data[offset]) ;
			offset += 3;
		} else if( icp20100->FIFO_READOUT_MODE == ICP20100_FIFO_READOUT_MODE_PRES_ONLY) {
			icp20100->Raw_Press_Data[i] = (int32_t)((( icp20100->FIFO_Data[offset+2] & 0x0f) << 16) | (icp20100->FIFO_Data[offset+1] << 8) | icp20100->FIFO_Data[offset]) ;
			offset += 3;
		}
	}
}
/*========================================================================================================================================= 
 * @brief     Convert ICP20100 temperature and Pressure raw data to valid by changing from two's complement and use of datasheet formula .
 * @param     icp20100  ICP20100 Structure Valid_Temp_Data and Valid_Press_Data Variable.
 * @return    Nothing
 ========================================================================================================================================*/	
void GB_ICP20100_Valid_Temp_Press_Data(GebraBit_ICP20100 * icp20100)
{
	
	uint8_t i;
	for ( i = 0 ; i < icp20100->FIFO_Packet_Qty ; i++)
	{
			/** P = (POUT/2^17)*40kPa + 70kPa **/
			if (icp20100->Raw_Press_Data[i] & 0x080000 )//20 bit data output
		  	icp20100->Raw_Press_Data[i] |= 0xFFF00000;	
        //raw_pressure[i] &= 0x7FFFF;
	  	  //raw_pressure[i] -= 524288;			
		  icp20100->Valid_Press_Data[i] = ((float)(icp20100->Raw_Press_Data[i]) *400 /131072) +700;	//datasheet formula 		
			/* T = (TOUT/2^18)*65C + 25C */
			if (icp20100->Raw_Temp_Data[i] & 0x080000 )//20 bit data output
			 icp20100->Raw_Temp_Data[i] |= 0xFFF00000;		
       //raw_temperature[i] &= 0x7FFFF;
	  	 //raw_temperature[i] -= 524288;				
			icp20100->Valid_Temp_Data[i] = ((float)( icp20100->Raw_Temp_Data[i] )*65 /262144 ) + 25;//datasheet formula 
	}
}
