#include "mbed.h"

InterruptIn botao(p5);
DigitalOut ledAz(p7);
DigitalOut ledLar(p6);
PwmOut luz(p8);

Timeout timeout_led;
Timeout timeout_pressing;
Timeout timeout_increase_light;
Timeout timeout_decrease_light;

enum STATE
{
  DOWN, UP, FALL, MIN, RISE, MAX
};

STATE estado = UP;

float time_press = 0;
bool press_flag = false;

void to_down();
void to_up();
void to_fall();
void to_min();
void to_rise();
void to_max();


void comecar_apertar();
void acabar_apertar();

void limpar();

int main()
{
  to_up();
  luz = 0;
  botao.rise(&comecar_apertar);
  botao.fall(&acabar_apertar);
}

void apertando()
{
  time_press += 0.1;

  if (luz < 0)
    luz = 0;
  else if (luz > 1)
    luz = 1;

  if (time_press >= 1)
    switch (estado)
    {
    case UP: // tBt >= 1s && i < 1.0
      to_rise();
      break;
    case DOWN: // tBt >= 1s && i > 0.0
      to_fall();
      break;
    case RISE: // Bt = 1 && i = 1.0
      if (luz >= 1)
        to_max();
      break;
    case FALL: // Bt = 1 && i = 0.0
      if (luz <= 0)
        to_min();
      break;
    default:
      printf("apertando estado exception\n");
      break;
    }
  if (press_flag)
    timeout_pressing.attach(&apertando, 0.1);
}
void comecar_apertar()
{
  printf("comecar_apertar\n");

  time_press = 0;
  press_flag = true;
  timeout_pressing.attach(&apertando, 0.1);
}

void acabar_apertar()
{
  printf("acabar_apertar\n");

  press_flag = false;
  timeout_pressing.detach();
  switch (estado)
  {
  case UP: // tBt < 1s
    to_down();
    break;
  case DOWN: // tBt < 1s
    to_up();
    break;
  case RISE: // Bt release && i < 1.0
    to_up();
    break;
  case FALL: // Bt release && i > 0.0
    to_down();
    break;
  case MAX: // Bt release
    to_down();
    break;
  case MIN: // Bt release
    to_up();
    break;
  default:
    printf("acabar_apertar estado exception\n");
    break;
  }
}

void to_up()
{
  estado = UP;

  printf("estado UP\n");

  limpar();
  ledAz = 1;
  timeout_increase_light.detach();
}

void to_down()
{
  estado = DOWN;

  printf("estado DOWN\n");

  limpar();
  ledLar = 1;
  timeout_decrease_light.detach();
}

void aumentar_luz()
{
  if (luz < 1)
    luz = luz + 0.05;
  timeout_increase_light.attach(&aumentar_luz, 1);
}

void piscar_ledAz2s()
{
  ledAz = 1;
  wait(0.2);
  ledAz = 0;
  if (estado == RISE)
    timeout_led.attach(&piscar_ledAz2s, 0.8);
}

void to_rise()
{
  estado = RISE;

  printf("estado RISE\n");

  piscar_ledAz2s();
  timeout_increase_light.attach(&aumentar_luz, 1);
}

void diminuir_luz()
{
  if (luz > 0)
    luz = luz - 0.05;
  timeout_decrease_light.attach(&diminuir_luz, 1);
}

void piscar_ledLar2s()
{
  ledLar = 1;
  wait(0.2);
  ledLar = 0;
  if (estado == FALL)
    timeout_led.attach(&piscar_ledLar2s, 0.8);
}

void piscar_ledAz()
{
  ledAz = 1;
  wait(0.1);
  ledAz = 0;
  if (estado == MAX)
    timeout_led.attach(&piscar_ledAz, 0.1);
}

void to_fall()
{
  estado = FALL;

  printf("estado FALL\n");

  piscar_ledLar2s();
  timeout_decrease_light.attach(&diminuir_luz, 1);
}

void to_max()
{
  estado = MAX;

  printf("estado MAX\n");

  limpar();
  piscar_ledAz();
  timeout_increase_light.detach();
}

void piscar_ledLar()
{
  ledLar = 1;
  wait(0.1);
  ledLar = 0;
  if (estado == MIN)
    timeout_led.attach(&piscar_ledLar, 0.1);
}

void to_min()
{
  estado = MIN;

  printf("estado MIN\n");

  limpar();
  piscar_ledLar();
  timeout_decrease_light.detach();
}

void limpar()
{
  ledAz = 0;
  ledLar = 0;
  timeout_led.detach();
}