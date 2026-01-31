# 🐑 Cult of the Lamb Clone Project

> DirectX 9 기반 컴포넌트 아키텍처 게임 엔진으로 구현한 로그라이크 + 마을 관리 게임

## 📸 스크린샷

| 마을 | 던전 | 보스전 |
|:---:|:---:|:---:|
| ![마을](screenshots/village.png) | ![던전](screenshots/dungeon.png) | ![보스전](screenshots/boss.png) |

<!-- 스크린샷 이미지를 screenshots 폴더에 추가 후 위 경로 수정 -->

## 🎬 시연 영상

[![시연 영상](https://img.youtube.com/vi/VIDEO_ID/0.jpg)](https://www.youtube.com/watch?v=VIDEO_ID)

<!-- 유튜브 업로드 후 VIDEO_ID 부분을 실제 ID로 교체 -->

## 📌 프로젝트 개요

- **개발 기간**: 2024.12 ~ 2025.01 (약 2개월)
- **개발 인원**: 3인 팀 프로젝트
- **개발 환경**: Visual Studio 2022, DirectX 9, x64
- **사용 기술**: C++, DirectX 9, FMOD, ImGui
- **프로젝트 목적**: 3D 그래픽스 프로그래밍 및 게임 엔진 아키텍처 학습

## 🎮 주요 기능

### 🔧 엔진 아키텍처
- **Engine(DLL) + Client(EXE)** 분리 구조
- **컴포넌트 기반 GameObject**: Transform, Texture, Collider 등 조합
- **참조 카운팅 메모리 관리**: AddRef/Release 패턴, 지연 삭제 큐
- **씬/레이어 관리**: CManagement 싱글톤을 통한 게임 흐름 제어

### 🖼️ 렌더링 시스템
- **지연 렌더링**: 그룹별 렌더 순서 관리 (Priority → NonAlpha → Tile → Floor → Alpha → UI)
- **알파 소팅**: Z-depth 기반 투명 오브젝트 정렬
- **빌보드**: X/Y/Z축 빌보드 지원 (Transform 컴포넌트 내장)
- **직교 투영 UI**: RENDER_UI 그룹을 통한 2D UI 렌더링

### 💥 충돌 시스템
- **AABB 충돌**: CCollisionMgr를 통한 그룹 기반 충돌 검사
- **충돌 콜백**: OnCollision 가상함수를 통한 충돌 처리
- **그룹 필터링**: 충돌 그룹 쌍 등록으로 선택적 검사

### 📨 메시지/이벤트 시스템
- **발행/구독 패턴**: IMessageChannel 인터페이스
- **씬 단위 채널**: Scene이 채널 소유, Layer/Object가 공유
- **게임 이벤트**: 보스전 진입, 아이템 획득, UI 상호작용 등

### 🤖 AI 시스템
- **상태 머신**: CAIController 기반 FSM
- **몬스터 AI**: 일반몬스터(N1~N3), 보스(B1~B2) 각각 고유 패턴
- **패턴 시스템**: deque 기반 패턴 큐 + 가중치 랜덤 선택
- **페이즈 시스템**: HP 기반 페이즈 전환, 패턴 강제 삽입

### 🗺️ 지형 시스템
- **월드 좌표 지원**: 지형 위치/회전 설정 가능
- **DDA 마우스 피킹**: 광선 경로 셀만 검사하여 최적화

## 🎯 게임 콘텐츠

### ⚔️ 전투 (던전)
- **플레이어**: 11개 상태 (Idle, Walk, Roll, Attack 콤보, Charge 등)
- **일반 몬스터**: 근접(N1), 분절(N2), 비행(N3)
- **보스**: 미니보스(B1, 4개 마디), 메인보스(B2, 다중 페이즈)
- **투사체**: 중력 옵션, 360도 발사 패턴

### 🏘️ 마을 관리
- **Follower 시스템**: NPC 추종자, 작업 할당
- **Building 시스템**: 건물 건설 (Workshop, 오븐, 성지 등)
- **상호작용**: IInteractable 인터페이스, CInteractMgr 관리

### 🖥️ UI 시스템
- **요리 UI**: 레시피 선택 → 미니게임 → 결과
- **건물 제작 UI**: 슬롯 호버 → InfoCard 표시 → 건설
- **너클본 미니게임**: 주사위 게임 (보드, 캐릭터, 튜토리얼)
- **자원 히스토리 UI**: 획득 자원 표시
- **보스 HP바**: 다층 구조 (배경, 중간, 전면)

### ✨ 이펙트 시스템
- **CEffectMgr**: 이펙트 관리 싱글톤
- **CSpriteEffect**: 스프라이트 시트 기반 이펙트
- **CParticleEffect**: 파티클 이펙트
- **CLetterBox**: 컷씬 레터박스 연출
- **CScreenEffect**: 화면 효과 (흑백, 페이드)

## 🏗️ 아키텍처

### 📁 디렉토리 구조
```
Engine/
├── Header/            → 엔진 헤더 54개 (CGameObject, CComponent, CScene 등)
└── Code/              → 엔진 구현 (DLL 빌드)

Client/
├── Header/            → 클라이언트 헤더 179개 (CPlayer, CMonster, CItem 등)
├── Code/              → 클라이언트 구현 (EXE 빌드)
└── Shader, Font/      → 리소스
```

### 📦 핵심 클래스
| 클래스 | 역할 |
|--------|------|
| CManagement | 씬/레이어 관리 싱글톤 |
| CGameObject | 게임 오브젝트 베이스, 컴포넌트 컨테이너 |
| CComponent | 컴포넌트 베이스 (Transform, Texture 등) |
| CAIController | AI 상태 머신 베이스 |
| CRenderer | 렌더 그룹별 지연 렌더링 |
| IMessageChannel | 발행/구독 이벤트 인터페이스 |

### 📐 설계 원칙
- **컴포넌트 조합**: 상속보다 조합 선호
- **참조 카운팅**: Get() 공유 시 AddRef() 필수
- **지연 삭제**: Update 중 즉시 삭제 금지
- **단일 책임**: Transform이 월드행렬 독점 관리

## 🚀 실행 방법

### 📥 리소스 다운로드
게임 실행에 필요한 리소스 파일(텍스처, 사운드, 맵 데이터 등)은 용량 문제로 별도 제공됩니다.

| 항목 | 링크 |
|:---|:---|
| **Resource 파일** | [📁 Google Drive](https://drive.google.com/drive/folders/1zx3saK3foV18kZLbxc0bIW1SqXbGBRih?usp=drive_link) |

> 다운로드 후 `Client/Bin/Resource` 폴더에 배치해주세요.

### 빌드 및 실행
1. Visual Studio 2022에서 `Frame159.sln` 열기
2. **x64, Debug** 모드 설정
3. **Engine** 프로젝트 먼저 빌드
4. **Client** 프로젝트 빌드 및 실행

## 🌿 Git 브랜치 전략

```
원본 레포 (upstream)
    ↓ Fork
팀원 개인 레포 (origin)
    ↓ Branch
feature/* 브랜치에서 작업
    ↓ PR
원본 레포에 머지
```

### 작업 흐름

1. **Fork**: 원본 레포를 각자의 GitHub 계정으로 Fork
2. **Branch**: 개인 레포의 `main`에서 `feature/*` 브랜치 생성
3. **Commit**: feature 브랜치에서 작업 후 커밋
4. **PR**: 원본 레포에 Pull Request 생성하여 머지
5. **Sync**: 다른 팀원의 작업 반영 시
   - GitHub에서 **Sync fork**로 원본 레포와 동기화
   - 로컬에서 `feature/*` 브랜치에 `main`을 merge하여 갱신

### 주의사항
- ⚠️ **절대 main 브랜치에서 직접 작업하지 않음**
- ⚠️ **원본 레포에 직접 push 금지**

## ✅ 구현 현황

### 엔진 시스템
- [x] **컴포넌트 기반 GameObject** - Transform, Texture, Collider 등 조합
- [x] **참조 카운팅 메모리 관리** - AddRef/Release 패턴, 지연 삭제 큐
- [x] **렌더링 시스템** - 그룹별 지연 렌더링, 알파 소팅, 빌보드
- [x] **충돌 시스템** - AABB 충돌, 그룹 필터링, 콜백
- [x] **이벤트 시스템** - 발행/구독 패턴 (IMessageChannel)
- [x] **멀티스레드 로딩** - 동적 스레드 할당

### 게임 콘텐츠
- [x] **플레이어** - 11개 상태, 콤보, 구르기, 차지 공격
- [x] **일반 몬스터** - 근접(N1), 분절(N2), 비행(N3)
- [x] **보스 몬스터** - 미니보스(B1), 메인보스(B2) - 패턴/페이즈 시스템
- [x] **아이템 시스템** - 즉시 획득, 상호작용 아이템
- [x] **Follower 시스템** - NPC 추종자, 작업 할당
- [x] **Building 시스템** - 건물 건설 (건설→완료 2단계)

### UI 시스템
- [x] **요리 UI** - 레시피 선택 → 미니게임 → 결과
- [x] **건물 제작 UI** - 슬롯 호버 → InfoCard → 건설
- [x] **너클본 미니게임** - 주사위 게임 (튜토리얼 포함)
- [x] **인벤토리 UI** - 아이템 관리, 장비 슬롯
- [x] **보스 HP바** - 다층 구조 UI

### 이펙트/연출
- [x] **스프라이트 이펙트** - 스프라이트 시트 기반
- [x] **파티클 이펙트** - 몬스터 처치, 아이템 획득
- [x] **화면 효과** - 레터박스, 페이드, 흑백
- [x] **사운드 시스템** - FMOD 기반 BGM/효과음

## 💡 기술적 성과

### 엔진 설계
- **컴포넌트 아키텍처** - 상속보다 조합을 선호하는 유연한 GameObject 설계
- **참조 카운팅** - COM 스타일 메모리 관리로 안전한 리소스 해제
- **지연 삭제** - Update 중 삭제 문제 해결을 위한 삭제 큐 구현

### 그래픽스
- **DirectX 9 파이프라인** - 고정 파이프라인 기반 3D 렌더링
- **지연 렌더링** - 그룹별 렌더 순서로 알파 블렌딩 문제 해결
- **빌보드** - Transform 컴포넌트 내 X/Y/Z축 빌보드 지원

### 게임 로직
- **상태 머신 AI** - FSM 기반 몬스터 AI, 페이즈/패턴 시스템
- **발행/구독 이벤트** - 느슨한 결합의 게임 이벤트 시스템
- **DDA 마우스 피킹** - 광선 경로 최적화로 성능 개선

### 협업
- **Fork 기반 Git 전략** - 원본 보호 + 개인 작업 분리
- **모듈화** - Engine/Client 분리로 재사용성 확보

## 📄 라이선스

이 프로젝트는 **Cult of the Lamb**을 학습 목적으로 모작한 포트폴리오 프로젝트입니다.
상업적 사용을 금지하며, 모든 게임 에셋의 저작권은 원 저작자에게 있습니다.

---

<p align="center">
  <b>쥬신 게임 아카데미 159기 1조</b><br>
  2024.12 ~ 2025.01
</p>
