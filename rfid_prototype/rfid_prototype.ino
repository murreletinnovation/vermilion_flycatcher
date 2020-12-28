/* This program was developed for the Quantitative Conservation Lab at the University of Washington. The 
 * code should detect RFID PIT Tags (134.2 kHz) that are within 1 inch of the antenna coil.
 */

#include <SoftwareSerial.h>
SoftwareSerial rfid_serial(5,6);

void setup()
{
  Serial.begin(115200);
  rfid_serial.begin(9600);
  init_rfid();
}

void loop() {
  start_rfid_capture();
  delay(500);
  stop_rfid_capture();
  delay(2000);
}

void flush_buffer()
{
  /* Flush all characters in buffer */
  while(rfid_serial.available())
  {
    rfid_serial.read();
  }
}

void init_rfid()
{
  float freq = 0.0;
  rfid_serial.print("SD2\r");
  rfid_serial.print("SRD\r");
  rfid_serial.print("RSD\r");
  delay(300);
  
  flush_buffer();

  /* Check the frequency of the antenna */ 
  freq = get_rfid_freq();
  Serial.println("Antenna Frequency: " + String(freq) + " kHz");
}

float get_rfid_freq()
{
  float retv = 0.0;
  rfid_serial.print("MOF\r");

  delay(1400);
  if(rfid_serial.available())
  {
    retv = rfid_serial.parseFloat();
  }
  
  return retv;
}

void start_rfid_capture()
{
  rfid_serial.print("SRA\r");
  flush_buffer();
}

void stop_rfid_capture()
{
  uint32_t captured_id = check_rfid_id();
  if(captured_id)
  {
    Serial.println("ID: " + String(captured_id));
  }
  rfid_serial.print("SRD\r");
  flush_buffer();
}

uint32_t check_rfid_id()
{
  uint32_t retv = 0;
  bool found_id = false;
  uint8_t skipped_chars = 0;
  while(rfid_serial.available())
  {
    char c = rfid_serial.read();
    if(found_id)
    {
      if(skipped_chars == 3)
      {
        retv = rfid_serial.parseInt();
        found_id = false;
        flush_buffer();
        break;
      }
      skipped_chars++;
    }
    else if(c == '_')
    {
      found_id = true;
      skipped_chars = 0;
    }
  }
  
  return retv;
}
