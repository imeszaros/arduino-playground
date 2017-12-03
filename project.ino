#include <Bounce2.h>
#include "pmf_player.h"
#include "pmf_tune.cpp"

#define BTN_MUSIC 8

static pmf_player s_player;
Bounce debouncer = Bounce();

bool music = true;

void setup()
{
	//Serial.begin(9600);
  pinMode(BTN_MUSIC, INPUT);

  debouncer.attach(BTN_MUSIC);
  debouncer.interval(5);

  if (music) {
	  s_player.start(s_pmf_file);
  }
}

void loop()
{
	debouncer.update();

	if (music) {
		s_player.update();
	}

  if (debouncer.rose()) {
	 // Serial.println(music);
	  music = !music;
	  if (music) {
		  s_player.start(s_pmf_file);
	  } else {
		  s_player.stop();
	  }
  }
}
