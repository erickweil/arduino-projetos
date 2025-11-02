#ifndef POSITION_QUEUE_LITTLEFS_H
#define POSITION_QUEUE_LITTLEFS_H

#include <Arduino.h>
#include <time.h>
#include <FS.h>
#include <LittleFS.h>

#include "config.h"
#include "Position.h"

// Índice do início global da fila. É um índice com a primeira posição armazenada
RTC_DATA_ATTR static size_t filaInicio = 0;
// Índice do fim global da fila. É o índice onde a próxima posição será escrita
RTC_DATA_ATTR static size_t filaFim = 0;

// Índice de posições confirmadas como enviadas.
RTC_DATA_ATTR static size_t filaEnvioConfirmado = 0;
// Índice de leitura para envio.
RTC_DATA_ATTR static size_t filaEnvio = 0;

/**
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
 * https://randomnerdtutorials.com/esp32-write-data-littlefs-arduino/
 */
class PositionQueueLittleFS : public PositionQueueClass
{
public:
    PositionQueueLittleFS() : initialized(false)
    {
        static_assert(LITTLE_FS_QUEUE_MAX_FILE_SIZE % RECORD_SIZE == 0, "MAX_FILE_SIZE deve ser múltiplo de RECORD_SIZE");
        static_assert(LITTLE_FS_QUEUE_MAX_FILE_SIZE % CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE == 0, "MAX_FILE_SIZE deve ser múltiplo do tamanho do bloco de escrita do SPI Flash");
        static_assert(LITTLE_FS_QUEUE_FILES >= 2 && LITTLE_FS_QUEUE_FILES < 1024, "Deve haver pelo menos 2 e no máximo 1023 arquivos para rotação");
        static_assert(POSITIONS_PER_FILE > 3, "MAX_FILE_SIZE deve ser pelo menos o tamanho de 3 Posições");

#ifdef UNIT_TEST
        filaInicio = 0;
        filaFim = 0;
        filaEnvioConfirmado = 0;
        filaEnvio = 0;
#endif
    }

#ifdef UNIT_TEST
    void resetForUnitTest()
    {
        LittleFS.end();
        LittleFS.begin(true);

        // Remove todos os arquivos
        for (int i = 0; i < LITTLE_FS_QUEUE_FILES; ++i)
        {
            char path[32];
            getFilePath(i, path, sizeof(path));
            LittleFS.remove(path);
        }

        begin();
    }
#endif

    bool begin()
    {
        if (initialized)
            return true;

        // 1. validar que o tamanho total disponível é suficiente
        // https://github.com/littlefs-project/littlefs/issues/1067
        size_t totalCapacity = LITTLE_FS_QUEUE_FILES * LITTLE_FS_QUEUE_MAX_FILE_SIZE;
        size_t totalBytes = LittleFS.totalBytes();
        size_t usedBytes = LittleFS.usedBytes();
        Serial.print("[PosQueue] Espaço disponível:");
        Serial.print(" Total="); Serial.print(totalBytes);
        Serial.print(" Usado="); Serial.print(usedBytes);
        Serial.println();
        if (totalBytes < totalCapacity)
        {
            Serial.println("[PosQueue] Erro: LittleFS não tem espaço suficiente alocado.");
            return false;
        }
      
        // Calcular tamanho de cada arquivo de posição e atualizar índices
        FileLocation end_slot = {SIZE_MAX, SIZE_MAX}; // fim
        size_t next_slot = 0; // inicio
        for (size_t i = 0; i < LITTLE_FS_QUEUE_FILES; ++i)
        {
            char path[32];
            getFilePath(i, path, sizeof(path));
            File f = LittleFS.open(path, FILE_READ);
            Serial.print("  "); Serial.print(path);
            if (f)
            {
                size_t fileSize = f.size();
                Serial.print(" ("); Serial.print(fileSize); Serial.println(" bytes)");

                if(end_slot.fileSlot == SIZE_MAX)
                {
                    // Procurando o primeiro arquivo que não está cheio
                    if (fileSize < LITTLE_FS_QUEUE_MAX_FILE_SIZE)
                    {
                        end_slot.fileSlot = i;
                        end_slot.localIndex = fileSize / RECORD_SIZE;
                    }
                }
                else if(next_slot == 0)
                {
                    // Após encontrar o primeiro arquivo não cheio, o próximo que existir é o início
                    next_slot = i;
                }

                f.close();
            }
            else
            {
                Serial.println(" (não existe)");

                // O anterior está cheio e este não existe, então o anterior é o último com dados e este é o fim
                if(end_slot.fileSlot == SIZE_MAX)
                {
                    end_slot.fileSlot = i;
                    end_slot.localIndex = 0;
                }
            }
        }

        if(end_slot.fileSlot == SIZE_MAX)
        {
            Serial.println("[PosQueue] Estado inconsistente: todos os arquivos de posição estão cheios.");
            end_slot.fileSlot = 0;
            end_slot.localIndex = 0;
            next_slot = 1;
        }

        filaFim = end_slot.fileSlot * POSITIONS_PER_FILE + end_slot.localIndex;
        filaInicio = next_slot * POSITIONS_PER_FILE + 0;
        filaEnvioConfirmado = filaInicio;
        filaEnvio = filaInicio;
        // Índice de envio está escrito na última posição armazenada na fila
        if(filaFim != filaInicio)
        {
            Record lastRecord;
            if(readRecordAt((filaFim + POSITIONS_SIZE - 1) % POSITIONS_SIZE, lastRecord))
            {
                filaEnvio = lastRecord.sendIndex;
                filaEnvioConfirmado = lastRecord.sendIndex;
            }
            else
            {
                Serial.println("[PosQueue] Erro ao ler índice de envio da última posição.");
            }
        }

        initialized = true;
        return true;
    }

