/*
 * Madrona Maling
 * April 11, 2024
 * Prototyping with AI
 * Jon Froehlich
 * CSE 493F
 */

const boolean COMMON_ANODE = false; 

const int RGB_RED_PIN = 9; 
const int RGB_GREEN_PIN  = 11; 
const int RGB_BLUE_PIN  = 10;
const int DELAY_MS = 20; // delay in ms between changing colors
const int MAX_COLOR_VALUE = 255;
const int PHOTOCEL_PIN = A0;
const int LOFI_PIN = A1;
const int FORCE_PIN = A2;

int LAST_BUTTON_STATE = 0;
int CURR_BUTTON_STATE = 0;
int CURR_MODE = 0;
const int INPUT_BUTTON_PIN = 2;
int photoVal = 0;

enum RGB{
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};

int _rgbLedValues[] = {255, 0, 0}; // Red, Green, Blue
enum RGB _curFadingUpColor = GREEN;
enum RGB _curFadingDownColor = BLUE;
const int FADE_STEP = 5;  

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  pinMode(INPUT_BUTTON_PIN, INPUT);
  pinMode(PHOTOCEL_PIN, INPUT);
  pinMode(LOFI_PIN, INPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
  Serial.println("Red, Green, Blue");

  // Set initial color
  setColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
  // delay(DELAY_MS);
}

void loop() {
  // Handles cases of long button presses to accurately and reasonably change modes
  CURR_BUTTON_STATE = digitalRead(INPUT_BUTTON_PIN);
  if (CURR_BUTTON_STATE == HIGH && LAST_BUTTON_STATE == LOW) {
    CURR_MODE = (CURR_MODE + 1) % 3;
  }
  LAST_BUTTON_STATE = CURR_BUTTON_STATE;

  if (CURR_MODE == 0) {
      Serial.println("case 0");
      photoVal = analogRead(PHOTOCEL_PIN);
      Serial.println(photoVal);

      int brightness = map(photoVal, 200, 800, 0, 255);
      brightness = constrain(brightness, 0, 255);
      

      // This LED crossfading code from: https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade.html
      // Increment and decrement the RGB LED values for the current
      // fade up color and the current fade down color
      _rgbLedValues[_curFadingUpColor] += FADE_STEP;
      _rgbLedValues[_curFadingDownColor] -= FADE_STEP;

      // Check to see if we've reached our maximum color value for fading up
      // If so, go to the next fade up color (we go from RED to GREEN to BLUE
      // as specified by the RGB enum)
      // This fade code partially based on: https://gist.github.com/jamesotron/766994
      if(_rgbLedValues[_curFadingUpColor] > MAX_COLOR_VALUE){
        _rgbLedValues[_curFadingUpColor] = MAX_COLOR_VALUE;
        _curFadingUpColor = (RGB)((int)_curFadingUpColor + 1);

        if(_curFadingUpColor > (int)BLUE){
          _curFadingUpColor = RED;
        }
      }

      // Check to see if the current LED we are fading down has gotten to zero
      // If so, select the next LED to start fading down (again, we go from RED to 
      // GREEN to BLUE as specified by the RGB enum)
      if(_rgbLedValues[_curFadingDownColor] < 0){
        _rgbLedValues[_curFadingDownColor] = 0;
        _curFadingDownColor = (RGB)((int)_curFadingDownColor + 1);

        if(_curFadingDownColor > (int)BLUE){
          _curFadingDownColor = RED;
        }
      }

      float adjustedRed = _rgbLedValues[RED] * (float)(255 - brightness) / 255.0;
      float adjustedGreen = _rgbLedValues[GREEN] * (float)(255 - brightness) / 255.0;
      float adjustedBlue = _rgbLedValues[BLUE] * (float)(255 - brightness) / 255.0;

      // Set the color and then delay
      // setColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
      setColor(adjustedRed, adjustedGreen, adjustedBlue);
      // setColor(brightness, brightness, brightness);
      // delay(DELAY_MS);
    } else if (CURR_MODE == 1) {
      int variableResister = analogRead(LOFI_PIN);

      // Map to colors
      if (variableResister < 450) {
        setColor(165, 42, 42); // Brown
      } else if (variableResister < 816) {
        setColor(255, 51, 222); // Pink
      } else {
        setColor(196, 255, 51); // Green
      }
      
    } else if (CURR_MODE == 2) {
      int fsrValue = analogRead(A2); // Read the FSR value

      // This code is from: https://chat.openai.com/
      // Map the FSR reading (0-1023) to the LED color range (0-255)
      int greenValue = map(fsrValue, 0, 1023, 0, 255);
      int redValue = 255 - greenValue; // As green increases, red decreases

      // Set the LED color
      setColor(redValue, greenValue, 0); // No blue component
    }
    delay(100);
  }




/**
 * This function is from: https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade.html
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 */
void setColor(float red, float green, float blue)
{

  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_COLOR_VALUE - red;
    green = MAX_COLOR_VALUE - green;
    blue = MAX_COLOR_VALUE - blue;
  }
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue); 
}