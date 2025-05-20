### ESP32 Kakao Messaging 프로젝트 작업 요약

---

#### 1. **프로젝트 개요**
- ESP32-CAM에서 이미지를 캡처하고, Supabase Storage에 업로드 후 Kakao 메시지로 이미지 URL을 전송하는 IoT 프로젝트.
- Kakao API, Supabase Storage, PlatformIO, C++ 기반.

---

#### 2. **주요 구현 내용**
- **Wi-Fi 연결**: `WiFiManager.cpp`에서 SSID/PW로 네트워크 연결.
- **카메라 초기화 및 이미지 캡처**: `main.cpp`에서 카메라 설정 및 캡처.
- **이미지 업로드**:  
  - Supabase Storage REST API를 사용해 JPEG 이미지를 업로드.
  - 업로드 성공 시 public URL 생성.
- **Kakao 메시지 전송**:  
  - 업로드된 이미지의 public URL을 Kakao 메시지(텍스트)로 전송.
  - Kakao API 인증 및 메시지 전송 기능 구현.
- **시리얼 입력 제어**:  
  - `#` 입력 후 엔터 시 이미지 캡처 및 업로드.
  - 일반 텍스트 입력 후 엔터 시 메시지 전송.

---

#### 3. **보안 및 환경설정**
- 민감 정보(`Secrets.h`, `Secrets.cpp`) 분리 관리, .gitignore로 보호.
- Supabase Storage의 RLS 정책 및 public 권한 설정 필요.
- Kakao API 토큰 갱신 및 관리 방법(shell 스크립트 등) 안내.

---

#### 4. **문제 해결 및 참고**
- Supabase Storage 403/RLS 에러: 정책(SQL Policy) 추가로 해결.
- ESP32 SSL 연결 실패: 시간 동기화 및 네트워크 환경 점검.
- 무료 이미지 업로드 대안(imgur, imgbb 등)도 안내.
- Git 브랜치명 변경, README 구조 및 사용법 정리.

---

#### 5. **README 및 문서화**
- 프로젝트 구조, 설정, 빌드, 업로드, 사용법 등 문서화 완료.
- 주요 파일: `main.cpp`, `KakaoAPI.cpp/h`, `Secrets.h/cpp`, `WiFiManager.cpp`, `CameraPins.h`, README.md

---

### 6. **이후 계획**
- main.cpp 등에서 민감한 정보 가리기
- access token 자동 생성 기능 추가
- 실제 버턴 입력으로 capture방식 변경
- 안정화