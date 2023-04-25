#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define PIN 12
#define BUTTON 0

#define NUM_OF_BALLS 15
#define NUM_OF_LEDS 256

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB+NEO_KHZ800);
#define COLOR = pixel.Color(150, 0, 0)

Adafruit_MPU6050 mpu;
float accelX, accelY, accelZ, gyroX, gyroY, gyroZ, gyroXDiff, gyroYDiff, gyroZDiff, accXDiff, accYDiff, accZDiff;

Adafruit_SSD1306 lcd(128, 64); // create display object

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

// Combine create functions?
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

void calibrationFunc() {
  int rounds = 500;
  // Potentially have progress bar???
  lcd.print("Calibrating accelerometer and gyroscope . . . ");
  lcd.display();
  float gX, gY, gZ, aX, aY, aZ;
  for (int i = 0; i < rounds; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    aX += a.acceleration.x;
    aY += a.acceleration.y;
    aZ += a.acceleration.z;
    gX += g.gyro.x;
    gY += g.gyro.y;
    gZ += g.gyro.z;
    delay(50);
  }
  gyroXDiff = gX/rounds;
  gyroYDiff = gY/rounds;
  gyroZDiff = gZ/rounds;
  accXDiff = aX/rounds;
  accYDiff = aY/rounds;
  accZDiff = aZ/rounds;

  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.println("Calibration finished");
  lcd.println("\nSimulation started!");
  lcd.display();
}

void setup() {
  createBalls();
  createObstacles();
  pixel.begin();
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // set accelerometer range to +-8G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); // set gyro range to +- 500 deg/s
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); // set filter bandwidth to 21 Hz

  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C); // init
  lcd.clearDisplay();
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);
  lcd.setCursor(0, 0);

  calibrationFunc();
}

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

void accelGyroData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  accelX = a.acceleration.x - accXDiff;
  accelY = a.acceleration.y - accYDiff;
  accelZ = a.acceleration.z - accZDiff;

  gyroX = g.gyro.x - gyroXDiff;
  gyroY = g.gyro.y - gyroYDiff;
  gyroZ = g.gyro.z - gyroZDiff;

  Serial.println("Accelerometer data: ");
  Serial.print("x: "); Serial.println(accelX);
  Serial.print("y: "); Serial.println(accelY);
  Serial.print("z: "); Serial.println(accelZ);

  Serial.println("Gyroscope data: ");
  Serial.print("x: "); Serial.println(gyroX);
  Serial.print("y: "); Serial.println(gyroY);
  Serial.print("z: "); Serial.println(gyroZ);
}

byte prev = 1;
bool paused = false;
unsigned long lastClick = 0;
void loop() {
  byte cur = digitalRead(BUTTON);
  if (cur == 0 && prev == 1 && millis() > lastClick + 75) {paused = !paused; lastClick = millis();}
  if (!paused) {
    accelGyroData();
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

    pixel.show();
    delay(100);
  }
  prev = cur;
}
