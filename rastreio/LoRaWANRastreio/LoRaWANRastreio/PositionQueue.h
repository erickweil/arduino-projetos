/*
    Fila circular de posições GPS armazenadas em RTC memory

    A FAZER: Memória flash?
*/

#ifndef POSITION_QUEUE_H
#define POSITION_QUEUE_H

#include <Arduino.h>
#include <time.h>
#include "config.h"

// Rastreio, fila de posições armazenadas
// Controle da fila de posições armazenadas em RTC memory (Mantém os dados após deep sleep)

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

// Buffer circular de posições armazenadas na fila
RTC_DATA_ATTR static Posicao posicoes[POSICOES_FILA_SIZE];

// Início da fila. Soma +1 antes de enviar cada posição
RTC_DATA_ATTR static size_t posicoesInicio = 0;
// Início da fila. Soma +1 antes de enviar cada posição (Para envio)
RTC_DATA_ATTR static size_t posicoesEnvio = 0;
// Índice da última posição armazenada na fila. Soma +1 após armazenar cada posição
RTC_DATA_ATTR static size_t posicoesFim = 0;

class PositionQueueClass
{
public:
    PositionQueueClass()
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
        posicoesEnvio = 0;
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
    size_t size() const
    {
        return (POSICOES_FILA_SIZE - posicoesInicio + posicoesFim) % POSICOES_FILA_SIZE;
    }

    size_t capacity() const
    {
        return POSICOES_FILA_SIZE - 1U;
    }

    bool isEmpty() const
    {
        return posicoesFim == posicoesInicio;
    }

    // Acessores para índices (úteis para integrar com código que escreve índice no payload)
    size_t getStart() const
    {
        return posicoesInicio;
    }

    size_t getSendIndex() const
    {
        return posicoesEnvio;
    }

    size_t getEnd() const
    {
        return posicoesFim;
    }
    
    /**
     * Chamar antes de dequeueForSend para iniciar o processo de envio.
     * Irá fixar o índice de envio na posição atual de início da fila.
     */
    void resetSend()
    {
        posicoesEnvio = posicoesInicio;
    }


    /**
     * Irá marcar que todas as posições até SendIndex foram enviadas/consumidas.
     */
    bool commitSend()
    {
        // Nada para confirmar
        if(posicoesEnvio == posicoesInicio)
        {
            return false;
        }
        posicoesInicio = posicoesEnvio;
        return true;
    }

    /**
     * Enfileira uma posição. Se a fila estiver cheia, sobrescreve a mais antiga.
     * @param p Referência à posição que será enfileirada.
     */
    void enqueue(const Posicao &p)
    {
        posicoes[posicoesFim] = p;
        posicoesFim = incrementIndex(posicoesFim);

        // Se avançou o início, perdeu a posição mais antiga
        if (isEmpty())
        {
            posicoesInicio = incrementIndex(posicoesInicio);
        }
    }

    bool getAt(size_t index, Posicao &out) const
    {
        // Índice inválido
        if (index >= capacity())
            return false;

        out = posicoes[(posicoesInicio + index) % POSICOES_FILA_SIZE];
        return true;
    }

    /**
     * Desenfileira a próxima posição para envio (Alterando apenas o índice de envio).
     * @param out Referência para onde a posição desenfileirada será copiada (só se retornar true).
     * @return true se havia uma posição para desenfileirar, false se a fila estava vazia.
     */
    [[nodiscard]] bool dequeueForSend(Posicao &out)
    {
        // Fila vazia
        if (posicoesEnvio == posicoesFim)
            return false;

        out = posicoes[posicoesEnvio];
        posicoesEnvio = incrementIndex(posicoesEnvio);
        return true;
    }

    static int toJson(const Posicao &pos, uint32_t count, char * const str_buf, size_t str_buf_size)
    {
        time_t ts = pos.timestamp;
        tm* t = gmtime(&ts);

        return snprintf(str_buf, str_buf_size, 
            "{\"n\":%d,\"data\":\"%04d-%02d-%02dT%02d:%02d:%02dZ\",\"coords\":[%.7f,%.7f],\"vel\":%u,\"dir\":%.2f,\"hdop\":%.2f,\"satellites\":%u}",
            count,
            // Iso DateTime format: YYYY-MM-DDThh:mm:ssZ
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,
            (double)pos.lat/ 10000000.0, 
            (double)pos.lng/ 10000000.0, 
            pos.speed, 
            ((double)pos.course * 360.0) / 255.0, 
            (double)pos.hdop / 10.0, 
            pos.sats
        );
    }
private:
    static inline size_t incrementIndex(size_t idx)
    {
        return (idx + 1) % POSICOES_FILA_SIZE;
    }
};

#endif
