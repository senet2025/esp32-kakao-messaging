# ESP32 Kakao Messaging Project

This project is designed to send messages to Kakao using an ESP32 board. It utilizes the Kakao API for messaging and includes functionality for managing Wi-Fi connections.

## Project Structure

```
esp32-kakao-messaging
├── src
│   ├── main.cpp          # Entry point of the application
│   ├── KakaoAPI.cpp     # Implementation of the KakaoAPI class
│   ├── KakaoAPI.h       # Header file for the KakaoAPI class
│   ├── Secrets.h        # Contains sensitive API keys and tokens
│   └── WiFiManager.cpp   # Manages Wi-Fi connections
├── include
│   └── CameraPins.h     # Defines pin configurations for the camera module
├── lib                  # Additional libraries can be included here
├── platformio.ini      # Configuration file for PlatformIO
└── README.md            # Documentation for the project
```

## Setup Instructions

1. **Clone the Repository**: Clone this repository to your local machine.
   
2. **Install PlatformIO**: Ensure you have PlatformIO installed in your development environment.

3. **Configure Secrets**: Open `src/Secrets.h` and enter your Kakao API keys and tokens.

4. **Connect to Wi-Fi**: Modify the `WiFiManager.cpp` file to include your Wi-Fi credentials.

5. **Build the Project**: Use PlatformIO to build the project.

6. **Upload to ESP32**: Connect your ESP32 board and upload the code.

## Usage Guidelines

- The main functionality is implemented in `src/main.cpp`, which initializes the ESP32, connects to Wi-Fi, and sends messages to Kakao.
- The `KakaoAPI` class in `src/KakaoAPI.cpp` handles the messaging functionality.
- Ensure that your device has internet access to communicate with the Kakao API.

## Additional Information

For any issues or contributions, please refer to the project's GitHub page or contact the maintainer.