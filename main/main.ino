#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define PIN 12
#define BUTTON 6

#define NUM_OF_BALLS 30
#define NUM_OF_LEDS 256
#define HEIGHT 16
#define WIDTH 16

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB + NEO_KHZ800);
#define COLOR = pixel.Color(150, 0, 0)

Adafruit_MPU6050 mpu;
float accelX = 0, accelY = 0, accelZ = 0, gyroX = 0, gyroY = 0, gyroZ = 0, gyroXDiff = 0, gyroYDiff = 0, gyroZDiff = 0, accXDiff = 0, accYDiff = 0, accZDiff = 0;
int matrix[HEIGHT * WIDTH] = {0};

Adafruit_SSD1306 lcd(128, 64); // create display object

class Ball
{
private:
  int posx;
  int posy;
  float xRate;
  float yRate;
  uint32_t color;

public:
  Ball() {}

  Ball(int x, int y, float mx, float my, uint32_t c)
  {
    posx = x;
    posy = y;
    xRate = mx;
    yRate = my;
    color = c;
  }

  int getX() { return posx; }
  void setX(int x) { posx = x; }
  int getY() { return posy; }
  void setY(int y) { posy = y; }
  int getXRate() { return xRate; }
  void setXRate(int rate) { xRate = rate; }
  int getYRate() { return yRate; }
  void setYRate(int rate) { yRate = rate; }
  uint32_t getColor() { return color; }
};

class SquareObstacle
{
private:
  int x;
  int y;
  int size;
  uint32_t color;

public:
  SquareObstacle()
  {
    x = 0;
    y = 0;
    size = 0;
    color = 0;
  }

  SquareObstacle(int xCoord, int yCoord, uint32_t c, int s)
  {
    x = xCoord;
    y = yCoord;
    size = s;
    color = c;
  }

  int getX() { return x; }
  int getY() { return y; }
  int getSize() { return size; }
  uint32_t getColor() { return color; }
};

const int obstacleSize = 4; // Size of the obstacle (can be adjusted)
uint32_t obstacleColor;     // Color of the obstacle

SquareObstacle obstacles[obstacleSize * obstacleSize];

int count = 0;
void createObstacles()
{
  int obstacleIndex = 0;
  for (int y = 6; y <= 9; y++)
  {
    for (int x = 6; x <= 9; x++)
    {

      if (count <= 11)
      {
        obstacleColor = pixel.Color(41, 24, 0);
      }
      else
      {
        obstacleColor = pixel.Color(60, 105, 30);
      }
      matrix[getIndex(x, y)] = 1;
      obstacles[obstacleIndex] = SquareObstacle(x, y, obstacleColor, obstacleSize);
      obstacleIndex++;
      count++;
    }
  }
}

/* the following code pertains to Triangle Obstacle. to use a triangle obstacle, comment out all of square obstacle code. */

// class TriangleObstacle
// {
// private:
//   int x;
//   int y;
//   int size;
//   uint32_t color;

// public:
//   TriangleObstacle()
//   {
//     x = 0;
//     y = 0;
//     size = 0;
//     color = 0;
//   }

//   TriangleObstacle(int xCoord, int yCoord, uint32_t c, int s)
//   {
//     x = xCoord;
//     y = yCoord;
//     size = s;
//     color = c;
//   }

//   int getX() { return x; }
//   int getY() { return y; }
//   int getSize() { return size; }
//   uint32_t getColor() { return color; }
// };

// const int obstacleSize = 3;
// const uint32_t obstacleColor = pixel.Color(100, 100, 100);

// TriangleObstacle obstacles[(obstacleSize * (obstacleSize + 1)) / 2];

// void createObstacles()
// {
//   int index = 0;
//   for (int y = 8 - obstacleSize / 2; y <= 8 + obstacleSize / 2; y++)
//   {
//     int startX = 8 - obstacleSize / 2;
//     int endX = 8 + obstacleSize / 2 - (y - (8 - obstacleSize / 2));
//     for (int x = startX; x <= endX; x++)
//     {
//       matrix[getIndex(x, y)] = 1;
//       obstacles[index++] = TriangleObstacle(x, y, obstacleColor, obstacleSize);
//     }
//   }
// }

Ball balls[NUM_OF_BALLS];

