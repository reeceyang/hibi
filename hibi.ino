//import("stdfaust.lib");
//
//env = en.ar(0.001, 0.05);
//kick(g) = g : env : *(os.osc(100));
//snare(g) = g : env : *(no.noise : fi.high_shelf(-60, 3000));
//hihat(g) = g : env : *(no.noise : fi.highpass(3, 10000));
//
//freq = nentry("freq",440,20,20000,0.01) : si.smoo;
//gate1 = nentry("gate1",0,0,1,1);
//gate2 = nentry("gate2",0,0,1,1);
//gate3 = nentry("gate3",0,0,1,1);
//gate4 = nentry("gate4",0,0,1,1);
//process = os.square(freq) * gate1 + kick(gate2) + snare(gate3) + hihat(gate4);

#include <Bounce.h>  // Bounce library makes button change detection easy
#include "Hibi.h"

Hibi hibi;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(hibi,0,out,0);
AudioConnection patchCord1(hibi,0,out,1);

const int channel = 1;
const int debounceTime = 10;
const int numButtons = 16;

const int pins[numButtons] = {
  0, 1, 2, 3, 4, 5, 9, 24, 25, 26, 27, 28, 29, 30, 31, 32
};

Bounce buttons[numButtons] = {
    Bounce(pins[0], debounceTime),  Bounce(pins[1], debounceTime),
    Bounce(pins[2], debounceTime),  Bounce(pins[3], debounceTime),
    Bounce(pins[4], debounceTime),  Bounce(pins[5], debounceTime),
    Bounce(pins[6], debounceTime),  Bounce(pins[7], debounceTime),
    Bounce(pins[8], debounceTime),  Bounce(pins[9], debounceTime),
    Bounce(pins[10], debounceTime), Bounce(pins[11], debounceTime),
    Bounce(pins[12], debounceTime), Bounce(pins[13], debounceTime),
    Bounce(pins[14], debounceTime), Bounce(pins[15], debounceTime)
};

const int frequencies[13] = {
    262, // C4
    277, // C#4/Db4
    294, // D4
    311, // D#4/Eb4
    330, // E4
    349, // F4
    370, // F#4/Gb4
    392, // G4
    415, // G#4/Ab4
    440, // A4
    466, // A#4/Bb4
    494, // B4
    523  // C5 (Octave above)
};

void setup() {
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(1);
  for (int i = 0; i < numButtons; i++) {
    auto pin = pins[i];
    pinMode(pin, INPUT_PULLUP);
    buttons[i] = Bounce(pin, debounceTime);
  }
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
  }
  bool keyPressed = false;
  for (int i = 0; i < numButtons; i++) {
    auto button = buttons[i];
    if (button.fallingEdge()) {
      usbMIDI.sendNoteOn(24 + i, 99, channel); // 24 = C1
    }
    if (button.risingEdge()) {
      usbMIDI.sendNoteOff(24 + i, 0, channel); // 24 = C1
    }
    auto pin = pins[i];    
    switch (i) {
      case 0:
        if (!digitalRead(pin)) {
          hibi.setParamValue("gate2",1);
        } else {
          hibi.setParamValue("gate2",0);          
        }
        break;
      case 1:
        if (!digitalRead(pin)) {
          hibi.setParamValue("gate3",1);
        } else {
          hibi.setParamValue("gate3",0);          
        }
        break;
      case 2:
        if (!digitalRead(pin)) {
          hibi.setParamValue("gate4",1);
        } else {
          hibi.setParamValue("gate4",0);          
        }
        break;
      default:
        if (!digitalRead(pin)) {
          hibi.setParamValue("gate1",1);
          hibi.setParamValue("freq", frequencies[i - 3]);
          keyPressed = true;
        }
        break;      
    }
  }
  if (!keyPressed) {
    hibi.setParamValue("gate1", 0);  
  }

  // MIDI Controllers should discard incoming MIDI messages.
  while (usbMIDI.read()) {
  }
}
