export const fqaToString = (_fqa: number) => {
  const fqa = Math.floor(_fqa);
  const wire = fqa >> 13 & 0b111;
  const mux = fqa >> 10 & 0b111;
  const bus = fqa >> 7 & 0b111;
  const addr = fqa & 0b1111111;
  return `I2C[${wire}]:${mux}:${bus}:0x${addr.toString(16)}`;
};

// String s = F("I2C[");
//   s += String(I2CIP_FQA_SEG_I2CBUS(fqa), HEX);
//   s += F("]:");
//   if(I2CIP_FQA_SEG_MODULE(fqa) == I2CIP_MUX_NUM_FAKE || I2CIP_FQA_SEG_MUXBUS(fqa) == I2CIP_MUX_BUS_FAKE) {
//     // s += F(" NOMUX ");
//   } else {
//     // s += F(" Subnet ");
//     s += String(I2CIP_FQA_SEG_MODULE(fqa), HEX);
//     s += ':';
//     s += String(I2CIP_FQA_SEG_MUXBUS(fqa), HEX);
//     s += ':';
//   }
//   s += F("0x");
//   if(I2CIP_FQA_SEG_DEVADR(fqa) < 0x10) s += '0';
//   s += String(I2CIP_FQA_SEG_DEVADR(fqa) & 0x7F, HEX);
//   return s;