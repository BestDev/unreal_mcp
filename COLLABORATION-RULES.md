# AI Collaboration Common Rules (AI 협업 공통 규칙)

## 🎯 Core Philosophy: User-Centric Collaboration Model (사용자 중심 협업 모델)

### Collaborative Design Principle (협업 설계 원칙)
이 협업 규칙은 **사용자 중심 협업 모델**을 기반으로 설계되었습니다. 여러 협업 모델을 검토한 결과, 다음과 같은 이유로 이 모델을 채택했습니다:

**선택 이유:**
- **투명성**: 모든 과정이 사용자에게 명확히 보고됨
- **제어권**: 사용자가 언제든 과정에 개입하고 방향을 수정할 수 있음
- **실용성**: 현재 AI 기술 수준에서 가장 현실적인 접근법

**핵심 구조:**
```
사용자 요청 → Gemini 분석 및 계획 수립 → [사용자 계획 승인] →
Gemini `spec.md` 명세서 작성 → Claude 구현 → Gemini 결과 검증 → [사용자 최종 검토] → 완료
```

### Clear Role Separation (명확한 역할 분리)

```
.kb/ → AI 간 협업 소통 기록 (Internal AI Communication)
[프로젝트]/doc/ → 사용자 문서 (User Documentation)
[프로젝트]/README.md → 프로젝트 소개 (Project Introduction) - 예외
```

## 📋 Documentation Triggers (문서 생성 조건)

### ✅ Automatic Creation (자동 생성) - 반드시 만들어야 함

#### 1. Project Start (프로젝트 시작 시)
```
✅ .kb/projects/[프로젝트명]/spec.md (Gemini 작성)
✅ [프로젝트명]/README.md (Claude 작성)
```

#### 2. Complex Collaboration (복잡한 협업 세션 시) - 다음 조건 중 하나라도 해당
```
Specific Conditions (구체적 조건):
- 2개 이상 AI가 협업하는 경우
- 3개 이상 파일을 수정하는 경우
- 새로운 기술 스택 도입이 필요한 경우
- 사용자가 "세션 기록해줘", "문서화해줘" 요청하는 경우

Auto-Generated (자동 생성):
✅ .kb/sessions/작업명/session-log.md
```

### 📝 Optional Creation (선택적 생성) - 요청 시에만

#### 3. Advanced Documentation Request (고급 문서화 요청 시)
```
Trigger Keywords (트리거 키워드) - 다음 중 하나 포함 시:
- "문서화", "가이드", "매뉴얼", "설명서"
- "사용법", "튜토리얼", "도움말"
- "아키텍처", "설계", "구조"

Generated Documents (생성 문서):
📝 [프로젝트명]/doc/user-guide.md
📝 [프로젝트명]/doc/developer-guide.md  
📝 [프로젝트명]/doc/architecture.md
```

## 🤖 AI Responsibility Areas (AI별 담당 영역)

### Analysis & Planning (분석 및 계획)
- **Gemini CLI**: 요구사항 분석, 아키텍처 설계, 작업 명세서 작성
- **Direct Storage (직접 저장)**: 분석 보고서, 명세서, 설계 문서를 `.kb/projects/[프로젝트명]/`에 저장

### Implementation & Documentation (구현 및 문서화)
- **Claude Code**: 코드 구현, 프로젝트 README 작성, 사용자용 구현 가이드 작성
- **Direct Storage (직접 저장)**: 코드, 테스트, README.md, 빌드 결과물을 프로젝트 폴더에 저장

### Advanced Documentation (고급 문서화)
- **Documentation Specialist**: 사용자 가이드, 개발자 문서 등 고급 문서화 (요청 시에만)

## 📁 Folder Structure (폴더 구조)

### `.kb/` (For AI Collaboration)
```
/작업공간루트/
├── [프로젝트명]/
└── .kb/
    ├── sessions/                    # 세션별 작업 기록
    │   └── 작업명/
    │       ├── session-log.md       # 세션 진행 기록 (메인 세션 AI 작성)
    │       ├── ai-collaboration.md  # AI 간 소통 내역 (메인 세션 AI 작성)
    │       ├── decisions.md         # 주요 결정 사항 (메인 세션 AI 작성)
    │       └── [서브에이전트명]-작업내용키워드.md # 서브에이전트 작업 내용 요약 (담당 서브 에이전트 작성)
    └── projects/                    # 프로젝트별 AI 소통 기록
        └── [프로젝트명]/
            ├── spec.md              # Gemini → Claude 작업 명세 (Gemini 작성)
            ├── analysis.md          # Gemini 분석 보고서 (Gemini 작성)
            └── review.md            # 상호 리뷰 기록 (검증 담당 AI 작성)
```

