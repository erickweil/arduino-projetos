/**
 * Decode uplink function
 *
 * @param {object} input
 * @param {number[]} input.bytes Byte array containing the uplink payload, e.g. [255, 230, 255, 0]
 * @param {number} input.fPort Uplink fPort.
 * @param {Record<string, string>} input.variables Object containing the configured device variables.
 *
 * @returns {{data: object, errors: string[], warnings: string[]}}
 * An object containing:
 * - data: Object representing the decoded payload.
 * - errors: An array of errors (optional).
 * - warnings: An array of warnings (optional).
 */
function decodeUplink(input) {
  // Cria um ArrayBuffer a partir dos bytes recebidos.
  // Usamos Uint8Array.from para garantir a conversão correta.
  const buffer = Uint8Array.from(input.bytes).buffer;
  
  // DataView nos permite ler tipos de dados multi-byte (como float 32-bit)
  // de um buffer, especificando a ordem dos bytes (endianness).
  const view = new DataView(buffer);
    
  /*
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
  */
  //   count      [ timestamp   | latitude    | longitude  | speed | course | hdop | satellites] * N
  // 00 00 00 00    AB 35 FA 68   9F 6D 67 F8   69 7F 27 DC   01       00      0A     09  
  //                CE 35 FA 68   6B 71 67 F8   DA 81 27 DC   00       00      0B     07

  if(view.byteLength < 4) {
    return {
      data: {},
      errors: ["Payload muito curto."],
      warnings: []
    };
  }

  let i = 0;
  const count = view.getInt32((i++) * 4, true);

  let positions = [];
  let warnings = [];
  for (let pos = 0; pos < (view.byteLength - 4) / 16; pos++) {
    if ((i * 4) + 16 > view.byteLength) {
      warnings.push(`Dados incompletos para a posição ${pos}.`);
      break; // Evita leitura além do buffer
    }

    const timestamp = view.getUint32((i++) * 4, true);
    const latitude = view.getInt32((i++) * 4, true);
    const longitude = view.getInt32((i++) * 4, true);

    const speed = view.getUint8(i * 4 + 0);
    const course = view.getUint8(i * 4 + 1);
    const hdop = view.getUint8(i * 4 + 2);
    const satellites = view.getUint8(i * 4 + 3);
    i++;

    positions.push({
      tipo: "posicao",
      n: count + pos,
      data: new Date(timestamp * 1000).toISOString(),
      coords: [
        latitude / 10000000.0,
        longitude / 10000000.0
      ],
      
      // extra
      vel: speed,
      dir: (course * 360) / 255,
      hdop: hdop / 10.0,
      satellites: satellites
    });
  }

  return {
    data: {
      // compatibilidade rastreio frotas
      software_version: "1.0.0",
      // dados completos
      posicoes: positions
    },
    errors: [],
    warnings: warnings
  };
}
// ------------------------------
const testPayload = `
20 00 00 00 DF 83 FA 68 7A 91 6D F8 92 A1 2A DC 02 00 0E 0D FE 83 FA 68 33 91 6D F8 60 A1 2A DC 01 00 0B 0F 1E 84 FA 68 D6 91 6D F8 23 A1 2A DC 00 00 0A 10 3C 84 FA 68 15 92 6D F8 0B A1 2A DC 01 00 0A 10
`;

function hexStringToByteArray(hexString) {
  return hexString
    .replace(/\s+/g, '') // Remove espaços em branco
    .match(/.{1,2}/g)    // Divide em pares de caracteres
    .map(byte => parseInt(byte, 16)); // Converte cada par para um número
}

// Teste local
const input = {
  bytes: hexStringToByteArray(testPayload),
  fPort: 1,
  variables: {}
};

const output = decodeUplink(input);
console.log(JSON.stringify(output, null, 2));