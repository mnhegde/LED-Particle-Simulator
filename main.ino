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
  // put your setup code here, to run once:
  createBalls();
  pixel.begin();
  Serial.begin(9600);
}

// x from 0 to 15
// y from 0 to 15

// 239 = 15, 14
// Know for calculating correct index;

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
  // put your main code here, to run repeatedly:
  // For detecting collisions between balls - not working because we are updating the objects in the list order
  // However, when pixels are cleared, the states disappear, so then those above may move down thinking it is free
  // Need better way to manage states - use occupiedPixel func

  pixel.clear();
  Serial.println("New Iteration: ");
  for (int i = 0; i < NUM_OF_BALLS; i++) {
    int pixelNum = getIndex(objs[i].getX(), objs[i].getY());
    pixel.setPixelColor(pixelNum, pixel.Color(150, 0, 0));
    Serial.println(pixel.getPixelColor(getIndex(objs[i].getX(), objs[i].getY()-1)));
    if (objs[i].getY() > 0 && !occupiedPixel(objs[i].getX(), objs[i].getY()-1)) objs[i].setY(objs[i].getY() - objs[i].getRate());
  }
  pixel.show();
  delay(100);
}
