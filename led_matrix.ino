#include <LedControl.h>


const int DIN_PIN = 7;
const int CS_PIN = 6;
const int CLK_PIN = 5;

const uint64_t IMAGES[] = {
0x3e2222223e3e0808, 0x3e22223e3e2a0808, 0x3e223e3e2a2a0808, 0xbe3e3e2a2a2a0808,
0xbe223e3e2a2a0808, 0xbe22223e3e2a0808, 0xbe2222223e3e0808, 0xbe22223e3e2a0808,
0xbe223e3e2a2a0808, 0xbebe3e2a2a2a0808, 0xbea23e3e2a2a0808, 0xbea2223e3e2a0808,
0xbea222223e3e0808, 0xbea2223e3e2a0808, 0xbea23e3e2a2a0808, 0xbebebe2a2a2a0808,
0xbea2be3e2a2a0808, 0xbea2a23e3e2a0808, 0xbea2a2223e3e0808, 0xbea2a23e3e2a0808,
0xbea2be3e2a2a0808, 0xbebebeaa2a2a0808, 0xbea2bebe2a2a0808, 0xbea2a2be3e2a0808,
0xbea2a2a23e3e0808, 0xbea2a2be3e2a0808, 0xbea2bebe2a2a0808, 0xbebebeaaaa2a0808,
0xbea2bebeaa2a0808, 0xbea2a2bebe2a0808, 0xbea2a2a2be3e0808, 0xbea2a2bebe2a0808,
0xbea2bebeaa2a0808, 0xbebebeaaaaaa0808, 0xbea2bebeaaaa0808, 0xbea2a2bebeaa0808,
0xbea2a2a2bebe0808, 0xbea2a2a2a2be1c08, 0xbea2a2a2a2a21c1c, 0xbea2a2a2a222001c,
0xbea2a2a22222001c, 0xbea2a2222222001c, 0xbea222222222001c, 0xbe2222222222001c,
0x3e2222222222001c, 0x3e2222222222001c, 0x3e22222222221c1c, 0x3e222222223e1c08
};
const int IMAGES_LEN = sizeof(IMAGES) / sizeof(uint64_t);

LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN);

void setup() {
  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0, 10);
}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display.setLed(0, i, j, bitRead(row, j));
    }
  }
}

int i = 0;

void loop() {
  displayImage(IMAGES[i]);
  if (++i >= IMAGES_LEN ) {
    i = 0;
  }
  delay(100);
}