    /**
     * @return Número de posições atualmente armazenadas na fila.
     */
    size_t size() const override
    {
        // return (POSICOES_FILA_SIZE - posicoesInicio + posicoesFim) % POSICOES_FILA_SIZE;
        return (POSITIONS_SIZE - filaEnvioConfirmado + filaFim) % POSITIONS_SIZE;
    }

    size_t capacity() const override
    {
        return POSICOES_FILA_SIZE - 1U;
    }

    bool isEmpty() const override
    {
        return filaFim == filaEnvioConfirmado;
    }

    // Acessores para índices (úteis para integrar com código que escreve índice no payload)
    size_t getStart() const override
    {
        return filaEnvioConfirmado;
    }

    size_t getSendIndex() const override
    {
        return filaEnvio;
    }

    size_t getEnd() const override
    {
        return filaFim;
    }

    /**
     * Chamar antes de dequeueForSend para iniciar o processo de envio.
     * Irá fixar o índice de envio na posição atual de início da fila.
     */
    void resetSend() override
    {
        filaEnvio = filaEnvioConfirmado;
    }

    /**
     * Irá marcar que todas as posições até SendIndex foram enviadas/consumidas.
     */
    bool commitSend() override
    {
        // Nada para confirmar
        if(filaEnvio == filaEnvioConfirmado)
        {
            return false;
        }
        filaEnvioConfirmado = filaEnvio;
        return true;
    }

    /**
     * Enfileira uma posição. Se a fila estiver cheia, sobrescreve a mais antiga.
     * @param p Referência à posição que será enfileirada.
     */
    bool enqueue(const Posicao &p) override
    {
        Record record = {p, (uint32_t)filaEnvioConfirmado};
        if (!writeRecordAt(filaFim, record))
        {
            return false;
        }

        filaFim = incrementIndex(filaFim);
        return true;
    }

    bool getAt(size_t index, Posicao &out) const override
    {
        Record record;
        if (!readRecordAt(index, record))
        {
            return false;
        }
        out = record.pos;
        return true;
    }

    /**
     * Desenfileira a próxima posição para envio (Alterando apenas o índice de envio).
     * @param out Referência para onde a posição desenfileirada será copiada (só se retornar true).
     * @return true se havia uma posição para desenfileirar, false se a fila estava vazia.
     */
    [[nodiscard]] bool dequeueForSend(Posicao &out) override
    {
        if (filaEnvio == filaFim)
        {
            return false;
        }

        Record record;
        if (!readRecordAt(filaEnvio, record))
        {
            return false;
        }
        out = record.pos;
        filaEnvio = incrementIndex(filaEnvio);
        return true;
    }

private:
    // Estrutura para localização de posição
    struct FileLocation
    {
        size_t fileSlot;
        size_t localIndex;
    };

