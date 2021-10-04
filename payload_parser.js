// Search the payload variable in the payload global variable. It's contents is always [ { variable, value...}, {variable, value...} ...]
const payload_raw = payload.find(x => x.variable === 'helium_payload' || x.variable === 'decoded' || x.variable === 'payload' || x.variable === 'weight');
if (payload_raw) {
  try {
    // Convert the data from Hex to Javascript Buffer.
    const buffer = Buffer.from(payload_raw.value, 'hex');

    // More information about buffers can be found here: https://nodejs.org/api/buffer.html
    const data = [
      { variable: 'weight', value: buffer.readFloatLE().toFixed(2) }
    ];

    // This will concat the content sent by your device with the content generated in this payload parser.
    // It also add the field "serie" and "time" to it, copying from your sensor data.
    payload = payload.concat(data.map(x => ({ ...x, serie: payload_raw.serie, time: payload_raw.time })));
  } catch (e) {
    // Print the error to the Live Inspector.
    console.error(e);

    // Return the variable parse_error for debugging.
    payload = [{ variable: 'parse_error', value: e.message }];
  }
}
