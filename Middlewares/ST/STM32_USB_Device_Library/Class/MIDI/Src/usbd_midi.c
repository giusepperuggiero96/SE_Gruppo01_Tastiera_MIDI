/**
  ******************************************************************************
  * @file    USBD_MIDI.c
  * @author  Andrea Marchetta, Giuseppe Ruggiero, Marco Barletta, Riccardo Corvi
  * @brief   This file provides the MIDI core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                MIDI Class  Description
  *          ===================================================================
  *           This driver manages the MIDI Class 1.0 following the "USB Device Class Definition for
  *           MIDI Devices V1.0 '99".
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Standard AC Interface Descriptor management
  *             - 1 MIDI Streaming Interface
  *             - 2 MIDI OUT JACS
  *             - 2 MIDI IN JACKS
  *             - 1 MIDI IN Streaming Endpoint
  *             - 1 MIDI OUT Streaming Endpoint
  *             - MIDI Class-Specific AC Interfaces
  *             - No control requests are supported yet (such GET_CUR or SET_CUR)
  *             - MIDI channel multiplexing through the interface file
  *             - Several helper function with frequent messages through interface file
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
  */


/* Includes ------------------------------------------------------------------*/
#include "usbd_midi.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_MIDI
  * @brief USBD core module
  * @{
  */

/** @defgroup USBD_MIDI_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_FunctionPrototypes
  * @{
  */
static uint8_t  USBD_MIDI_Init        (USBD_HandleTypeDef  *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DeInit      (USBD_HandleTypeDef  *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DataIn      (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_DataOut     (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_SOF         (USBD_HandleTypeDef *pdev);
static uint8_t  USBD_MIDI_Setup       (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length); // Done
static uint8_t  *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t  *USBD_MIDI_GetDeviceQualifierDesc(uint16_t *length);

static USBD_StatusTypeDef MIDI_DataTx(USBD_HandleTypeDef *pdev, uint8_t *msg, uint16_t length);
static void USBD_Handle_init(USBD_MIDI_HandleTypeDef* midi);
static void Handle_USBAsynchXfer  (USBD_HandleTypeDef *pdev);

/**
  * @}
  */

/** @defgroup USBD_MIDI_Private_Variables
  * @{
  */
/* The interface class calbacks for the MIDI driver */
USBD_ClassTypeDef USBD_MIDI =
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  USBD_MIDI_Setup,
  NULL,// USBD_MIDI_EP0_TxReady,
  NULL,// USBD_MIDI_EP0_RxReady,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  USBD_MIDI_SOF,
  NULL,//USBD_MIDI_IsoINIncomplete,
  NULL,//USBD_MIDI_IsoOutIncomplete,
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetCfgDesc,     //(??)
  USBD_MIDI_GetOtherSpeedCfgDesc,
  USBD_MIDI_GetDeviceQualifierDesc,//USBD_MIDI_GetDeviceQualifierDesc,
};

/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MIDI_CfgDesc[101]  __ALIGN_END =
{
		// configuration descriptor
		        0x09, 0x02, 0x65, 0x00, 0x02, 0x01, 0x00, 0xc0, 0x50,

		        // The Audio Interface Collection
		        0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
		        0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
		        0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
		        0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor

		        // MIDI IN JACKS
		        0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
		        0x06, 0x24, 0x02, 0x02, 0x02, 0x00,

		        // MIDI OUT JACKS
		        0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
		        0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,

		        // OUT endpoint descriptor
		        0x09, 0x05, MIDI_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
		        0x05, 0x25, 0x01, 0x01, 0x01,

		        // IN endpoint descriptorUSB_HID_CONFIG_DESC_SIZ
		        0x09, 0x05, MIDI_IN_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
		        0x05, 0x25, 0x01, 0x01, 0x03,
} ;

/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_MIDI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

USBD_StatusTypeDef USBD_MIDI_SendMsg(USBD_HandleTypeDef *pdev, uint8_t *msg, uint16_t length)
{
    return MIDI_DataTx(pdev,msg, length);
}

USBD_StatusTypeDef MIDI_DataTx(USBD_HandleTypeDef *pdev, uint8_t *msg, uint16_t length)
{
	uint32_t i = 0;

  USBD_MIDI_HandleTypeDef *midi;
  midi = (USBD_MIDI_HandleTypeDef *)pdev->pClassData;

  if (midi == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

	while (i < length) {
		midi->APP_Rx_Buffer[midi->APP_Rx_ptr_in] = *(msg + i);
		midi->APP_Rx_ptr_in++;
		i++;
		/* To avoid buffer overflow */
		if (midi->APP_Rx_ptr_in == APP_RX_DATA_SIZE) {
			midi->APP_Rx_ptr_in = 0;
		}
	}
	return USBD_OK;
}
/* This is the callback function that is called
 * whenever a MIDI message is revieved.
 */
__weak USBD_StatusTypeDef MIDI_DataRx(uint8_t *msg, uint16_t length){
	return USBD_OK;
}

static uint8_t USBD_MIDI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  UNUSED(pdev);
  UNUSED(req);
  return (uint8_t) USBD_OK;
}

static void USBD_Handle_init(USBD_MIDI_HandleTypeDef* midi)
{
  midi->APP_Rx_ptr_in = 0;
  midi->APP_Rx_ptr_out = 0;
  midi->APP_Rx_length = 0;
  midi->USB_Tx_State = 0;
  midi->Rx_callback = MIDI_DataRx;
  midi->FrameCount = 0;
}

static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef  *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

  USBD_MIDI_HandleTypeDef *midi;

  midi = USBD_malloc(sizeof(USBD_MIDI_HandleTypeDef));


  if (midi == NULL)
  {
    pdev->pClassData = NULL;
    return (uint8_t)USBD_EMEM;
  }

  USBD_Handle_init(midi);
  pdev->pClassData = (void *)midi;

	/* Open the in EP */
	// DCD_EP_Open(pdev,
	//               MIDI_IN_EP,
	//               MIDI_DATA_IN_PACKET_SIZE,
	//               USB_OTG_EP_BULK);
  (void)USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK, MIDI_DATA_IN_PACKET_SIZE);
  pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 1U;

	/* Open the out EP */
	// DCD_EP_Open(pdev,
	// 			  MIDI_OUT_EP,
	// 			  MIDI_DATA_OUT_PACKET_SIZE,
	// 			  USB_OTG_EP_BULK);
  (void)USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK, MIDI_DATA_OUT_PACKET_SIZE);
  pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 1U;

	/* Prepare Out endpoint to receive next packet */
	// DCD_EP_PrepareRx(pdev,
	//                    CDC_OUT_EP,
	//                    (uint8_t*)(USB_Rx_Buffer),
	//                    MIDI_DATA_OUT_PACKET_SIZE);
  (void)USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP ,(uint8_t*)(midi->USB_Rx_Buffer), MIDI_DATA_OUT_PACKET_SIZE);

	return (uint8_t) USBD_OK;
}

