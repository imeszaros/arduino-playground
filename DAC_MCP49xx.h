#ifndef _DAC_MCP49xx_H
#define _DAC_MCP49xx_H

#ifndef DAC_MCP49XX_SS_PORT
#define DAC_MCP49XX_SS_PORT PORTB
#endif

#ifndef DAC_MCP49XX_SS_PIN
#define DAC_MCP49XX_SS_PIN 2
#endif

#ifndef DAC_MCP49XX_LDAC_PORT
#define DAC_MCP49XX_LDAC_PORT PORTD
#endif

#ifndef DAC_MCP49XX_LDAC_PIN
#define DAC_MCP49XX_LDAC_PIN 7
#endif

#include <SPI.h>
#include <Arduino.h>
#include <inttypes.h>

#ifndef _SPI_H_INCLUDED
#error Please include SPI.h before DAC_MCP49xx.h!
#endif

// Microchip MCP4901/MCP4911/MCP4921/MCP4902/MCP4912/MCP4922 DAC driver
// Thomas Backman, 2012

class DAC_MCP49xx {
  public:

  // These are the DAC models we support
  enum Model {
    MCP4901 = 1, /* single, 8-bit */
    MCP4911,     /* single, 10-bit */
    MCP4921,     /* single, 12-bit */
    MCP4902,     /* dual, 8-bit */
    MCP4912,     /* dual, 10-bit */
    MCP4922      /* dual, 12-bit */
  };

  enum Channel {
	  CHANNEL_A = 0,
	  CHANNEL_B = 1
  };
    
  DAC_MCP49xx(Model _model);
  void setBuffer(boolean _buffer);
  boolean setGain(int _gain);
  boolean setSPIDivider(int _spi_divider);
  void shutdown(void);

  /* All of these call _output() internally */
  void output(unsigned short _out);
  void outputA(unsigned short _out); /* same as output(), but having A/B makes more sense for dual DACs */
  void outputB(unsigned short _out);
  void output2(unsigned short _out, unsigned short _out2); // For MCP49x2

  void latch(void); // Actually change the output, if the LDAC pin isn't shorted to ground
  boolean setAutomaticallyLatchDual(bool _latch);

  private:
    void _output(unsigned short _out, Channel _chan);
    int bitwidth;
    boolean bufferVref;
    boolean gain2x; /* false -> 1x, true -> 2x */
    int spi_divider;
    boolean automaticallyLatchDual; /* call latch() automatically after output2() has been called? */
};

#endif
