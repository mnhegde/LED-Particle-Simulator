#include <Adafruit_NeoPixel.h>
#define PIN 12

#define NUM_OF_BALLS 10
#define NUM_OF_LEDS 256

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB+NEO_KHZ800);
#define COLOR = pixel.Color(150, 0, 0)

class Ball {
  private:
    int posx;
    int posy;
    int rate;

  public:
    Ball() {}

    Ball(int x, int y, int r) {
      posx = x;
      posy = y;
      rate = r;
    }

    int getX() {return posx;}
    int getY() {return posy;}
    void setY(int y) {posy = y;}
    int getRate() {return rate;}
    // need rate for x and y - so (x, y) and mx, my
};

Ball objs[NUM_OF_BALLS]; 

void createBalls() {
  for (int i = 0; i < NUM_OF_BALLS; i++) {
    objs[i] = Ball(random(0, 16), random(0, 16), 1);
  }
}


void setup() {
  createBalls();
  pixel.begin();
  Serial.begin(9600);
}

// x from 0 to 15
// y from 0 to 15

int getIndex(int x, int y) {
  int curX = (y % 2 == 0) ? x : 15 - x;
  return curX + y*16;
}

// May need more efficient method once pixel count grows
bool occupiedPixel(int x, int y) {
  for (int i = 0; i < NUM_OF_BALLS; i++) {
    if (objs[i].getX() == x && objs[i].getY() == y) return true;
  }
  return false;
}

void loop() {
  pixel.clear();
  for (int i = 0; i < NUM_OF_BALLS; i++) {
    int pixelNum = getIndex(objs[i].getX(), objs[i].getY());
    pixel.setPixelColor(pixelNum, pixel.Color(150, 0, 0));
    if (objs[i].getY() > 0 && !occupiedPixel(objs[i].getX(), objs[i].getY()-1)) objs[i].setY(objs[i].getY() - objs[i].getRate());
  }
  pixel.show();
  delay(100);
}


/* TODO:
- Need to add movement across x-axis now
  - Have both mx and my represented in class, as well as for collision and edge dection
- Once x and y motion is established, along with collisions, can start thinking about representing different states on board
- Need to think about obstacles, different colored balls, objects, etc.
- Also, once base motion is complete, need to start integrating IMU data to be able to dictate movement of objects
  - Challenges: Interpreting IMU data correctly, and then applying this "gravity" to motion of balls on LED board
- May need more efficient method once pixel count grows
*/