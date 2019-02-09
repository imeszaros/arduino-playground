#ifndef __DAC_H
#define __DAC_H

#ifndef DAC_MCP4921_SS_PORT
#define DAC_MCP4921_SS_PORT PORTB
#endif

#ifndef DAC_MCP4921_SS_PIN
#define DAC_MCP4921_SS_PIN 4
#endif

#include <SPI.h>
#include <Arduino.h>
#include <inttypes.h>

class DAC_MCP4921 {
public:
	DAC_MCP4921();
	boolean setSPIDivider(int _spi_divider);
	void output(unsigned short _out);
	void shutdown(void);

private:
	int spi_divider;
};

#endif