int getIndex(int x, int y)
{
  int curX = (y % 2 == 0) ? x : 15 - x;
  return curX + y * 16;
}

void createBalls()
{
  for (int i = 0; i < NUM_OF_BALLS; i++)
  {
    int xRate = 0, yRate = 0;
    while (xRate == 0)
      xRate = random(-1, 1);
    while (yRate == 0)
      yRate = random(-1, 1);
    int x = random(0, 16);
    int y = random(0, 16);
    matrix[getIndex(x, y)] = 1;
    balls[i] = Ball(x, y, xRate, yRate, pixel.Color(random(0, 100), random(0, 100), random(0, 100)));
  }
}

void calibratingLetters()
{
  uint32_t fontColor = pixel.Color(75, 0, 100);
  uint32_t arrowColor = pixel.Color(100, 100, 100);

  pixel.setPixelColor(254, fontColor);
  pixel.setPixelColor(225, fontColor);
  pixel.setPixelColor(222, fontColor);
  pixel.setPixelColor(193, fontColor);
  pixel.setPixelColor(190, fontColor);
  pixel.setPixelColor(189, fontColor);
  pixel.setPixelColor(188, fontColor); // L

  pixel.setPixelColor(250, fontColor);
  pixel.setPixelColor(229, fontColor);
  pixel.setPixelColor(218, fontColor);
  pixel.setPixelColor(197, fontColor);
  pixel.setPixelColor(186, fontColor);
  pixel.setPixelColor(185, fontColor);
  pixel.setPixelColor(184, fontColor);
  pixel.setPixelColor(183, fontColor);
  pixel.setPixelColor(200, fontColor);
  pixel.setPixelColor(215, fontColor);
  pixel.setPixelColor(232, fontColor);
  pixel.setPixelColor(247, fontColor);
  pixel.setPixelColor(248, fontColor);
  pixel.setPixelColor(249, fontColor); // O

  pixel.setPixelColor(245, fontColor);
  pixel.setPixelColor(234, fontColor);
  pixel.setPixelColor(213, fontColor);
  pixel.setPixelColor(202, fontColor);
  pixel.setPixelColor(185, fontColor);
  pixel.setPixelColor(244, fontColor);
  pixel.setPixelColor(212, fontColor);
  pixel.setPixelColor(243, fontColor);
  pixel.setPixelColor(211, fontColor);
  pixel.setPixelColor(242, fontColor);
  pixel.setPixelColor(237, fontColor);
  pixel.setPixelColor(210, fontColor);
  pixel.setPixelColor(205, fontColor);
  pixel.setPixelColor(181, fontColor);
  pixel.setPixelColor(178, fontColor); // A

  pixel.setPixelColor(159, fontColor);
  pixel.setPixelColor(158, fontColor);
  pixel.setPixelColor(157, fontColor);
  pixel.setPixelColor(128, fontColor);
  pixel.setPixelColor(131, fontColor);
  pixel.setPixelColor(127, fontColor);
  pixel.setPixelColor(124, fontColor);
  pixel.setPixelColor(96, fontColor);
  pixel.setPixelColor(99, fontColor);
  pixel.setPixelColor(95, fontColor);
  pixel.setPixelColor(94, fontColor);
  pixel.setPixelColor(93, fontColor); // D

  pixel.setPixelColor(154, fontColor);
  pixel.setPixelColor(133, fontColor);
  pixel.setPixelColor(122, fontColor);
  pixel.setPixelColor(101, fontColor);
  pixel.setPixelColor(90, fontColor); // I

  pixel.setPixelColor(152, fontColor);
  pixel.setPixelColor(135, fontColor);
  pixel.setPixelColor(120, fontColor);
  pixel.setPixelColor(103, fontColor);
  pixel.setPixelColor(88, fontColor);
  pixel.setPixelColor(136, fontColor);
  pixel.setPixelColor(118, fontColor);
  pixel.setPixelColor(149, fontColor);
  pixel.setPixelColor(138, fontColor);
  pixel.setPixelColor(117, fontColor);
  pixel.setPixelColor(106, fontColor);
  pixel.setPixelColor(85, fontColor); // N

  pixel.setPixelColor(147, fontColor);
  pixel.setPixelColor(146, fontColor);
  pixel.setPixelColor(145, fontColor);
  pixel.setPixelColor(144, fontColor);
  pixel.setPixelColor(140, fontColor);
  pixel.setPixelColor(115, fontColor);
  pixel.setPixelColor(108, fontColor);
  pixel.setPixelColor(83, fontColor);
  pixel.setPixelColor(82, fontColor);
  pixel.setPixelColor(81, fontColor);
  pixel.setPixelColor(80, fontColor);
  pixel.setPixelColor(111, fontColor);
  pixel.setPixelColor(112, fontColor);
  pixel.setPixelColor(113, fontColor); // G

  pixel.setPixelColor(68, arrowColor);
  pixel.setPixelColor(59, arrowColor);
  pixel.setPixelColor(36, arrowColor);
  pixel.setPixelColor(34, arrowColor);
  pixel.setPixelColor(38, arrowColor);
  pixel.setPixelColor(28, arrowColor);
  pixel.setPixelColor(27, arrowColor);
  pixel.setPixelColor(26, arrowColor);
  pixel.setPixelColor(4, arrowColor); // left arrow

  pixel.setPixelColor(75, arrowColor);
  pixel.setPixelColor(52, arrowColor);
  pixel.setPixelColor(43, arrowColor);
  pixel.setPixelColor(41, arrowColor);
  pixel.setPixelColor(45, arrowColor);
  pixel.setPixelColor(21, arrowColor);
  pixel.setPixelColor(20, arrowColor);
  pixel.setPixelColor(19, arrowColor);
  pixel.setPixelColor(11, arrowColor); // right arrow

  pixel.show();
}

