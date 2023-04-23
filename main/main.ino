#include <Adafruit_NeoPixel.h>
#define PIN 12

#define NUM_OF_BALLS 15
#define NUM_OF_LEDS 256

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB+NEO_KHZ800);
#define COLOR = pixel.Color(150, 0, 0)

class Ball {
  private:
    int posx;
    int posy;
    int xRate;
    int yRate;
    uint32_t color;

  public:
    Ball() {}

    Ball(int x, int y, int mx, int my, uint32_t c) {
      posx = x;
      posy = y;
      xRate = mx;
      yRate = my;
      color = c;
    }

    int getX() {return posx;}
    void setX(int x) {posx = x;}
    int getY() {return posy;}
    void setY(int y) {posy = y;}
    int getXRate() {return xRate;}
    void setXRate(int rate) {xRate = rate;}
    int getYRate() {return yRate;}
    void setYRate(int rate) {yRate = rate;}
    uint32_t getColor() {return color;}
};

class Obstacle {
  private:
    int posx;
    int posy;
    uint32_t color;

  public:
    Obstacle() {}

    Obstacle(int x, int y, uint32_t c) {
      posx = x;
      posy = y;
      color = c;
    }

    int getX() {return posx;}
    void setX(int x) {posx = x;}
    int getY() {return posy;}
    void setY(int y) {posy = y;}
    uint32_t getColor() {return color;}
};

Obstacle obstacles[9];

Ball objs[NUM_OF_BALLS]; 

void createBalls() {
  for (int i = 0; i < NUM_OF_BALLS; i++) {
    int xRate = 0, yRate = 0;
    while (xRate == 0) xRate = random(-1, 1);
    while (yRate == 0) yRate = random(-1, 1);
    objs[i] = Ball(random(0, 16), random(0, 16), xRate, yRate, pixel.Color(random(0, 100), random(0, 100), random(0, 100)));
  }
}

void createObstacles() {
  int xStart = 6;
  int yStart = 6;
  int x = xStart;
  int y = yStart;
  int index = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      obstacles[index] = Obstacle(x, y, pixel.Color(100, 100, 100));
      x++;
      index++;
    }
    x = xStart;
    y++;
  }
}

void setup() {
  createBalls();
  createObstacles();
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

bool withinBounds(int x, int y) {
  return x >= 0 && x <= 15 && y >= 0 && y <= 15 ? true : false;
}

void checkCollisions(int x, int y, Ball& ball) {
  if (occupiedPixel(x - ball.getXRate(), y - ball.getYRate())) {ball.setXRate(ball.getXRate() * -1); ball.setYRate(ball.getYRate() * -1);}
  else {
    if (x == 0 || x == 15) ball.setXRate(ball.getXRate() * -1);
    if (y == 0 || y == 15) ball.setYRate(ball.getYRate() * -1);
  }
}

void loop() {
  pixel.clear();
  for (int i = 0; i < NUM_OF_BALLS; i++) {
    int pixelNum = getIndex(objs[i].getX(), objs[i].getY());
    pixel.setPixelColor(pixelNum, objs[i].getColor());
    checkCollisions(objs[i].getX(), objs[i].getY(), objs[i]);
    int newX = objs[i].getX() - objs[i].getXRate();
    int newY = objs[i].getY() - objs[i].getYRate();
    if (withinBounds(newX, newY) && !occupiedPixel(newX, newY)) {objs[i].setX(newX); objs[i].setY(newY);}
  }

  for (int i = 0; i < 9; i++) {
    int curIndex = getIndex(obstacles[i].getX(), obstacles[i].getY());
    pixel.setPixelColor(curIndex, obstacles[i].getColor());
  }
  // Think about how to organize obstacles:
  // - Have predetermined shapes (square, circle, triangle, etc.) that can be called and loaded onto board
  // In future, have custom files that can be translated into objects
  // Reorganize Object classe so it uses coords/inheritance rather than list of pixels for shape
  pixel.show();
  delay(100);
}


/* TODO:
- Need to think about obstacles, different colored balls, objects, etc.
- Also, once base motion is complete, need to start integrating IMU data to be able to dictate movement of objects
  - Challenges: Interpreting IMU data correctly, and then applying this "gravity" to motion of balls on LED board
- May need more efficient method once pixel count grows
*/