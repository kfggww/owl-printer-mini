#include <Arduino.h>

#include "owl_printer.h"
#include "owl_tasks.h"

void setup() {
  Serial.begin(115200);
  printer_init();
  tasks_init();
}

void loop() {
  delay(1000);
  Serial.println("OWL-Printer-MINI running...");
  printer_run();
}