void calibrationFunc()
{
  int rounds = 250;
  // Potentially have progress bar???

  calibratingLetters();

  float gX = 0, gY = 0, gZ = 0, aX = 0, aY = 0, aZ = 0;
  for (int i = 0; i < rounds; i++)
  {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    aX += a.acceleration.x;
    aY += a.acceleration.y;
    aZ += a.acceleration.z;
    gX += g.gyro.x;
    gY += g.gyro.y;
    gZ += g.gyro.z;
    delay(25);
  }
  gyroXDiff = gX / rounds;
  gyroYDiff = gY / rounds;
  gyroZDiff = gZ / rounds;
  accXDiff = aX / rounds;
  accYDiff = aY / rounds;
  accZDiff = aZ / rounds - 9.80;

  // int fLetters[8][5] = {
  //   {5, 4, 3, 2, 1},    // F
  //   {6, 7, 8, 9, 10},    // I
  //   {21, 20, 19, 18, 17},   // N
  //   {22, 23, 24, 25, 26},   // I
  //   {37, 36, 35, 34, 33},   // S
  //   {38, 39, 40, 41, 42},   // H
  //   {53, 52, 51, 50, 49},   // E
  //   {54, 55, 56, 57, 58}    // D
  // };

  // pixel.clear();

  // for (int i = 0; i < 8; i++)
  // {
  //   for (int j = 0; j < 5; j++)
  //   {
  //     int index;
  //     if (i % 2 == 0)
  //     {
  //       index = fLetters[i][j];
  //     }
  //     else
  //     {
  //       index = 15 - fLetters[i][j];
  //     }
  //     pixel.setPixelColor(index, color);
  //   }
  // }

  pixel.show();
  delay(4000);
  pixel.clear();
}

void drawPixel(Ball &ball)
{
  int index = getIndex(ball.getX(), ball.getY());
  pixel.setPixelColor(index, ball.getColor());
}

void setup()
{
  pixel.setBrightness(50);
  createObstacles();
  pixel.begin();
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);

  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // set accelerometer range to +-8G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);      // set gyro range to +- 500 deg/s
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);   // set filter bandwidth to 21 Hz

  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C); // init
  lcd.clearDisplay();
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);
  lcd.setCursor(0, 0);

  for (int i = 0; i < NUM_OF_BALLS; i++)
  {
    drawPixel(balls[i]);
  }

  calibrationFunc();
  createBalls();
}

