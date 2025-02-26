# Snake Game

This project implements a classic Snake Game using an LCD display and WS2812 LEDs. The game is controlled via the backend terminal using the `W`, `A`, `S`, and `D` keys.

## Features
- **Terminal-based Controls:** Move the snake using `W`, `A`, `S`, `D` keys in the backend terminal.
- **Collision Detection:** The game detects collisions with walls and the snake’s own body.
- **Scoring System:** Earn points by collecting food.
- **Random Food Generation:** The game dynamically generates food for the snake to consume and grow.

## Requirements
- **Display:** A compatible graphical display (e.g., OLED SSD1306, LCD 16x2, etc.).
- **LEDs:** WS2812 LED strip.
- **Backend:** A Spring Boot backend for managing game logic.
- **Docker Compose:** Used to set up and run the backend, MQTT, and Kafka services.

## Installation and Usage
1. Upload the `sketch_snakeGame.ino` code to the microcontroller using the Arduino IDE or another compatible environment.
2. Connect the LCD display and WS2812 LEDs according to the wiring diagram.
3. Use the `docker-compose.yml` file available in the backend repository to set up and run the backend, MQTT, and Kafka:
   ```sh
   docker-compose up -d
   ```
4. Control the game from the backend terminal using the `W`, `A`, `S`, `D` keys.

## Author
**Pedro Alberto**

## License
This project is open-source and may be freely used for educational and learning purposes.

