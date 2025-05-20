#ifndef KAKAOAPI_H
#define KAKAOAPI_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <Secrets.h>

class KakaoAPI {
public:
    KakaoAPI();
    bool authenticate();
    bool sendMessage(const String& message);
    bool sendImage(const uint8_t *imageBuffer, size_t length);
    void setAccessToken(const String& token);

private:
    String accessToken; // <-- 멤버 변수로 선언
};

#endif // KAKAOAPI_H