void checkCollisions(Ball &ball)
{
  int x = ball.getX();
  int y = ball.getY();
  int newX = x + ball.getXRate();
  int newY = y + ball.getYRate();

  if (newX > 15)
  {
    ball.setXRate(ball.getXRate() / -2);
    newX = WIDTH - 1;
  }
  else if (newX < 0)
  {
    ball.setXRate(ball.getXRate() / -2);
    newX = 0;
  }

  if (newY > 15)
  {
    ball.setYRate(ball.getYRate() / -2);
    newY = HEIGHT - 1;
  }
  else if (newY < 0)
  {
    ball.setYRate(ball.getYRate() / -2);
    newY = 0;
  }

  int oldIndex = getIndex(x, y);
  int newIndex = getIndex(newX, newY);

  if (oldIndex != newIndex && matrix[newIndex])
  {
    int delta = abs(newIndex - oldIndex);
    if (delta == 1)
    {
      newX = x;
      ball.setXRate(ball.getXRate() / -2);
      newIndex = oldIndex;
    }
    else if (abs(ball.getXRate()) >= abs(ball.getYRate()))
    {
      newIndex = getIndex(newX, y);
      if (!matrix[newIndex])
      {
        ball.setYRate(ball.getYRate() / -2);
        newY = y;
      }
      else
      {
        newIndex = getIndex(x, newY);
        if (!matrix[newIndex])
        {
          ball.setXRate(ball.getXRate() / -2);
          newX = x;
        }
        else
        {
          newX = x;
          newY = y;
          ball.setXRate(ball.getXRate() / -2);
          ball.setYRate(ball.getYRate() / -2);
          newIndex = oldIndex;
        }
      }
    }
    else
    {
      newIndex = getIndex(x, newY);
      if (!matrix[newIndex])
      {
        ball.setXRate(ball.getXRate() / -2);
        newX = x;
      }
      else
      {
        newIndex = getIndex(newX, y);
        if (!matrix[newIndex])
        {
          ball.setYRate(ball.getYRate() / -2);
          newY = y;
        }
        else
        {
          newX = x;
          newY = y;
          ball.setXRate(ball.getXRate() / -2);
          ball.setYRate(ball.getYRate() / -2);
          newIndex = oldIndex;
        }
      }
    }
  }

  matrix[oldIndex] = 0;
  matrix[newIndex] = 1;
  ball.setX(newX);
  ball.setY(newY);
}

void accelGyroData()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  accelX = a.acceleration.x - accXDiff;
  accelY = a.acceleration.y - accYDiff;
  accelZ = a.acceleration.z - accZDiff;

  gyroX = g.gyro.x - gyroXDiff;
  gyroY = g.gyro.y - gyroYDiff;
  gyroZ = g.gyro.z - gyroZDiff;

  // Resting: 0, 0, 9.8
  // Left: 0, -9.8, 0
  // Right: 0, 9.8, 0
  // Up: -9.8, 0, 0
  // Down: 9.8, 0, 0
  // Flipped: 0, 0, -9.8

  int16_t ax = accelX / 1.225;
  int16_t ay = accelY / 1.225;
  int16_t az = accelZ / 4.9;

  float mX = ay;
  float mY = -az;
  mY -= ax / 2;

  int16_t az2 = az * 2 + 1;

  if (mX != 0)
    mX /= abs(mX);
  if (mY != 0)
    mY /= abs(mY);

  for (int i = 0; i < NUM_OF_BALLS; i++)
  {
    int randX, randY;
    if (balls[i].getX() >= 8)
      randX = -0.08;
    else
      randX = 0.08;
    if (balls[i].getY() >= 8)
      randY = -0.08;
    else
      randY = 0.08;
    balls[i].setXRate(balls[i].getXRate() - mX + randX);
    balls[i].setYRate(balls[i].getYRate() + mY + randY);
  }
}

byte prev = 1;
bool paused = false;
unsigned long lastClick = 0;
void loop()
{
  byte cur = digitalRead(BUTTON);
  if (cur == 0 && prev == 1 && millis() > lastClick + 150)
  {
    paused = !paused;
    lastClick = millis();
  }
  if (!paused)
  {
    accelGyroData();
    pixel.clear();
    for (int i = 0; i < NUM_OF_BALLS; i++)
    {
      checkCollisions(balls[i]);
      drawPixel(balls[i]);
    }

    for (int i = 0; i < pow(obstacleSize, 2); i++)
    {
      int curIndex = getIndex(obstacles[i].getX(), obstacles[i].getY());
      pixel.setPixelColor(curIndex, obstacles[i].getColor());
    }

    pixel.show();
    delay(125);
  }
  prev = cur;
}
