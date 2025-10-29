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
