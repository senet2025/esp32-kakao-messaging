#include "KakaoAPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "Secrets.h"
#include <Base64.h> // Include the Base64 library

KakaoAPI::KakaoAPI() {}

bool KakaoAPI::authenticate() {
    HTTPClient http;
    String url = "https://kapi.kakao.com/v1/user/access_token_info"; // Kakao API endpoint for token validation
    http.begin(url);
    http.addHeader("Authorization", "Bearer " + accessToken); // 멤버 변수 사용

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        String response = http.getString();
        Serial.println("Authentication successful:");
        Serial.println(response);
        http.end();
        return true; // Authentication successful
    } else {
        Serial.print("Authentication failed with code: ");
        Serial.println(httpResponseCode);
        http.end();
        return false; // Authentication failed
    }
}

bool KakaoAPI::sendMessage(const String& message) {
    if (!authenticate()) {
        Serial.println("Authentication failed");
        return false;
    }

    HTTPClient http;
    String url = "https://kapi.kakao.com/v2/api/talk/memo/default/send"; // Kakao API endpoint
    http.begin(url);
    http.addHeader("Authorization", "Bearer " + accessToken); // 멤버 변수 사용
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String payload = "template_object={\"object_type\":\"text\",\"text\":\"" + message + "\",\"link\":{\"web_url\":\"https://www.kakao.com\",\"mobile_web_url\":\"https://www.kakao.com\"}}";
    
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
        http.end();
        return true; // Message sent successfully
    } else {
        Serial.print("Error on sending message: ");
        Serial.println(httpResponseCode);
        http.end();
        return false; // Failed to send message
    }
}

bool KakaoAPI::sendImage(const uint8_t *imageBuffer, size_t length) {
    HTTPClient http;
    String url = "https://kapi.kakao.com/v2/api/talk/memo/default/send";
    http.begin(url);
    http.addHeader("Authorization", "Bearer " + accessToken); // 멤버 변수 사용
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Convert image to Base64
    String base64Image = base64::encode(imageBuffer, length);

    // Create payload
    String payload = "template_object={"
                     "\"object_type\":\"feed\","
                     "\"content\":{"
                     "\"title\":\"ESP32-CAM Image\","
                     "\"description\":\"Image captured by ESP32-CAM\","
                     "\"image_url\":\"data:image/jpeg;base64," + base64Image + "\","
                     "\"link\":{"
                     "\"web_url\":\"https://www.kakao.com\","
                     "\"mobile_web_url\":\"https://www.kakao.com\""
                     "}"
                     "}"
                     "}";

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode == 200) {
        Serial.println("Image sent successfully");
        http.end();
        return true;
    } else {
        Serial.print("Failed to send image. HTTP response code: ");
        Serial.println(httpResponseCode);
        Serial.println(http.getString()); // Print the error response for debugging
        http.end();
        return false;
    }
}

void KakaoAPI::setAccessToken(const String& token) {
    accessToken = token; // Set the access token
}