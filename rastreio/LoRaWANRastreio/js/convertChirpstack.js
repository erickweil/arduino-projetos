function generatreChirpstackLikePayload(info) {
    return {
        deviceInfo: {
            devEui: info.LoRaWAN.devEui,
            tags: {
                MODELO: "ST390-LORA",
                SERIAL: "e0596aba2010"
            }
        },
        rxInfo: [
            {
                gatewayId: "manual"
            }
        ],
        fPort: 1,
        fCnt: 1,
        data: "SGVsbG8gV29ybGQ=",
        object: {
            // compatibilidade rastreio frotas
            software_version: "1.0.0",
            // dados completos
            posicoes: info.historico.map(pos => ({
                tipo: "posicao",
                ...pos
            }))
        }
    };
}

const ipAddress = "10.139.252.141"
const jsonData = await fetch(`http://${ipAddress}/info`, {
    method: "GET",
    headers: {
        "Accept": "application/json"
    }
}).then((res) => {
    return res.json();
});

console.log(JSON.stringify(generatreChirpstackLikePayload(jsonData), null, 2));