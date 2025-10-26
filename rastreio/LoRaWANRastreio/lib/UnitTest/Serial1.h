#include "ArduinoFake.h"

class Serial1_: public Serial_ {

public:
    Serial1_();
	void begin(unsigned long, uint8_t, int, int);
};
extern Serial1_ Serial1;