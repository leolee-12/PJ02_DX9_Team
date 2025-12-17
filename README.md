# JusinSRProject
쥬신 159기 1조 SR 프로젝트입니다

## Resource
https://drive.google.com/drive/folders/1zx3saK3foV18kZLbxc0bIW1SqXbGBRih?usp=drive_link
- 위 구글 드라이브 링크 접속하셔서 Resource 파일 다운받아 Client/Bin 폴더 내부에 넣어서 사용해주세요

## Git 사용 방식
### 첫 프로젝트 다운로드 시
1. 자신의 repository로 현재 프로젝트를 fork하기
2. github desktop 접속
3. clone repository 클릭
4. Repository URL: 자신이 fork한 repository의 url || Locak path: 개인 컴퓨터에 다운로드할 위치
5. 구글 드라이브에서 Resource파일 다운받아 적용

### 프로젝트 작업 전
1. fork했던 repository의 main branch를 기존 repository의 main branch와 동기화
2. local branch를 하나 만들어 원하는 이름으로 설정
3. 해당 branch에 들어가서 작업

⚠️ 절대 main branch에서 작업하면 안됨!

### 수정 작업 업로드 시
1. fork했던 repository의 main branch를 기존 repository의 main branch와 동기화
2. 자신이 작업한 branch를 main branch와 merge
3. local main branch를 fork한 repository의 main branch에 push

⚠️ 절대 원본 repository의 main branch에 push하면 안됨!!!!!!!!

4. 이후 웹사이트 깃허브로 돌아와 fork한 repository와 원본 repository의 main 동기화 (이 작업은 회의 시에 함께 할 예정. 절대 개인이 하지 않을 것)


## 업데이트 사항
### 2025 12 13 업데이트

강의 코드에서 추가,수정했음 (5개월 15일차 프로젝트)

#### 1. 지형 버퍼 생성 시 높이맵 선택적 입력

- 지형 버퍼를 생성할 때 높이맵 경로를 선택적으로 받을 수 있도록 optional<wstring>을 사용.

- 높이맵이 있으면 Ready_Heightmap 호출, 없으면 Ready_Flat 호출.

- CTerrainTex::Create()에서 인자로 높이맵 경로를 넘겨줄 수 있음.

#### 2. 지형 월드좌표 설정 추가

- 지형 오브젝트의 y축 회전값을 선택적으로 받을 수 있도록 optional<float> 추가.

- 지형 오브젝트의 위치 좌표를 선택적으로 받을 수 있도록 optional<_vec3> 추가.

- 위치와 회전이 없으면 기본값(0,0,0 / 회전 0도)으로 설정.

#### 3. 지형 월드좌표 추가에 따른 계산구조 수정

- 기존 코드에서는 지형의 월드좌표 = 로컬좌표 였기 때문에 플레이어의 월드좌표와 지형의 로컬좌표로 지형타기 처리를 해도 문제가 없었음

- 하지만, 지형의 월드좌표를 수정 할 수 있게 되었기 때문에 모든 계산은 같은 좌표계에서 일어나야함

- 따라서 플레이어의 월드좌표를 __지형의 로컬좌표__ 로 내려서 계산 하도록 함.

<u>오브젝트의 좌표를 지형의 월드행렬의 역행렬을 곱해서 지형의 로컬좌표로 내린다.</u>

#### 4. 마우스 피킹 DDA 알고리즘

- 마우스 클릭시 생성하는 광선이 지나가는 셀을 확인하는 용도로 사용

- 기존엔 광선이 어느방향이든 광선이랑 충돌 하고자 하는 지형의 모든 셀을 순회하여 충돌지점을 파악하는 방식 > 연산량 많음

- 현재 광선이 진행하는 방향에 광선이 지나가는 셀을 찾고 해당 셀들만 충돌체크 > 연산량 매우감소

## 업데이트 사항
### 2025 12 14 업데이트

#### 1. 메세지/이벤트 추가

- 각각의 씬은 메세지 채널을 보유함

- 메세지 채널은 IMessageChannel 인터페이스를 상속받음

- 씬 > 레이어 > 오브젝트 각각 생성시에 __씬의 메세지채널__ 을 공유함

- 씬은 항상 메세지 채널을 보유해야함 (Ready시에 메세지 채널 생성해줄것)

- 레이어, 오브젝트는 채널을 보유 할 수도 안 할 수도

- 해당 채널을 공유하는 객체들끼리 이벤트를 공유함

- 예시. 플레이어가 보스방 입장 이벤트발생 > 씬이 이벤트확인 > 씬이 보스전 진입 이벤트 발생 > 해당 이벤트를 구독중인 모든 오브젝트는 상호작용.

## 업데이트 사항
### 2025 12 17 업데이트

#### 1. 알파소팅 추가

- 수업코드랑 동일

#### 2. 빌보드 추가

- 수업코드에서의 변경점 -> 객체 내부에서 계산하지않고 Transform 컴포넌트에 Compute_Bilboard 함수 추가

```cpp
void CTransform::Compute_Bilboard(BILBOARD Axis)
{
    _matrix	matView, matBill;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matBill);

    switch (Axis) {
    case BBD_X:
        matBill._22 = matView._22;
        matBill._23 = matView._23;
        matBill._32 = matView._32;
        matBill._33 = matView._33;
        break;
    case BBD_Y:
        matBill._11 = matView._11;
        matBill._13 = matView._13;
        matBill._31 = matView._31;
        matBill._33 = matView._33;
        break;
    case BBD_Z:
        matBill._11 = matView._11;
        matBill._12 = matView._12;
        matBill._21 = matView._21;
        matBill._22 = matView._22;
        break;
    }

    D3DXMatrixInverse(&matBill, 0, &matBill);

    m_matWorld = matBill * m_matWorld;
}
```

- 해당 함수는 __꼭 객체의 Late_Update 에서 호출 할 것__ 매 프레임마다 Transform의 Update에서 월드행렬을 갱신하기때문

- 렌더 최상단에서 월드 행렬 적용시에 그냥 기존과 동일하게 Transform의 월드행렬 받아와서 적용하면됨 (월드행렬의 소유, 생성, 저장은 Transform이 독점 [단일책임])

#### 3. TestEffect 클래스 추가

- 그냥 뭐 수업코드입니다 네

#### 4. Layer 내부 수정

- 수업코드에서는 Layer에 동일한 키값의 오브젝트를 삽입하기위해 멀티맵을 사용

- 저는 ``` map<const _tchar*, vector<GameObject*>> ```로 수정했습니다.

- 수정에 따른 내부 Update, Late_Update, Free 수정 (한 번 쯤 보세용)

- ```Layer->Get_Component``` 내부가 조금 수정됐는데, 기존에 키값으로 접근해서 벨류의 게터를 호출했다면 지금은 키값에접근해서 벨류(vector)의 front의 게터를 호출하고있습니다.

- 따라서 묶음으로 관리하는 오브젝트 키값이 있는데 내부의 특정오브젝트의 컴포넌트가 필요하면 내부 벡터를 원하는 조건으로 정렬 시켜야되기 때문에 정렬기능 필요하면 말해주세요.

#### 5. LightMgr, Light 추가

- 수업코드랑 동일





