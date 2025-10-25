/*
    Fila circular de posições GPS armazenadas em RTC memory

    A FAZER: Memória flash?
*/

#ifndef POSITION_QUEUE_H
#define POSITION_QUEUE_H

#include <Arduino.h>
#include "Position.h"

#define POSICOES_FILA_SIZE 100U

class PositionQueueClass
{
public:
    PositionQueueClass();

    /**
     * @return Número de posições atualmente armazenadas na fila.
     */
    size_t size() const;
    size_t capacity() const;
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
    bool commitSend();

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
    [[nodiscard]] bool dequeueForSend(Posicao &out);
private:
    static inline size_t incrementIndex(size_t idx)
    {
        return (idx + 1) % POSICOES_FILA_SIZE;
    }
};

extern PositionQueueClass PositionQueue;
#endif
