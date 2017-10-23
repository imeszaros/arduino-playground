#include "pmf_player.h"
#include "pmf_tune.cpp"

static pmf_player s_player;

void setup()
{
  s_player.start(s_pmf_file);
  delay(1000);
}

void loop()
{
  s_player.update();
}