### `[Project Name]/` (Project Code & Docs)
```
tetris_v2/
├── README.md                       # 프로젝트 소개 (Claude 작성)
├── src/, include/, CMakeLists.txt   # 실제 코드
└── doc/                            # 추가 사용자 문서
    ├── implementation.md           # 구현 보고서 (Claude 작성)
    ├── user-guide.md               # 사용자 가이드
    ├── developer-guide.md          # 개발자 가이드
    └── architecture.md             # 시스템 아키텍처
```

## 📝 Document Standards (문서 작성 표준)

### ✍️ Document Metadata Rule (문서 메타데이터 규칙)

`.kb` 디렉토리 내의 모든 문서는 최상단에 아래 형식의 메타데이터를 포함해야 한다.

> - **Requester (요청자):** `User` | `Gemini` | `Claude`
> - **Author (작성자):** `Gemini` | `Claude (Subagent 이름)`
> - **Timestamp (생성일시):** `YYYY-MM-DD HH:MM:SS`
> - **Purpose (목적):** [문서 생성의 핵심 목적 한 줄 요약]

*참고: `작성자`가 Claude일 경우, 실제 작업을 수행한 전문가 Subagent의 이름을 괄호 안에 명시합니다.*

**예시:**
> - **Requester (요청자):** `User`
> - **Author (작성자):** `Gemini`
> - **Timestamp (생성일시):** `2025-08-18 14:30:00`
> - **Purpose (목적):** `tetris_v3` 프로젝트의 초기 기능 명세 정의.

### 🔤 Keyword Format Rule (키워드 형식 규칙)
- **중요 키워드**: 모든 문서에서 중요한 개념, 기술 용어는 **영문 (한글)** 형식으로 통일하여 작성
- **파일명 규칙**: 모든 파일명은 **영문만** 사용 (예: `spec.md`, `session-log.md`)
- **목적**: 파일 기반 협업에서 AI 간 소통 효율성 극대화 및 검색 편의성 향상
- **적용 범위**: `.kb/` 내 모든 문서, 프로젝트 문서, README.md 등 전체
- **예시**: 
  - ✅ `Specification (명세서)`, `Analysis (분석 보고서)`, `Session Log (세션 로그)`
  - ❌ `명세서`, `Specification`, `분석 보고서 작성`

### 📋 Gemini's spec.md Format (Gemini 명세서 형식)
```markdown
# [프로젝트명] 작업 명세서
## Goal (목표)
[구체적인 목표]

## Requirements (요구사항)  
[기능 요구사항 나열]

## Tech Stack (기술 스택)
[사용할 기술/라이브러리]

## Implementation Guide (구현 가이드)
[Claude가 따라야 할 구현 순서]

## Key Rule Reminder (중요 규칙 재확인)
- **File Naming & Encoding (파일명 및 인코딩)**: 모든 파일명은 영문만, 내용은 한글로 작성하며, 파일 인코딩은 UTF-8로 통일합니다.
- **(기타 이 작업에 특히 중요한 규칙 명시)**

## Completion Criteria (완료 조건)
[작업 완료 판단 기준]
```

### 📄 Claude's README.md Format (Claude README 형식) - Project Root
```markdown
# [프로젝트명]
## Description (설명)
[프로젝트 한 줄 설명]

## Build Instructions (빌드 방법)
[설치 및 빌드 명령어]

## Usage (사용법)
[기본 사용법]

## Dependencies (의존성)
[필요한 라이브러리/도구]
```

### 📝 Session Log Format (세션 로그 형식)
```markdown
# 세션 기록: [작업명]
- 날짜 (Date): YYYY-MM-DD HH:MM:SS
- 참여 AI (Participants): Claude, Gemini 등
- 목표 (Goal): [이 세션의 목표]

## 진행 과정 (Process)
1. [단계별 진행 사항]

## 주요 결정사항 (Key Decisions)
- [중요한 기술적 결정들]

## 결과 (Results)
- [완료된 작업들]
- [미완료/다음 단계]
```


## 🔄 Inter-AI Communication (AI 간 소통 원칙)

### File-Based Communication (파일 기반 소통)
- **원칙**: 모든 작업 지시는 파일 경로를 통해 이루어짐
- **Claude → Gemini 예시**: `echo "이 아키텍처 설계에 대한 보안상 문제점이 있는지 분석해주세요" | gemini -y`
- **Gemini → Claude 예시**: `claude -p "파일 '.kb/projects/game/spec.md'를 참조하여 게임 로직을 구현해주세요" --dangerously-skip-permissions`

