#include <SX126x.h>

SX126x LoRa;

void setup() {

  // Begin serial communication
  Serial.begin(38400);

  // Begin LoRa radio and set NSS, reset, busy, txen, and rxen pin with connected arduino pins
  // IRQ pin not used in this example (set to -1). Set txen and rxen pin to -1 if RF module doesn't have one
  Serial.println("Begin LoRa radio");
  int8_t nssPin = 10, resetPin = 9, busyPin = 4, irqPin = -1, txenPin = 8, rxenPin = 7;
  if (!LoRa.begin(nssPin, resetPin, busyPin, irqPin, txenPin, rxenPin)){
    Serial.println("Something wrong, can't begin LoRa radio");
    while(1);
  }

  // Optionally configure TCXO or XTAL used in RF module
  // Different RF module can have different clock, so make sure clock source is configured correctly
  // uncomment code below to use TCXO
  Serial.println("Set RF module to use TCXO as clock reference");
  uint8_t dio3Voltage = SX126X_DIO3_OUTPUT_1_8;
  uint32_t tcxoDelay = SX126X_TCXO_DELAY_10;
  LoRa.setDio3TcxoCtrl(dio3Voltage, tcxoDelay);
  // uncomment code below to use XTAL
  //uint8_t xtalA = 0x12;
  //uint8_t xtalB = 0x12;
  //Serial.println("Set RF module to use XTAL as clock reference");
  //LoRa.setXtalCap(xtalA, xtalB);
  
  // Set frequency to 915 Mhz
  Serial.println("Set frequency to 915 Mhz");
  LoRa.setFrequency(915000000);

  // Set RX gain. RX gain option are power saving gain or boosted gain 
  Serial.println("Set RX gain to power saving gain");
  LoRa.setRxGain(SX126X_RX_GAIN_POWER_SAVING);                        // Power saving gain

  // Configure modulation parameter including spreading factor (SF), bandwidth (BW), and coding rate (CR)
  // Transmitter must have same SF and BW setting so receiver can receive LoRa packet
  Serial.println("Set modulation parameters:\n\tSpreading factor = 7\n\tBandwidth = 125 kHz\n\tCoding rate = 4/5");
  uint8_t sf = 7;                                                     // LoRa spreading factor: 7
  uint8_t bw = SX126X_LORA_BW_125;                                    // Bandwidth: 125 kHz
  uint8_t cr = SX126X_LORA_CR_4_5;                                    // Coding rate: 4/5
  LoRa.setLoRaModulation(sf, bw, cr);

  // Configure packet parameter including header type, preamble length, payload length, and CRC type
  // The explicit packet includes header contain CR, number of byte, and CRC type
  // Packet with explicit header can't be received by receiver with implicit header mode
  Serial.println("Set packet parameters:\n\tExplicit header type\n\tPreamble length = 12\n\tPayload Length = 15\n\tCRC on");
  uint8_t headerType = SX126X_LORA_HEADER_EXPLICIT;                   // Explicit header mode
  uint16_t preambleLength = 12;                                       // Set preamble length to 12
  uint8_t payloadLength = 15;                                         // Initialize payloadLength to 15
  uint8_t crcType = SX126X_LORA_CRC_ON;                               // Set CRC enable
  LoRa.setLoRaPacket(headerType, preambleLength, payloadLength, crcType);

  // Set syncronize word for public network (0x3444)
  Serial.println("Set syncronize word to 0x3444");
  LoRa.setLoRaSyncWord(0x3444);

  Serial.println("\n-- LoRa RECEIVER --\n");
  
}

void loop() {
  
  // Request for receiving new LoRa packet
  LoRa.request();

  // Put received packet to message and counter variable
  // read() and available() method must be called after request() or listen() method
  const uint8_t msgLen = LoRa.available() - 1;
  char message[msgLen];
  uint8_t counter;
  // available() method return remaining received payload length and will decrement each read() or get() method called
  uint8_t i=0;
  while (LoRa.available() > 1){
    message[i++] = LoRa.read();
  }
  counter = LoRa.read();
  
  // Print received message and counter in serial
  Serial.print(message);
  Serial.print("  ");
  Serial.println(counter);

  // Print packet/signal status including RSSI, SNR, and signalRSSI
  Serial.print("Packet status: RSSI = ");
  Serial.print(LoRa.rssi());
  Serial.print(" dBm | SNR = ");
  Serial.print(LoRa.snr());
  Serial.print(" dB | signalRSSI = ");
  Serial.print(LoRa.signalRssi());
  Serial.println(" dB");
  
  // Show received status in case CRC or header error occur
  if (LoRa.status() == SX126X_STATUS_CRC_ERR) Serial.println("CRC error");
  if (LoRa.status() == SX126X_STATUS_HEADER_ERR) Serial.println("Packet header error");
  Serial.println();

}
