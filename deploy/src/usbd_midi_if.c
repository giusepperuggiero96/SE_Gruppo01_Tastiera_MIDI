/* Includes ------------------------------------------------------------------*/
//Authors: Andrea Marchetta, Marco Barletta, Giuseppe Ruggiero, Riccardo Corvi
#include "usbd_midi_if.h"

//Function prototypes
// static void set_note(uint8_t* midi_packet, uint8_t note, uint8_t velocity, uint8_t channel, uint8_t cmd);
static void set_note(uint8_t* midi_packet, USBD_MIDI_NoteTypedef *midi_note);
static void set_pitch(uint8_t* midi_packet, USBD_MIDI_PitchTypedef *midi_pitch);

//Function Bodies

static void set_note(uint8_t* midi_packet, USBD_MIDI_NoteTypedef *midi_note)
{
  if (midi_note->cmd == NOTE_ON || midi_note->cmd == NOTE_OFF ){
    *(midi_packet     ) = 0x08;
    *(midi_packet + 1 ) = (midi_note->cmd == NOTE_ON) ? \
                            (0x90 | (0x0f & midi_note->channel)) : \
                            (0x80 | (0x0f & midi_note->channel));
    *(midi_packet + 2 ) = midi_note->note;
    *(midi_packet + 3 ) = midi_note->velocity;
  }
}

static void set_pitch(uint8_t* midi_packet, USBD_MIDI_PitchTypedef *midi_pitch)
{
    *(midi_packet     ) =  0x0E;
    *(midi_packet + 1 ) = (0xE0 | (0x0f & midi_pitch->channel));
    *(midi_packet + 2 ) = (uint8_t) ((midi_pitch->pitchBend >> 8) & 0x00FF);
    *(midi_packet + 3 ) = (uint8_t) (midi_pitch->pitchBend & 0x00FF);
}

USBD_StatusTypeDef USBD_MIDI_SendPitch(USBD_HandleTypeDef *pdev, USBD_MIDI_PitchTypedef *midi_pitch)
{
  uint8_t message [4];
  set_pitch(message, midi_pitch);
  return USBD_MIDI_SendMsg(pdev, message, 4);
}

USBD_StatusTypeDef USBD_MIDI_SendCC(USBD_HandleTypeDef *pdev, uint8_t channel , uint8_t controler_number, uint8_t controller_value) 
{
    uint8_t b[4];
    b[0] = 0x0B;
    b[1] = 0xB0 | channel;
    b[2] = controler_number;
    b[3] = controller_value;

   return USBD_MIDI_SendMsg(pdev, b, 4);
}

USBD_StatusTypeDef USBD_MIDI_SendNote(USBD_HandleTypeDef *pdev, USBD_MIDI_NoteTypedef *midi_note)
{
  uint8_t message [4];
  set_note(message , midi_note);
  return USBD_MIDI_SendMsg(pdev,message, 4);
}