### Project Folder Structure (프로젝트 폴더 구조)
- **규칙**: 모든 개별 프로젝트는 루트 디렉토리에 자신의 이름을 딴 전용 폴더 생성
- **구조**: 모든 관련 파일은 해당 폴더 내에 위치
- **예외**: README.md는 각 프로젝트 루트에 위치 (다른 사용자 접근성 우선)

### Efficiency Principle (효율성 원칙)
- **Direct Storage (직접 저장)**: 중간 전달 과정 없이 바로 파일 저장으로 시간 단축
- **File-Based Workflow (파일 기반 워크플로우)**: 모든 AI 간 소통은 파일 경로를 통해 수행

## ✅ Pre-Task Checklist (작업 시작 체크리스트)

복잡하거나 2개 이상의 AI가 협업하는 작업을 시작하기 전, 담당 AI는 아래 항목을 사용자에게 보고해야 한다.

1.  **규칙 확인:** `COLLABORATION-RULES.md`의 최신 내용을 숙지했는가?
2.  **세션 기록:** '복잡한 협업 세션'에 해당하여 `.kb/sessions/`에 로그 디렉토리를 생성했는가?
3.  **목표 명확화:** 사용자의 최종 목표와 완료 조건이 명확한가?

## 🔬 Unit Test Rule (유닛 테스트 규칙)

시스템의 핵심 로직 검증을 위해 유닛 테스트를 도입하며, 모든 AI는 아래의 절차를 따른다.

1.  **PM (Gemini)의 책임:**
    *   **명세 작성:** `spec.md`에 테스트가 필요한 **핵심 로직**(예: 핵심 데이터 처리, 상태 변경 로직, 주요 알고리즘 등)과 **테스트 케이스(정상, 경계, 예외 포함)**를 명확히 정의한다.
    *   **최종 검증:** **(A) 유닛 테스트 실행**과 **(B) 소스 코드 리뷰**를 모두 수행하여, 기능의 정확성과 코드 품질을 함께 검증한다.

2.  **구현 (Claude)의 책임:**
    *   **코드 작성:** `spec.md`에 명시된 기능 코드와 유닛 테스트 코드를 함께 작성한다.
    *   **파일 분리:** 모든 테스트 코드는 `tests/` 디렉토리에, 소스 코드와 분리된 별도 파일로 작성한다.

3.  **빌드 시스템의 요구사항:**
    *   빌드 스크립트(`CMakeLists.txt` 등)는 실제 애플리케이션과 테스트 실행 파일을 **별개의 타겟**으로 빌드해야 한다.

## 🚀 Feature Proposal Rule (기능 추가/변경 제안 규칙)

`spec.md`에 명시되지 않은 기능은 아래 절차를 통해서만 제안하고 구현할 수 있다.

1.  **구현 AI:** 새 기능을 직접 구현하는 대신, `.kb/projects/[프로젝트명]/feature-proposal.md`에 제안서를 작성하고 PM에게 검토를 요청한다.
2.  **PM:** 제안을 검토 후 `승인/보류/거절`을 결정하여 제안서에 기록한다.
3.  **PM (승인 시):** **반드시 `spec.md`를 수정**하여 승인된 기능을 요구사항에 추가한다.
4.  **구현 AI:** `spec.md`가 수정된 것을 확인한 후에만 기능 구현을 시작한다.

## 🚨 Error Handling & Decision Protocol (에러 처리 및 의사결정 프로토콜)

### Implementation Failure Response (구현 실패 시 대응)
1. **실패 AI**: 실패 원인과 상세한 에러 로그를 `.kb/sessions/[세션명]/error-report.md`에 기록
2. **실패 AI**: 상대 AI에게 자문 요청
3. **자문 AI**: 문제 분석 후 해결 방안을 `.kb/sessions/[세션명]/solution-proposal.md`에 작성
4. **실패 AI**: 자문 AI 제안 방안으로 재시도
5. **재시도 실패 시**: 사용자에게 실패 사실과 파일 경로들 보고 후 결정 요청

### Security Issue Response (보안 문제 발생 시)
1. **감지 AI**: 보안 위험 감지 즉시 작업 중단
2. **감지 AI**: 보안 문제 상세 내용을 `.kb/sessions/[세션명]/security-alert.md`에 기록
3. **감지 AI**: 사용자에게 즉시 보안 위험 경고 및 파일 경로 보고
4. **사용자**: 보안 위험 평가 후 조치 결정

