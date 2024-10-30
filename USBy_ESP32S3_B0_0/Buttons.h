//Add licence text

//Buttons timing calibration
#define DEBOUNCE_TIME     250
#define BACK_TO_HIGH        2 //number of BUTTON_CHECK_PERIODs
#define BUTTON_SHORT_LOW    1
#define BUTTON_SHORT_HIGH  10
#define BUTTON_LONG        20

struct Button {
  const uint8_t PIN;
  bool pressed;
  bool high_again;
  unsigned long button_time;
  unsigned long last_button_time; //variables to keep track of the timing of recent interrupts
  unsigned long counter;
  unsigned long low_counter;
  bool long_pressed;
};

extern void btnDebounce(Button *button);
extern int resolveButton(Button *button);