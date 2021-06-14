//Authors: Andrea Marchetta, Marco Barletta, Giuseppe Ruggiero, Riccardo Corvi
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_MIDI_IF_H
#define __USB_MIDI_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#include  "usbd_ioreq.h"
#include  "usbd_midi.h"

//Exported Macros
#define NOTE_ON           0
#define NOTE_OFF          1

#define MIDI_NOTE_C(x)    (0x00 + 12*x)
#define MIDI_NOTE_Cs(x)   (0x01 + 12*x)
#define MIDI_NOTE_D(x)    (0x02 + 12*x)
#define MIDI_NOTE_Eb(x)   (0x03 + 12*x)
#define MIDI_NOTE_E(x)    (0x04 + 12*x)
#define MIDI_NOTE_F(x)    (0x05 + 12*x)
#define MIDI_NOTE_Fs(x)   (0x06 + 12*x)
#define MIDI_NOTE_G(x)    (0x07 + 12*x)
#define MIDI_NOTE_Gs(x)   (0x08 + 12*x)
#define MIDI_NOTE_A(x)    (0x09 + 12*x)
#define MIDI_NOTE_Bb(x)   (0x0a + 12*x)
#define MIDI_NOTE_B(x)    (0x0b + 12*x)

//Exported typedefs
typedef struct{
    uint8_t note;
    uint8_t velocity;
    uint8_t channel;
    uint8_t cmd;
} USBD_MIDI_NoteTypedef;

typedef struct{
    uint8_t pitchBend;
    uint8_t channel;
} USBD_MIDI_PitchTypedef;

//Exported Functions
USBD_StatusTypeDef USBD_MIDI_SendNote(USBD_HandleTypeDef *pdev, USBD_MIDI_NoteTypedef *midi_note);
USBD_StatusTypeDef USBD_MIDI_SendPitch(USBD_HandleTypeDef *pdev, USBD_MIDI_PitchTypedef *midi_pitch);
USBD_StatusTypeDef USBD_MIDI_SendCC(USBD_HandleTypeDef *pdev, uint8_t channel , uint8_t controler_number, uint8_t controller_value);


#ifdef __cplusplus
}
#endif


#endif  /* __USB_MIDI_IF_H */

