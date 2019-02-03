#include <SPI.h>

#include "dac.h"

DAC_MCP4921::DAC_MCP4921() :
		spi_divider(SPI_CLOCK_DIV2) {

	if (&DAC_MCP4921_SS_PORT == &PORTB) {
		DDRB |= 1 << DAC_MCP4921_SS_PIN;
	} else if (&DAC_MCP4921_SS_PORT == &PORTC) {
		DDRC |= 1 << DAC_MCP4921_SS_PIN;
	} else if (&DAC_MCP4921_SS_PORT == &PORTD) {
		DDRD |= 1 << DAC_MCP4921_SS_PIN;
	}

	DAC_MCP4921_SS_PORT |= 1 << DAC_MCP4921_SS_PIN;

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(spi_divider);
}

boolean DAC_MCP4921::setSPIDivider(int _div) {
	switch (_div) {
	case SPI_CLOCK_DIV2:
	case SPI_CLOCK_DIV4:
	case SPI_CLOCK_DIV8:
	case SPI_CLOCK_DIV16:
	case SPI_CLOCK_DIV32:
	case SPI_CLOCK_DIV64:
	case SPI_CLOCK_DIV128:
		spi_divider = _div;
		SPI.setClockDivider(_div);
		return true;
	default:
		return false;
	}
}

void DAC_MCP4921::output(unsigned short data) {
	data &= 0xfff;

	DAC_MCP4921_SS_PORT &= ~(1 << DAC_MCP4921_SS_PIN);

	uint16_t out = (3 << 12) | data;

	SPI.transfer((out & 0xff00) >> 8);
	SPI.transfer(out & 0xff);

	DAC_MCP4921_SS_PORT |= (1 << DAC_MCP4921_SS_PIN);
}

void DAC_MCP4921::shutdown(void) {
	DAC_MCP4921_SS_PORT &= ~(1 << DAC_MCP4921_SS_PIN);

	unsigned short out = 1 << 13;
	SPI.transfer((out & 0xff00) >> 8);
	SPI.transfer(out & 0xff);

	DAC_MCP4921_SS_PORT |= (1 << DAC_MCP4921_SS_PIN);
}
