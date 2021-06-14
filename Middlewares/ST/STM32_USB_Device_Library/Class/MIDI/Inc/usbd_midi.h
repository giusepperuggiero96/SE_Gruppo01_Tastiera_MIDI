/**
  ******************************************************************************
  * @file    usbd_MIDI.h
  * @author  Andrea Marchetta, Giuseppe Ruggiero, Marco Barletta, Riccardo Corvi
  * @brief   header file for the usbd_MIDI.c file.
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_MIDI_H
#define __USB_MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_MIDI
  * @brief This file is the Header file for usbd_MIDI.c
  * @{
  */

/** @defgroup USB_MIDI_Exported_Functions
  * @{
  */
USBD_StatusTypeDef USBD_MIDI_SendMsg(USBD_HandleTypeDef *pdev, uint8_t *msg, uint16_t length);

//User callbacks for midi driver
USBD_StatusTypeDef MIDI_DataRx(uint8_t *msg, uint16_t length);



/** @defgroup USBD_MIDI_Exported_Macros
  * @{
  */
#define APP_RX_DATA_SIZE               2048 
/* Total size of IN buffer:
APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */

#define		MIDI_OUT_EP					0x01
#define		MIDI_IN_EP					0x81
#define 	MIDI_DATA_IN_PACKET_SIZE	0x40
#define 	MIDI_DATA_OUT_PACKET_SIZE	0x40

#define 	MIDI_IN_FRAME_INTERVAL		1

#define USBD_MIDI_CLASS &USBD_MIDI

#define USB_TX_STATE_EMPTY    0
#define USB_TX_STATE_FULL     1

/** @defgroup USBD_MIDI_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_MIDI;


/** @defgroup USBD_MIDI_Exported_TypesDefinitions
  * @{
  */
typedef struct
{
   uint32_t APP_Rx_ptr_in;
	 uint32_t APP_Rx_ptr_out;
	 uint32_t APP_Rx_length;
	 uint8_t  USB_Tx_State;
   uint32_t FrameCount;
	 uint8_t USB_Rx_Buffer   [MIDI_DATA_OUT_PACKET_SIZE];
	 uint8_t APP_Rx_Buffer   [APP_RX_DATA_SIZE];
   USBD_StatusTypeDef (*Rx_callback)		(uint8_t *msg, uint16_t length);
} USBD_MIDI_HandleTypeDef;


#ifdef __cplusplus
}
#endif

#endif  /* __USB_MIDI_H */
/**
  * @}
  */

/******************************END OF FILE****/
