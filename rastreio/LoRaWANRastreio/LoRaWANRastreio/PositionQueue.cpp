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

