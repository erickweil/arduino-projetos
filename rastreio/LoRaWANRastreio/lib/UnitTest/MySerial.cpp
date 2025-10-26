#include "MySerial.h"

MySerial Serial1;

MySerial::MySerial() : buf(), idx(0), delay(0) {
}

int MySerial::available() {
    if(delay > 0) {
        delay--;
        return 0;
    }

    if (idx >= buf.size()) return 0;
    return (int)(buf.size() - idx);
}

int MySerial::read() {
    if(delay > 0) {
        delay--;
        return -1;
    }

    if (idx >= buf.size()) return -1;
    unsigned char c = buf[idx++];

    if(c == '\n') {
        // Simula delay de chegada de nova linha
        delay += 50;
    }

    return (int)c;
}

void MySerial::feed(const char *s) {
    if (!s) return;
    buf.append(s);
}

void MySerial::clear() {
    buf.clear();
    idx = 0;
    delay = 0;
}
