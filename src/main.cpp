#include <Arduino.h>

#include "owl_printer.h"

void setup() {
  Serial.begin(115200);
  printer_init();
}

void loop() {
  delay(1000);
  Serial.println("OWL-Printer-MINI running...");
  printer_run();
}
