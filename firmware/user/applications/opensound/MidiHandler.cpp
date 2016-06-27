#include <string.h>
#include "MidiStatus.h"
#include "MidiHandler.h"
#include "OpenWareMidiControl.h"

MidiHandler::MidiHandler(){
  memset(midi_values, 0, NOF_PARAMETERS*sizeof(uint16_t));
}

void MidiHandler::handlePitchBend(uint8_t status, uint16_t value){
  // setParameter(PARAMETER_G, ((int16_t)value - 8192)>>1);
}

void MidiHandler::handleNoteOn(uint8_t status, uint8_t note, uint8_t velocity){
  // setButton(MIDI_NOTE_BUTTON+note, velocity<<5);
}

void MidiHandler::handleNoteOff(uint8_t status, uint8_t note, uint8_t velocity){
  // setButton(MIDI_NOTE_BUTTON+note, 0);
}

void MidiHandler::handleProgramChange(uint8_t status, uint8_t pid){
}

void MidiHandler::handleControlChange(uint8_t status, uint8_t cc, uint8_t value){
  switch(cc){
  case PATCH_PARAMETER_A:
    midi_values[PARAMETER_A] = value<<5; // scale from 7bit to 12bit value
    break;
  case PATCH_PARAMETER_B:
    midi_values[PARAMETER_B] = value<<5;
    break;
  case PATCH_PARAMETER_C:
    midi_values[PARAMETER_C] = value<<5;
    break;
  case PATCH_PARAMETER_D:
    midi_values[PARAMETER_D] = value<<5;
    break;
  case PATCH_PARAMETER_E:
    midi_values[PARAMETER_E] = value<<5;
    break;
  case REQUEST_SETTINGS:
      break;
    switch(value){
    case PATCH_PARAMETER_A:
    case PATCH_PARAMETER_B:
    case PATCH_PARAMETER_C:
    case PATCH_PARAMETER_D:
    case PATCH_PARAMETER_E:
      // midi.sendPatchParameterValues();
      break;
    case 127:
      // midi.sendSettings();
      break;
    }
    break;
  case SAVE_SETTINGS:
    // if(value == 127){
    //   toggleLed();
    //   settings.saveToFlash();
    //   toggleLed();
    // }
    break;
  case FACTORY_RESET:
    // if(value == 127){
    //   settings.reset();
    //   program.eraseProgramFromFlash(-1);
    //   updateCodecSettings();
    // }
    break;
  case MIDI_CC_MODULATION:
    // setParameter(PARAMETER_F, value<<5);
    break;
  case MIDI_CC_EFFECT_CTRL_1:
    // setParameter(PARAMETER_G, value<<5);
    break;
  case MIDI_CC_EFFECT_CTRL_2:
    // setParameter(PARAMETER_H, value<<5);
    break;
  default:
    // if(cc >= PATCH_PARAMETER_AA && cc <= PATCH_PARAMETER_BH)
    //   setParameter(PARAMETER_AA+(cc-PATCH_PARAMETER_AA), value<<5);
    break;
  }
}

void MidiHandler::handleConfigurationCommand(uint8_t* data, uint16_t size){
  // if(size < 4)
  //   return;
  // char* p = (char*)data;
  // int32_t value = strtol(p+2, NULL, 16);
  // if(strncmp(SYSEX_CONFIGURATION_AUDIO_RATE, p, 2) == 0){
  //   settings.audio_samplingrate = value;
  // }else if(strncmp(SYSEX_CONFIGURATION_AUDIO_BLOCKSIZE, p, 2) == 0){
  //   settings.audio_blocksize = value;
  // }
  // updateCodecSettings();
}

void MidiHandler::handleSysEx(uint8_t* data, uint16_t size){
  if(size < 3 || 
     data[0] != MIDI_SYSEX_MANUFACTURER || 
     data[1] != MIDI_SYSEX_DEVICE)
    return;
  // switch(data[2]){
  // case SYSEX_CONFIGURATION_COMMAND:
  //   handleConfigurationCommand(data+3, size-3);
  //   break;
  // case SYSEX_DFU_COMMAND:
  //   jump_to_bootloader();
  //   break;
  // case SYSEX_FIRMWARE_UPLOAD:
  //   handleFirmwareUploadCommand(data, size);
  //   break;
  // case SYSEX_FIRMWARE_RUN:
  //   handleFirmwareRunCommand(data+3, size-3);
  //   break;
  // case SYSEX_FIRMWARE_STORE:
  //   handleFirmwareStoreCommand(data+3, size-3);
  //   break;
  // case SYSEX_FIRMWARE_FLASH:
  //   handleFirmwareFlashCommand(data+3, size-3);
  //   break;
  // }
}

