/*
    Fila circular de posições GPS armazenadas em RTC memory

    A FAZER: Memória flash?
*/

#ifndef POSITION_QUEUE_H
#define POSITION_QUEUE_H

#include <Arduino.h>
#include <time.h>
#include "config.h"
#include "Position.h"

// Rastreio, fila de posições armazenadas
// Controle da fila de posições armazenadas em RTC memory (Mantém os dados após deep sleep)

// Buffer circular de posições armazenadas na fila
RTC_DATA_ATTR static Posicao posicoes[POSICOES_FILA_SIZE];

// Início da fila. Soma +1 antes de enviar cada posição
RTC_DATA_ATTR static size_t posicoesInicio = 0;
// Início da fila. Soma +1 antes de enviar cada posição (Para envio)
RTC_DATA_ATTR static size_t posicoesEnvioConfirmado = 0;
// Índice da última posição armazenada na fila. Soma +1 após armazenar cada posição
RTC_DATA_ATTR static size_t posicoesFim = 0;

class PositionQueueRTC: public PositionQueueClass
{
public:
    PositionQueueRTC(): _readIndex(SIZE_MAX)
    {
        // Nothing to do; dados persistem em RTC
#ifdef UNIT_TEST
        resetForUnitTest();
#endif        
    }

#ifdef UNIT_TEST
    /**
     * Helper para testes unitários: reseta índices e buffer.
     * Só compilado quando UNIT_TEST está definido (ex: ambiente native).
     */
    void resetForUnitTest()
    {
        // Reset indices and clear buffer for unit tests
        posicoesInicio = 0;
        posicoesEnvioConfirmado = 0;
        posicoesFim = 0;
        for (size_t i = 0; i < POSICOES_FILA_SIZE; ++i)
        {
            posicoes[i] = Posicao{0, 0, 0, 0, 0, 0, 0};
        }
    }
#endif

    /**
     * @return Número de posições atualmente armazenadas na fila.
     */
    size_t size() const override
    {
        return (POSICOES_FILA_SIZE - posicoesInicio + posicoesFim) % POSICOES_FILA_SIZE;
    }

    size_t pendingSend() const override
    {
        return (POSICOES_FILA_SIZE - posicoesEnvioConfirmado + posicoesFim) % POSICOES_FILA_SIZE;
    }

    size_t capacity() const override
    {
        return POSICOES_FILA_SIZE - 1U;
    }

    bool isEmpty() const override
    {
        return posicoesFim == posicoesInicio;
    }

    // Acessores para índices (úteis para integrar com código que escreve índice no payload)
    size_t getStart() const override
    {
        return posicoesInicio;
    }

    size_t getSendIndex() const override
    {
        return posicoesEnvioConfirmado;
    }

    size_t getEnd() const override
    {
        return posicoesFim;
    }

    bool beginReadAt(size_t index) override
    {
        if(_readIndex != SIZE_MAX)
        {
            Serial.println("[PosQueue] Erro: leitura já iniciada.");
            return false;
        }

        _readIndex = index;
        return true;
    }

    bool readNext(Posicao &out) override
    {
        if(_readIndex == SIZE_MAX)
        {
            Serial.println("[PosQueue] Erro: leitura não iniciada.");
            return false;
        }
        
        // Fila vazia
        if (_readIndex == posicoesFim)
        {
            return false;
        }

        out = posicoes[_readIndex];
        _readIndex = incrementIndex(_readIndex);
        return true;
    }

    size_t endRead() override
    {
        size_t finalIndex = _readIndex;
        _readIndex = SIZE_MAX;
        return finalIndex;
    }
    
    // /**
    //  * Chamar antes de dequeueForSend para iniciar o processo de envio.
    //  * Irá fixar o índice de envio na posição atual de início da fila.
    //  */
    // void resetSend() override
    // {
    //     posicoesEnvio = posicoesInicio;
    // }


    /**
     * Irá marcar que todas as posições até SendIndex foram enviadas/consumidas.
     */
    void commitSend(size_t index) override
    {
        posicoesEnvioConfirmado = index;
    }

    /**
     * Enfileira uma posição. Se a fila estiver cheia, sobrescreve a mais antiga.
     * @param p Referência à posição que será enfileirada.
     */
    bool enqueue(const Posicao &p) override
    {
        posicoes[posicoesFim] = p;
        posicoesFim = incrementIndex(posicoesFim);

        // Se avançou o início, perdeu a posição mais antiga
        if (isEmpty())
        {
            posicoesInicio = incrementIndex(posicoesInicio);
        }

        return true;
    }

    // bool getAt(size_t index, Posicao &out) const override
    // {
    //     // Índice inválido
    //     if (index >= capacity())
    //         return false;

    //     out = posicoes[(posicoesInicio + index) % POSICOES_FILA_SIZE];
    //     return true;
    // }

    // /**
    //  * Desenfileira a próxima posição para envio (Alterando apenas o índice de envio).
    //  * @param out Referência para onde a posição desenfileirada será copiada (só se retornar true).
    //  * @return true se havia uma posição para desenfileirar, false se a fila estava vazia.
    //  */
    // [[nodiscard]] bool dequeueForSend(Posicao &out) override
    // {
    //     // Fila vazia
    //     if (posicoesEnvio == posicoesFim)
    //         return false;

    //     out = posicoes[posicoesEnvio];
    //     posicoesEnvio = incrementIndex(posicoesEnvio);
    //     return true;
    // }
private:
    size_t _readIndex; // Índice atual de leitura sequencial

    static inline size_t incrementIndex(size_t idx)
    {
        return (idx + 1) % POSICOES_FILA_SIZE;
    }
};

#endif
