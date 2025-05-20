#include <Arduino.h>
#include <WiFi.h>
#include "KakaoAPI.h"
#include "Secrets.h"

KakaoAPI kakaoAPI;

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

    // Authenticate with Kakao API
    String token = fetchAccessToken();
    Serial.println("Fetched Kakao access token: " + token);
    kakaoAPI.setAccessToken(token);
    if (kakaoAPI.authenticate() == true) {
        Serial.println("Authenticated with Kakao API");
    } else {
        Serial.println("Kakao API authentication failed");
        return;
    }

    Serial.println("Enter a message to send to Kakao:");
}

void loop() {
    static String message = ""; // Store the message being typed

    // Check if data is available in Serial Monitor
    while (Serial.available() > 0) {
        char incomingChar = Serial.read();
        Serial.print(incomingChar); // Echo the character back

        // Check for newline character (Enter key)
        if (incomingChar == '\n') {
            message.trim(); // Remove any extra whitespace or newline characters

            if (message.length() > 0) {
                Serial.print("Sending message: ");
                Serial.println(message);

                // Send the message to Kakao
                if (kakaoAPI.sendMessage(message)) {
                    Serial.println("Message sent successfully");
                } else {
                    Serial.println("Failed to send message");
                }
            } else {
                Serial.println("Message is empty. Please enter a valid message.");
            }

            // Clear the message buffer for the next input
            message = "";
            Serial.println("Enter another message to send to Kakao:");
        } else {
            // Append the character to the message buffer
            message += incomingChar;
        }
    }
}