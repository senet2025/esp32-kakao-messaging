#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // ArduinoJson 라이브러리 필요
#include "esp_camera.h"
#include "KakaoAPI.h"
#include "Secrets.h"

KakaoAPI kakaoAPI;

// Camera configuration
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

//4 for flash LED
#define LED_GPIO_NUM       4
//33 for normal LED
#define BUILTIN_LED        33

String fetchAccessToken() {
    HTTPClient http;
    String tokenUrl = "http://192.168.0.9:8080/kakao_access_token.json";
    http.begin(tokenUrl);
    int httpCode = http.GET();
    String accessToken = "";
    if (httpCode == 200) {
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error && doc["access_token"].is<String>()) {
            accessToken = doc["access_token"].as<String>();
        }
    }
    http.end();
    return accessToken;
}

void uploadToSupabase(camera_fb_t *fb) {
    HTTPClient http;
    String filename = "esp32cam_" + String(millis()) + ".jpg";
    String url = String(SUPABASE_URL) + "/storage/v1/object/esp32imagebox/" + filename;
    http.begin(url);
    http.addHeader("Authorization", "Bearer " + String(SUPABASE_ANON_KEY));
    http.addHeader("Content-Type", "image/jpeg");
    int httpResponseCode = http.PUT(fb->buf, fb->len);
    String response = http.getString();
    http.end();
    Serial.println(httpResponseCode);
    Serial.println(response);

    if (httpResponseCode == 200 || httpResponseCode == 201) {
        // Supabase public URL 생성
        String publicUrl = String(SUPABASE_URL) + "/storage/v1/object/public/esp32imagebox/" + filename;
        Serial.println("Supabase image URL: " + publicUrl);

        // Kakao 메시지로 이미지 URL 전송
        String kakaoMsg = "ESP32에서 촬영한 이미지입니다: " + publicUrl;
        if (kakaoAPI.sendMessage(kakaoMsg)) {
            Serial.println("Kakao message sent with image URL!");
        } else {
            Serial.println("Failed to send Kakao message with image URL.");
        }
    } else {
        Serial.println("Image upload failed, Kakao message not sent.");
    }
}

void Camera_init_config(){
  camera_config_t config;
  // camera clock 
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  // camera hardware configuration
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  // configuration for streaming
  //config.frame_size = FRAMESIZE_UXGA; 
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  // if PSRAM is present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
      Serial.println("PSRAM is enabled");
      config.frame_size = FRAMESIZE_UXGA;
      config.fb_location = CAMERA_FB_IN_PSRAM;
      config.jpeg_quality = 8;
      config.fb_count = 2;   
  }
  else {
      Serial.println("PSRAM is not enabled");
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_QVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
      config.jpeg_quality = 12;
      config.fb_count = 1; 
  }

  // initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
  }
  else{
      Serial.printf("Camera init successfully!\n");
  }
  // configure camera sensor
  sensor_t * s = esp_camera_sensor_get();
  if(config.pixel_format == PIXFORMAT_JPEG){
      // drop down frame size for higher initial frame rate
      s->set_framesize(s, FRAMESIZE_QVGA);
      s->set_quality(s, 12);
      // image setup
      s->set_aec2(s,1);
      s->set_aec_value(s,168);
      s->set_agc_gain(s,5);
      s->set_hmirror(s,0);
      // register
      //s->set_reg(s, 0x111, 0x80, 0x80);
  }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize the camera
    Camera_init_config();

    // access token 동적으로 불러오기
    String token = fetchAccessToken();
    Serial.println("Fetched Kakao access token: " + token);
    kakaoAPI.setAccessToken(token);

    // Authenticate with Kakao API
    if (kakaoAPI.authenticate() == true) {
        Serial.println("Authenticated with Kakao API");
    } else {
        Serial.println("Kakao API authentication failed");
        return;
    }

    Serial.println("Enter a message to send to Kakao or press '#' to send an image:");
}

void loop() {
    static String message = ""; // Store the message being typed

    // Check if data is available in Serial Monitor
    while (Serial.available() > 0) {
        char incomingChar = Serial.read();
        Serial.print(incomingChar); // Echo the character back

        // 메시지 버퍼에 추가
        if (incomingChar != '\n') {
            message += incomingChar;
        } else {
            message.trim(); // Remove whitespace

            if (message == "#") {
                Serial.println("\nCapturing image...");
                camera_fb_t *fb = esp_camera_fb_get(); // Capture the image
                if (!fb) {
                    Serial.println("Camera capture failed");
                } else {
                    uploadToSupabase(fb);
                    esp_camera_fb_return(fb); // Return the frame buffer
                }
                Serial.println("Enter a message to send to Kakao or press '#' + Enter to send an image:");
            } else if (message.length() > 0) {
                Serial.print("Sending message: ");
                Serial.println(message);

                // Send the message to Kakao
                if (kakaoAPI.sendMessage(message)) {
                    Serial.println("Message sent successfully");
                } else {
                    Serial.println("Failed to send message");
                }
                Serial.println("Enter another message to send to Kakao or press '#' + Enter to send an image:");
            } else {
                Serial.println("Message is empty. Please enter a valid message.");
            }

            // Clear the message buffer for the next input
            message = "";
        }
    }
}