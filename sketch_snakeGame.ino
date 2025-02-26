#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>
#include <Adafruit_NeoPixel.h>

#define LED_PIN     4
#define LED_COUNT   4 

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Oled display conf
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 10
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// wifi conf
const char* ssid = "your_network_ssid";
const char* password = "your_network_pass";

// mqtt conf
const char* mqtt_server = "mqtt_broker_ip";
const int mqtt_port = 1883;
const char* mqtt_topic = "snake/direction";
WiFiClient espClient;
PubSubClient client(espClient);

struct Point {
  int x;
  int y;
};

// Game Variables
std::vector<Point> snake = { {64, 32} };
Point food = { random(0, SCREEN_WIDTH / 4) * 4, random(0, SCREEN_HEIGHT / 4) * 4 };
String direction = "RIGHT";
String nextDirection = "RIGHT";
int score = 0;

// Draw snake food
void drawGame() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.println(score);
  
  for (const auto& segment : snake) {
    display.fillRect(segment.x, segment.y, 4, 4, SSD1306_WHITE);
  }
  display.fillRect(food.x, food.y, 4, 4, SSD1306_WHITE);
  display.display();
}

// Update Snake Position
void updateSnake() {
  direction = nextDirection;
  Point head = snake.front();
  
  if (direction == "UP") head.y -= 4;
  if (direction == "DOWN") head.y += 4;
  if (direction == "LEFT") head.x -= 4;
  if (direction == "RIGHT") head.x += 4;
  
  // Verifica edge colisions
  if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
    // Restart game
    snake = { {64, 32} };
    direction = "RIGHT";
    nextDirection = "RIGHT";
    score = 0;
    colorWipe(strip.Color(0, 0, 0), 50);
    return;
  }
  
  // Verify body colisions
  for (size_t i = 0; i < snake.size(); i++) {
    if (head.x == snake[i].x && head.y == snake[i].y) {
      // Restart game
      snake = { {64, 32} };
      direction = "RIGHT";
      nextDirection = "RIGHT";
      score = 0;
      colorWipe(strip.Color(0, 0, 0), 50);
      return;
    }
  }
  
  snake.insert(snake.begin(), head);
  
  // Verify if ate food
  if (head.x == food.x && head.y == food.y) {
    score += 10;
    if(score <= 20){
      colorWipe(strip.Color(0, 255, 0), 50);
    }else if (score <= 40){
      strip.clear();
      colorWipe(strip.Color(255, 255, 0), 50);
    }else{
      strip.clear();
      colorWipe(strip.Color(255, 0, 0), 50);
    }
    food = { random(0, SCREEN_WIDTH / 4) * 4, random(0, SCREEN_HEIGHT / 4) * 4 };
  } else {
    snake.pop_back();
  }
}

//callback to recived MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  if ((message == "UP" && direction != "DOWN") ||
      (message == "DOWN" && direction != "UP") ||
      (message == "LEFT" && direction != "RIGHT") ||
      (message == "RIGHT" && direction != "LEFT")) {
    nextDirection = message;
  }
}

// Wifi connection
void connectToWiFi() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Conecting to Wi-Fi...");
  display.display();
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
  
  display.println("\nConected!");
  display.display();
  delay(1000);
}

// mqtt connection
void connectToMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Conecting to MQTT...");
  display.print("Server: ");
  display.println(mqtt_server);
  display.display();
  
  int attempts = 0;
  while (!client.connected() && attempts < 5) {
    attempts++;
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    display.print("Attempts ");
    display.print(attempts);
    display.print(": ");
    display.display();
    
    if (client.connect(clientId.c_str())) {
      display.println("OK!");
      client.subscribe(mqtt_topic);
    } else {
      display.print("Fail (");
      display.print(client.state());
      display.println(")");
      delay(2000);
    }
    display.display();
  }
  
  if (!client.connected()) {
    display.println("MQTT connection failed!");
    display.println("Continuing offline");
    display.display();
    delay(2000);
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(2, 3);
  strip.begin();
  strip.setBrightness(10); 
  strip.show();
  // Init display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Stop if fail
  }
  
  display.clearDisplay();
  display.display();
  delay(2000);
  
  // Connect to wifi and mqtt
  connectToWiFi();
  connectToMQTT();
  
  // Init game
  randomSeed(analogRead(0));
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();
  
  updateSnake();
  drawGame();
  delay(200); // Snake speed
}