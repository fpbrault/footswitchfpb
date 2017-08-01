/*
Hiduino ProductID: 0x2050
Hiduino Name: DaemonPedal

Simple MIDI footswitch 2.0

by Mikael Norrgard 2015
http://www.witchmastercreations.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

BUTTON CONNECTIONS
---------------------------
Connect one lead of each button to the input pin and one lead to
ground, simple. This code assume buttons on pins 2-8.
*/

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#define BUTTON1 2          // The pin for first button
#define NUMBUTTONS 7       // The number of buttons
#define MIDICHANNEL 15     // The midi channel to use
#define LATENCY 50         // The latency
#define DEBOUNCE_TIME 100
#define BUTTON_PRESSED 0

// Define arrays
int  note[NUMBUTTONS]                = {36,37,38,39,40,41,42};// The array for the midi notes to send
byte button_state[NUMBUTTONS]        = {1, 1, 1, 1, 1, 1, 1}; // Values are 0 when buttons are pressed
long button_time_changed[NUMBUTTONS] = {0, 0, 0, 0, 0, 0, 0}; // The time in milliseconds when the button changed status

int i; // For loops
byte iDigitalRead = 0; // Temporary variable for reading buttons
byte iBitValue = 0; // Temporary variable for reading bits from bytes

// Timer
unsigned long currentTime; // the current "time" variable


void setup()
{
  for(int i=BUTTON1;i<BUTTON1+NUMBUTTONS;i++) {
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }

  //initialize MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI); //initialize MIDI at all channels
  MIDI.setThruFilterMode(midi::Thru::Full);
  currentTime = millis(); // reset the timer
}

void loop() {

  currentTime = millis(); // get the current "time"

  // Read input midi
  MIDI.read();

  // Check the first 6 buttons
  iDigitalRead = PIND; // Pins 0-7
  for(i=2;i<=7;i++) { // Bits 2-7 are pins 2-7, the button_state array is offset
    iBitValue = bitRead(iDigitalRead, i);
    if(currentTime > (button_time_changed[i-2] + DEBOUNCE_TIME) && iBitValue != button_state[i-2])
    {
      button_state[i-2] = iBitValue;
      button_time_changed[i-2] = currentTime;

      if(iBitValue == BUTTON_PRESSED) { // The button has been pressed
        MIDI.sendNoteOn(note[i-2], 127, MIDICHANNEL); // Send note on
      }
      else {
        MIDI.sendNoteOff(note[i-2], 0, MIDICHANNEL); // Send note off
      }
    }
  }

  // Check the last button
  iDigitalRead = PINB; // Pins 8-13
  for(i=0;i<=0;i++) {// Bit 0 is pin 8
    iBitValue = bitRead(iDigitalRead, i);
    if(currentTime > (button_time_changed[i+6] + DEBOUNCE_TIME) && iBitValue != button_state[i+6])
    {
      button_state[i+6] = iBitValue;
      button_time_changed[i+6] = currentTime;

      if(iBitValue == BUTTON_PRESSED) { // The button has been pressed
        MIDI.sendNoteOn(note[i+6], 127, MIDICHANNEL); // Send note on
      }
      else {
        MIDI.sendNoteOff(note[i+6], 0, MIDICHANNEL); // Send note off
      }
    }
  }
}
