// Thats the code to control the display

// Define CRC8 settings
const uint8_t crc_width = 8;
const uint8_t crc_poly = 0x31;
const uint8_t crc_init = 0x0a;
const bool crc_refin = true;
const bool crc_refout = true;
const uint8_t crc_xorout = 0x00;
const uint8_t crc_check = 0xc1;
const uint8_t crc_residue = 0x00;


// Currently I only know how to turn off the LED
void turnOffDisplayLed() {
  byte myByte[19] = { 0x4C, 0x42, 0x44, 0x43, 0x50, 0x01, 0x10, 0x1B, 0x00, 0x08, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00 };
  byte checksum = crc8(myByte, sizeof(myByte), crc_poly, crc_init, crc_xorout, crc_refin, crc_refout);
  myByte[18] = checksum;
  delay(300);
  for (int i = 0; i < sizeof(myByte); i += 1) {
    Serial2.write(myByte[i]);
  }
  delay(300);
}

void sendDisplayCommand(int speed, byte battery, String status) {
  speed = (speed / 50.0) * 500.0;
  String SPEED_HEX = String(speed, HEX);

  // Add leading zeros to speed if necessary
  while (SPEED_HEX.length() < 4) {
    SPEED_HEX = "0" + SPEED_HEX;
  }

  // Convert battery to hex
  String BATT_HEX = String(battery, HEX);
  BATT_HEX = BATT_HEX.length() < 2 ? "0" + BATT_HEX : BATT_HEX;

  // Create hex command
  String input_str = "4C42444350011011000901" + status + "01" + BATT_HEX + "02" + SPEED_HEX + "0100";
  int input_len = input_str.length();

  uint8_t input_bytes[input_len / 2];  // byte array to store the converted values

  for (int i = 0; i < input_len; i += 2) {
    input_bytes[i / 2] = strtoul(input_str.substring(i, i + 2).c_str(), NULL, 16);
  }

  // Generate checksum
  uint8_t crc_value = crc8(input_bytes, input_len / 2, crc_poly, crc_init, crc_xorout, crc_refin, crc_refout);

  // Concatenate input hex string and CRC8 checksum
  String output_str = input_str + String(crc_value, HEX);
  // Convert concatenated string to bytes and send to display
  for (int i = 0; i < output_str.length(); i += 2) {
    uint8_t byte = strtoul(output_str.substring(i, i + 2).c_str(), NULL, 16);
    Serial2.write(byte);
  }
}