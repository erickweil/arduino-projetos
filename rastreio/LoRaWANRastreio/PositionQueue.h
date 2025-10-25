/*
    Fila circular de posições GPS armazenadas em RTC memory

    A FAZER: Memória flash?
*/
#ifndef POSICOES_FILA_SIZE
#define POSICOES_FILA_SIZE 100
#endif

#ifndef POSITION_QUEUE_H
#define POSITION_QUEUE_H

#include <Arduino.h>

// Dados de uma posição GPS (16 bytes)
struct Posicao
{
    // 0
    uint32_t timestamp; // Unix timestamp, seconds since 1970-01-01 UTC

    // 4
    int32_t lat;

    // 8
    int32_t lng;

    // 12
    uint8_t speed;  // 0-255 km/h
    uint8_t course; // 0-360° -> 0-255°
    uint8_t hdop;   // 0.1-100+ -> 0-255  Horizontal Dilution of Precision https://en.wikipedia.org/wiki/Dilution_of_precision
    uint8_t sats;   // Number of satellites used in fix
};

class PositionQueueClass
{
public:
    PositionQueueClass();

    /**
     * @return Número de posições atualmente armazenadas na fila.
     */
    size_t size() const;
    bool isEmpty() const;

    // Acessores para índices (úteis para integrar com código que escreve índice no payload)
    size_t getStart() const;
    size_t getSendIndex() const;
    size_t getEnd() const;
    
    /**
     * Chamar antes de dequeueForSend para iniciar o processo de envio.
     * Irá fixar o índice de envio na posição atual de início da fila.
     */
    void resetSend();

    /**
     * Irá marcar que todas as posições até SendIndex foram enviadas/consumidas.
     */
    void commitSend();

    /**
     * Enfileira uma posição. Se a fila estiver cheia, sobrescreve a mais antiga.
     * @param p Referência à posição que será enfileirada.
     */
    void enqueue(const Posicao &p);

    /**
     * Desenfileira a próxima posição para envio (Alterando apenas o índice de envio).
     * @param out Referência para onde a posição desenfileirada será copiada (só se retornar true).
     * @return true se havia uma posição para desenfileirar, false se a fila estava vazia.
     */
    bool dequeueForSend(Posicao &out);
private:
    static inline size_t incrementIndex(size_t idx, size_t capacity)
    {
        return (idx + 1) % capacity;
    }
};

extern PositionQueueClass PositionQueue;
#endif
