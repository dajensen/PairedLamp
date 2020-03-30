
class blinker {
  int state;
  int pin;
  unsigned long end_time;
  boolean is_blinking;
  int blink_time;
  int on_val;
  int off_val;

public:
  blinker(int pin) {
    blinker::pin = pin;
    state = LOW;
    end_time = blink_time = 0;
    is_blinking = false;
  }

  void setup(boolean inverted) {
    pinMode(pin, OUTPUT);

    if(inverted) {
      on_val = LOW;
      off_val = HIGH;
    }
    else {
      on_val = HIGH;
      off_val = LOW;
    }
  }

  // Turns on the LED.  Call check() periodically to turn the LED back off after the time
  // specified by ms has expired.
  void on(int ms) {
      digitalWrite(pin, on_val);
      state = on_val;
      if(ms > 0)
        end_time = millis() + ms;
      else
        end_time = ms;
        
      is_blinking = false;
//      Serial.println("Current time: " + String(millis()));
//      Serial.println("End time: " + String(end_time));
  }
  
  void off() {
      digitalWrite(pin, off_val);
      state = off_val;
      is_blinking = false;
  }

  void toggle() {
      int new_state = state == on_val ? off_val : on_val;
      digitalWrite(pin, new_state);
      state = new_state;
  }

  void startBlinking(int ms) {
    is_blinking = true;
    blink_time = ms;
    end_time = millis() + ms;
  }

  void check() {
    if(is_blinking){
      if(millis() > end_time) {
        toggle();
        end_time = millis() + blink_time;
      }
    }
    else {
//        Serial.println("Current time: " + String(millis()));
      if(end_time > 0) {
        if(millis() > end_time) {
          if(state != off_val) {
            Serial.println("Turning OFF");
            off();
          }
        }
      }
    }
  }

};
