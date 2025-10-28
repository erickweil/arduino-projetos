/*
  PositionQueue.cpp

  Implementação da PositionQueue. As variáveis de armazenamento são definidas
  com RTC_DATA_ATTR aqui para persistirem através de deep sleep no ESP (como
  no código original). A API da classe opera sobre essas variáveis.
*/

#include "PositionQueue.h"

PositionQueueClass PositionQueue;

// Rastreio, fila de posições armazenadas
// Controle da fila de posições armazenadas em RTC memory (Mantém os dados após deep sleep)

// Buffer circular de posições armazenadas na fila
RTC_DATA_ATTR static Posicao posicoes[POSICOES_FILA_SIZE];

// Início da fila. Soma +1 antes de enviar cada posição
RTC_DATA_ATTR static size_t posicoesInicio = 0;
// Início da fila. Soma +1 antes de enviar cada posição (Para envio)
RTC_DATA_ATTR static size_t posicoesEnvio = 0;
// Índice da última posição armazenada na fila. Soma +1 após armazenar cada posição
RTC_DATA_ATTR static size_t posicoesFim = 0;

PositionQueueClass::PositionQueueClass()
{
    // Nothing to do; dados persistem em RTC
}

#ifdef UNIT_TEST
void PositionQueueClass::resetForUnitTest()
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

size_t PositionQueueClass::size() const
{
    return (POSICOES_FILA_SIZE - posicoesInicio + posicoesFim) % POSICOES_FILA_SIZE;
}

size_t PositionQueueClass::capacity() const
{
    return POSICOES_FILA_SIZE - 1U;
}

bool PositionQueueClass::isEmpty() const
{
    return posicoesFim == posicoesInicio;
}

size_t PositionQueueClass::getStart() const
{
    return posicoesInicio;
}

size_t PositionQueueClass::getSendIndex() const
{
    return posicoesEnvio;
}

size_t PositionQueueClass::getEnd() const
{
    return posicoesFim;
}

void PositionQueueClass::resetSend()
{
    posicoesEnvio = posicoesInicio;
}

bool PositionQueueClass::commitSend()
{
    // Nada para confirmar
    if(posicoesEnvio == posicoesInicio)
    {
        return false;
    }
    posicoesInicio = posicoesEnvio;
    return true;
}

void PositionQueueClass::enqueue(const Posicao &p)
{
    posicoes[posicoesFim] = p;
    posicoesFim = incrementIndex(posicoesFim);

    // Se avançou o início, perdeu a posição mais antiga
    if (PositionQueue.isEmpty())
    {
        posicoesInicio = incrementIndex(posicoesInicio);
    }
}

bool PositionQueueClass::dequeueForSend(Posicao &out)
{
    // Fila vazia
    if (posicoesEnvio == posicoesFim)
        return false;

    out = posicoes[posicoesEnvio];
    posicoesEnvio = incrementIndex(posicoesEnvio);
    return true;
}

bool PositionQueueClass::getAt(size_t index, Posicao &out) const
{
    // Índice inválido
    if (index >= capacity())
        return false;

    out = posicoes[(posicoesInicio + index) % POSICOES_FILA_SIZE];
    return out.timestamp != 0;
}

int PositionQueueClass::toJson(const Posicao &pos, uint32_t count, char * const str_buf, size_t str_buf_size)
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