    struct Record
    {
        Posicao pos;
        uint32_t sendIndex;
    };

    bool initialized;

    // Tamanho de cada registro (Posicao + 4 bytes sendIndex)
    static constexpr size_t RECORD_SIZE = sizeof(Record);
    static constexpr size_t POSITIONS_PER_FILE = LITTLE_FS_QUEUE_MAX_FILE_SIZE / RECORD_SIZE;
    static constexpr size_t POSITIONS_SIZE = LITTLE_FS_QUEUE_FILES * POSITIONS_PER_FILE;

    static inline size_t incrementIndex(size_t idx)
    {
        return (idx + 1) % POSITIONS_SIZE;
    }

    void mapPositionToFile(size_t positionIndex, FileLocation &outLoc) const
    {
        outLoc.fileSlot = (positionIndex / POSITIONS_PER_FILE) % LITTLE_FS_QUEUE_FILES;
        outLoc.localIndex = positionIndex % POSITIONS_PER_FILE;
    }

    void getFilePath(size_t slot, char *buf, size_t bufSize) const
    {
        snprintf(buf, bufSize, "/positions.%lu.bin", slot);
    }

    bool readRecordAt(size_t positionIndex, Record &record) const
    {
        FileLocation loc;
        mapPositionToFile(positionIndex, loc);
        char path[32];
        getFilePath(loc.fileSlot, path, sizeof(path));
        File f = LittleFS.open(path, FILE_READ);
        if (!f)
            return false;

        size_t offset = loc.localIndex * RECORD_SIZE;
        if(f.size() < offset + RECORD_SIZE)
        {
            f.close();
            return false;
        }

        if (!f.seek(offset))
        {
            f.close();
            return false;
        }

        size_t readBytes = f.read((uint8_t*)&record, RECORD_SIZE);
        f.close();
        return readBytes == RECORD_SIZE;
    }

    bool writeRecordAt(size_t positionIndex, const Record &record)
    {
        FileLocation loc;
        mapPositionToFile(positionIndex, loc);
        char path[32];
        getFilePath(loc.fileSlot, path, sizeof(path));
        File f = LittleFS.open(path, FILE_APPEND); // criar se não existir?
        if (!f)
            return false;

        // Garantir que estamos escrevendo no final do arquivo
        size_t offset = loc.localIndex * RECORD_SIZE;
        size_t size = f.size();
        if(size != offset)
        {
            f.close();
            return false;
        }

        size_t writtenBytes = f.write((const uint8_t*)&record, RECORD_SIZE);
        
        // Arquivo cheio, deleta o próximo
        if(size + RECORD_SIZE >= LITTLE_FS_QUEUE_MAX_FILE_SIZE)
        {
            size_t nextSlot = (loc.fileSlot + 1) % LITTLE_FS_QUEUE_FILES;
            char nextPath[32];
            getFilePath(nextSlot, nextPath, sizeof(nextPath));
            LittleFS.remove(nextPath);

            // Avança o início da fila caso esteja no intervalo do arquivo deletado
            FileLocation loc;
            size_t novoInicio = ((nextSlot + 1) % LITTLE_FS_QUEUE_FILES) * POSITIONS_PER_FILE;

            mapPositionToFile(filaInicio, loc);
            if(loc.fileSlot == nextSlot) { filaInicio = novoInicio; }

            mapPositionToFile(filaEnvioConfirmado, loc);
            if(loc.fileSlot == nextSlot) { filaEnvioConfirmado = novoInicio; }

            mapPositionToFile(filaEnvio, loc);
            if(loc.fileSlot == nextSlot) { filaEnvio = novoInicio; }
        }

        f.close();
        return writtenBytes == RECORD_SIZE;
    }
};

#endif // POSITION_QUEUE_LITTLEFS_H