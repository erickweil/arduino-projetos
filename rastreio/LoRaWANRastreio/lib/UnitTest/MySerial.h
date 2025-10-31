#ifndef SERIAL_FAKE_H
#define SERIAL_FAKE_H

#include <vector>
#include <string>

class MySerial {
public:
    MySerial();
    void begin(int baud, int config, int rx, int tx) {}
    int available();
    int read();
    void print(const char *) {}
    void println(const char *) {}
    // Methods for unit testing
    void __feed(const char *s);
    void __delay();
    void __clear();
private:
    constexpr static char DELAY_SEP = (char)127; // DEL as separator for delays
    std::string buf;
    size_t idx;
    u_int32_t delay;
};

MySerial Serial1;

MySerial::MySerial() : buf(), idx(0), delay(0) {
    
}

int MySerial::available() {
    if(delay > 0) {
        delay--;
        return 0;
    }

    if (idx >= buf.size()) return 0;

    // Encontrar o próximo caractere divisor
    size_t end = buf.find(DELAY_SEP, idx);

    if(end != std::string::npos) {
        // Há uma nova linha disponível
        return (int)(end - idx + 1); // incluir o '\n'
    } else {
        // Não há nova linha, retornar o restante do buffer
        return (int)(buf.size() - idx);
    }
}

int MySerial::read() {
    if(delay > 0) {
        delay--;
        return -1;
    }

    if (idx >= buf.size()) return -1;
    unsigned char c = buf[idx++];

    if(idx < buf.size() && buf[idx] == DELAY_SEP) {
        // Próximo caractere é um separador de atraso
        delay = 50;
        idx++; // pular o separador
    }

    return (int)c;
}

void MySerial::__feed(const char *s) {
    if (!s) return;
    buf.append(s);
}

void MySerial::__delay() {
    buf.push_back(DELAY_SEP);
}

void MySerial::__clear() {
    buf.clear();
    idx = 0;
    delay = 0;
}


#endif
