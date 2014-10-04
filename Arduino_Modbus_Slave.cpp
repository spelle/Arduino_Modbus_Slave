/*
 * Arduino_Modbus_Slave.c
 *
 *  Created on: Sep 17, 2014
 *      Author: nuptse
 */


#include <Arduino.h>
#include <dht.h>
#include <SimpleModbusSlave.h>

dht DHT;

#define DHT22_PIN 2
#define LOOP_PERIOD 5000

//////////////// registers of your slave ///////////////////
enum
{
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  TEMP_VAL,
  HYGRO_VAL,
  GAUGE_VAL,
  HOLDING_REGS_SIZE // leave this one
  // total number of registers for function 3 and 16 share the same register array
  // i.e. the same address space
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

void setup()
{
	DDRB |= (1 << PB5 ) ;
	/* parameters(HardwareSerial* SerialPort,
long baudrate,
unsigned char byteFormat,
unsigned char ID,
unsigned char transmit enable pin,
unsigned int holding registers size,
unsigned int* holding register array)
	 */

	/* Valid modbus byte formats are:
SERIAL_8N2: 1 start bit, 8 data bits, 2 stop bits
SERIAL_8E1: 1 start bit, 8 data bits, 1 Even parity bit, 1 stop bit
SERIAL_8O1: 1 start bit, 8 data bits, 1 Odd parity bit, 1 stop bit

You can obviously use SERIAL_8N1 but this does not adhere to the
Modbus specifications. That said, I have tested the SERIAL_8N1 option
on various commercial masters and slaves that were suppose to adhere
to this specification and was always able to communicate... Go figure.

These byte formats are already defined in the Arduino global name space.
	 */

	modbus_configure(&Serial, 9600, SERIAL_8N2, 2, 3, HOLDING_REGS_SIZE, holdingRegs);
}

void loop()
{
	modbus_update() ;

	static unsigned long ulCurrentTime = 0 ;
	static unsigned long ulNextTime = LOOP_PERIOD ;

	ulCurrentTime = millis() ;

	if( ulCurrentTime > ulNextTime)
	{
		// READ DATA
		//Serial.print("DHT22, \t");
		int chk = DHT.read22(DHT22_PIN);
		switch (chk)
		{
		case DHTLIB_OK:
			Serial.print("OK,\t");
			break;
		case DHTLIB_ERROR_CHECKSUM:
			Serial.print("Checksum error,\t");
			break;
		case DHTLIB_ERROR_TIMEOUT:
			Serial.print("Time out error,\t");
			break;
		default:
			Serial.print("Unknown error,\t");
			break;
		}
		// DISPLAY DATA
		holdingRegs[0] = (int) 10 * DHT.humidity ;
		holdingRegs[1] = (int) 10 * DHT.temperature ;
		holdingRegs[2] = analogRead(0) ;

		PORTB ^= (1 << PB5 ) ;

		ulNextTime = ulCurrentTime + LOOP_PERIOD ;
	}

	delay(1000);
}