### AI Disagreement Resolution (AI 간 의견 불일치 해결)
1. **각 AI**: 자신의 의견과 근거를 `.kb/projects/[프로젝트명]/opinion-[ai명].md`에 기록
2. **양측**: 사용자에게 의견 불일치 상황과 파일 경로들 보고
3. **사용자**: 양측 의견 검토 후 최종 결정
4. **결정된 AI**: 사용자 결정 내용을 `.kb/projects/[프로젝트명]/final-decision.md`에 기록

## 🛡️ Security & Permission Protocol (보안 및 권한 승인 프로토콜)

### File System Access Permissions (파일 시스템 접근 권한)
- **규칙**: 파일 시스템 접근 시 사용자 승인 필요

## 🚫 Prohibited Actions (금지사항)

### Don'ts (하지 말아야 할 것들)
```
❌ .kb에 사용자 문서 작성
❌ 프로젝트 폴더에 AI 소통 기록 작성  
❌ 중복 문서 생성 (같은 내용 여러 곳)
❌ 모든 세션에 대해 기록 생성 (Complex Collaboration 조건 참조)
❌ 파일 수정 시 내용 요약/생략 (항상 완전한 내용으로 업데이트)
```

### Do's (해야 할 것들)
```
✅ AI는 자신의 담당 영역에만 문서 작성
✅ Complex Collaboration 조건 해당 시에만 세션 기록
✅ 사용자가 요청할 때만 고급 문서화
✅ **언어 및 인코딩 (UTF-8 통일)**: 모든 **파일명은 영문**으로, **내용은 한글**로 작성합니다. 모든 파일의 **인코딩은 UTF-8**로 통일합니다.
✅ 파일 수정 시 전체 내용으로 교체
✅ README.md는 각 프로젝트 루트에 위치 (다른 사용자 접근성 우선)
```

## 🎯 Success Criteria (성공 기준)

이 규칙이 성공적으로 작동하는 기준:
1. **명확성 (Clarity)**: 어느 AI가 어떤 문서를 작성할지 즉시 판단 가능
2. **일관성 (Consistency)**: 다른 세션에서도 동일한 구조로 문서 생성
3. **단순성 (Simplicity)**: 사용자가 규칙을 이해하고 예측 가능
4. **실용성 (Practicality)**: AI가 실제로 규칙을 준수할 수 있음
5. **접근성 (Accessibility)**: 다른 사용자가 프로젝트를 쉽게 이해할 수 있음

---

**이 규칙은 모든 AI 협업의 기본 원칙입니다. 작업 시작 전 반드시 확인하세요.**

---

## 🔍 Fact-Based Response Rule (사실 기반 응답 규칙)

모든 AI는 답변의 신뢰도를 보장하기 위해 아래 규칙을 따른다.

1. **추측 금지**: 모르는 부분은 추측하지 말고 모른다고 솔직히 명시
2. **검색 우선**: 확실하지 않은 정보는 반드시 최신 공식 문서와 웹 검색을 통해 최신 내용을 검색하여 확인
3. **사실 기반 개발**: 개발 참고 시에도 검증된 정보만 사용
4. **근거 제시**: 기본 개념이나 원리를 설명할 경우, 그 근거가 되는 원칙을 함께 제시

### 🔬 Required validation protocols in your implementation (구현 시 필수 검증 프로토콜)
코드 구현 작업 시 반드시 다음 문서들을 사전 학습:
- **`.kb/projects/[프로젝트명]/verification-protocol.md`**: 단계별 검증 방법, 확신도 레벨 시스템, 추측 방지 상세 규칙
- **`.kb/projects/[프로젝트명]/implementation-guidelines.md`**: 실행 가이드라인

**학습 시점**: 구현 작업 시작 전 반드시 완료
**중요**: 모든 기술적 답변 시 verification-protocol.md의 검증 절차 필수 준수

## 🔎 Gemini Fact-Verification Rule (Gemini 사실 검증 규칙)

Gemini는 추가적으로 아래 규칙을 따른다.

1.  **외부 정보 우선:** 최신 정보, API 등 사실 확인이 필요한 모든 요청에 대해, 내부 지식보다 `google_web_search`를 우선적으로 사용한다.
2.  **검증 요청 시 검색 필수:** 사용자가 "검색해줘", "확인해줘" 등의 키워드로 요청 시, 반드시 `google_web_search`를 실행한다.