static uint8_t  USBD_MIDI_DeInit      (USBD_HandleTypeDef  *pdev, uint8_t cfgidx){
  UNUSED(cfgidx);
  (void)USBD_LL_CloseEP(pdev, MIDI_IN_EP);
  pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 0U;
  (void)USBD_LL_CloseEP(pdev, MIDI_OUT_EP);
  pdev->ep_in[MIDI_OUT_EP & 0xFU].is_used = 0U;
	// DCD_EP_Close(pdev,
	// 		MIDI_IN_EP);
	// DCD_EP_Close(pdev,
	// 		MIDI_OUT_EP);
  /* Free allocated memory */
  if (pdev->pClassData != NULL) {
    (void)USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
	return (uint8_t) USBD_OK;
}


static uint8_t  USBD_MIDI_DataIn      (USBD_HandleTypeDef *pdev, uint8_t epnum){
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;
  USBD_MIDI_HandleTypeDef* midi;

  midi = (USBD_MIDI_HandleTypeDef *)pdev->pClassData;

  if (midi == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

	if (midi->USB_Tx_State == USB_TX_STATE_FULL) {
		if (midi->APP_Rx_length == 0) {
			midi->USB_Tx_State = USB_TX_STATE_EMPTY;
		} else {
			if (midi->APP_Rx_length > MIDI_DATA_IN_PACKET_SIZE){
				USB_Tx_ptr = midi->APP_Rx_ptr_out;
				USB_Tx_length = MIDI_DATA_IN_PACKET_SIZE;

				midi->APP_Rx_ptr_out += MIDI_DATA_IN_PACKET_SIZE;
				midi->APP_Rx_length -= MIDI_DATA_IN_PACKET_SIZE;
			}	else {
				USB_Tx_ptr = midi->APP_Rx_ptr_out;
				USB_Tx_length = midi->APP_Rx_length;

				midi->APP_Rx_ptr_out += midi->APP_Rx_length;
				midi->APP_Rx_length = 0;
			}

			/* Prepare the available data buffer to be sent on IN endpoint */
			// DCD_EP_Tx (pdev,
			// 		MIDI_IN_EP,
			// 		(uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr],
			// 		USB_Tx_length);
      (void)USBD_LL_Transmit(pdev, MIDI_IN_EP, (uint8_t*)&(midi->APP_Rx_Buffer[USB_Tx_ptr]), USB_Tx_length);
		}
	}

	return (uint8_t) USBD_OK;
}

static uint8_t  USBD_MIDI_DataOut     (USBD_HandleTypeDef *pdev, uint8_t epnum){
//	Should take care of processing the data and deliver it to the application.
	uint16_t USB_Rx_Cnt;
  USBD_MIDI_HandleTypeDef*midi = (USBD_MIDI_HandleTypeDef *)pdev->pClassData;

  if (midi == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }
  /* Get the received data buffer and update the counter */
	//USB_Rx_Cnt = pdev->ep_out[epnum].xfer_count;
  /* Get received data packet length */
  USB_Rx_Cnt = (uint16_t)USBD_LL_GetRxDataSize(pdev, epnum);

	/* Forward the data to the user callback. */
  midi->Rx_callback((uint8_t*)&(midi->USB_Rx_Buffer), USB_Rx_Cnt);
	// DCD_EP_PrepareRx(pdev,
	// 	                   CDC_OUT_EP,
	// 	                   (uint8_t*)(USB_Rx_Buffer),
	// 	                   MIDI_DATA_OUT_PACKET_SIZE);
  (void)USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP ,(uint8_t*)(midi->USB_Rx_Buffer), MIDI_DATA_OUT_PACKET_SIZE);

	return (uint8_t) USBD_OK;
}

static uint8_t  USBD_MIDI_SOF         (USBD_HandleTypeDef *pdev){
  USBD_MIDI_HandleTypeDef* midi = (USBD_MIDI_HandleTypeDef*)pdev->pClassData;
	
  if (midi == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

	if ((midi->FrameCount)++ == MIDI_IN_FRAME_INTERVAL)
	{
		/* Reset the frame counter */
		midi->FrameCount = 0;

		/* Check the data to be sent through IN pipe */
		Handle_USBAsynchXfer(pdev);
	}

	return (uint8_t) USBD_OK;
}


static void Handle_USBAsynchXfer (USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef* midi = (USBD_MIDI_HandleTypeDef*)pdev->pClassData;
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;

	if(midi->USB_Tx_State != USB_TX_STATE_FULL) {
		if (midi->APP_Rx_ptr_out == APP_RX_DATA_SIZE) {
			midi->APP_Rx_ptr_out = 0;
		}
		if(midi->APP_Rx_ptr_out == midi->APP_Rx_ptr_in){
			midi->USB_Tx_State = USB_TX_STATE_EMPTY;
      //no data to send
			return; 
		}
    /* rollback */
		if(midi->APP_Rx_ptr_out > midi->APP_Rx_ptr_in) {
			midi->APP_Rx_length = APP_RX_DATA_SIZE - midi->APP_Rx_ptr_out;
    } else {
			midi->APP_Rx_length = midi->APP_Rx_ptr_in - midi->APP_Rx_ptr_out;
		}
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
		midi->APP_Rx_length &= ~0x03;
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

		if (midi->APP_Rx_length > MIDI_DATA_IN_PACKET_SIZE){
			USB_Tx_ptr = midi->APP_Rx_ptr_out;
			USB_Tx_length = MIDI_DATA_IN_PACKET_SIZE;

			midi->APP_Rx_ptr_out += MIDI_DATA_IN_PACKET_SIZE;
			midi->APP_Rx_length -= MIDI_DATA_IN_PACKET_SIZE;
		}	else {
			USB_Tx_ptr = midi->APP_Rx_ptr_out;
			USB_Tx_length = midi->APP_Rx_length;

			midi->APP_Rx_ptr_out += midi->APP_Rx_length;
			midi->APP_Rx_length = 0;
		}
		midi->USB_Tx_State = USB_TX_STATE_FULL;

		// DCD_EP_Tx (pdev,
		// 		MIDI_IN_EP,
		// 		(uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr],
		// 		USB_Tx_length);
    (void)USBD_LL_Transmit(pdev, MIDI_IN_EP, (uint8_t*)&(midi->APP_Rx_Buffer[USB_Tx_ptr]), USB_Tx_length);
	}

}

static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length)
{
  *length = (uint16_t)sizeof (USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

static uint8_t *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MIDI_CfgDesc);

  return USBD_MIDI_CfgDesc;
}

static uint8_t *USBD_MIDI_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MIDI_DeviceQualifierDesc);

  return USBD_MIDI_DeviceQualifierDesc;
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/***************************END OF FILE****/
