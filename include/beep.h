#ifndef _H_BEEP_
#define _H_BEEP_

#define INIT_BEEP() (pinMode(BEEP_PIN, OUTPUT),analogWriteFrequency(4000))
#define BEEP_ON() analogWrite(BEEP_PIN, 128)
#define BEEP_OFF() analogWrite(BEEP_PIN, 0)

#endif