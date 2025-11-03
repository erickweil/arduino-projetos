#ifndef POSITION_H
#define POSITION_H

#include <Arduino.h>

// Dados de uma posição GPS (16 bytes)
struct Posicao
{
    // 0
    uint32_t timestamp; // Unix timestamp, seconds since 1970-01-01 UTC

    // 4
    int32_t lat; // Latitude x 1e7 (4 bytes)

    // 8
    int32_t lng; // Longitude x 1e7 (4 bytes)

    // 12
    uint8_t speed;  // 0-255 km/h
    uint8_t course; // 0-360° -> 0-255°
    uint8_t hdop;   // 0.1-100+ -> 0-255  Horizontal Dilution of Precision https://en.wikipedia.org/wiki/Dilution_of_precision
    uint8_t sats;   // Number of satellites used in fix
}; //__attribute__((packed));
static_assert(sizeof(Posicao) == 16, "Tamanho da struct Posicao deve ser 16 bytes");

class PositionQueueClass
{
public:
    /**
     * @return Número de posições atualmente armazenadas na fila.
     */
    virtual size_t size() const = 0;

    virtual size_t pendingSend() const = 0;

    virtual size_t capacity() const = 0;

    virtual bool isEmpty() const = 0;

    // Acessores para índices (úteis para integrar com código que escreve índice no payload)
    virtual size_t getStart() const = 0;

    virtual size_t getSendIndex() const = 0;

    virtual size_t getEnd() const = 0;
    
    /**
     * Irá marcar que todas as posições até SendIndex foram enviadas/consumidas.
     */
    virtual void commitSend(size_t index) = 0;

    /**
     * Enfileira uma posição. Se a fila estiver cheia, sobrescreve a mais antiga.
     * @param p Referência à posição que será enfileirada.
     */
    virtual bool enqueue(const Posicao &p) = 0;

    /**
     * Inicia a leitura sequencial de posições a partir do índice especificado.
     * @param index Índice da primeira posição a ser lida.
     * @return true se a leitura foi iniciada com sucesso, false caso contrário.
     */
    virtual bool beginReadAt(size_t index) = 0;
    /**
     * Lê a próxima posição na sequência iniciada por beginReadAt.
     * @param out Referência para onde a posição lida será copiada (só se retornar true).
     * @return true se uma posição foi lida com sucesso, false se não há mais posições ou ocorreu um erro.
     */
    virtual bool readNext(Posicao &out) = 0;
    /**
     * Finaliza a leitura sequencial iniciada por beginReadAt (retorna o índice final lido).
     */
    virtual size_t endRead() = 0;

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
};

#endif // POSITION_H