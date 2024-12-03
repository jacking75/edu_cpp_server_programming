# C++ 정적 라이브러리 추가하기

## 자신이 추가할 정적 라이브러리 빌드하기
* 오픈소스 확인 후 빌드하여 `*.lib`, `/include.*.h` 해당 파일 위치를 확인하자.
* 우리의 경우 아래 페이지를 보고 빌드를 진행하자.
  + 이때 오픈소스 빌드 시 자신의 프로젝트와 일치하는 **"구성(Debug/Release)"** 과 **"플랫폼(x64/x86)"** 으로 설정해야한다.
      * 우리의 경우는, Debug + x64를 기본으로 하고 있다. (Release setting도 되어있다 )
  + [🖱️Acl 오픈소스](https://github.com/acl-dev/acl/tree/master?tab=readme-ov-file)
  + [📄Acl 빌드 번역 및 설명 페이지](./Build.md)

<br><br>

## Visual Studio에서 설정 진행하기

### 1. 프로젝트 속성에 들어가기
![프로젝트속성](./img/CppStaticLib_01.PNG)


<br><br>

### 2. 구성관리자 솔루션 플랫폼 확인하기
#### 자신이 사용할 오픈소스 빌드 파일의 **구성** & **플랫폼 버전**과 같은지 확인하기 
(우리의 프로젝트는 "구성: Debug, Release & 플랫폼: x64"을 지원하고 있다.)
#### 이때 구성관리자 버튼까지 눌러서 확인하기
![플랫폼버전확인-구성관리자](./img/CppStaticLib_x64.PNG)




<br><br>

### 3-1. 추가포함디렉터리에 헤더파일 경로
#### `C/C++ > 일반` 에서 `추가 포함 디렉터리`에 헤더파일(include 폴더) 경로 넣기
![헤더파일경로추가](./img/CppStaticLib_0301.PNG)

 + ex. `..\acl\lib_fiber\c\include;..\acl\lib_fiber\cpp\include;..\acl\lib_acl_cpp\include;..\acl\lib_protocol\include;..\acl\lib_acl\include;..\acl\include;%(AdditionalIncludeDirectories)`

<br>

### 3-2. 추가라이브러리디렉토리 lib 파일 위치
#### `링커 > 일반` 에서 `추가 라이브러리 디렉터리`에 lib 파일 경로 넣기
![정적라이브러리파일경로추가](./img/CppStaticLib_0302.PNG)

 + ex. 구성:Debug `..\acl\Debug_lib_Files;%(AdditionalLibraryDirectories)`
 + ex. 구성:Release `..\acl\Release_lib_Files;%(AdditionalLibraryDirectories)`

<br>

### 3-3. 추가종속성 lib 파일 이름
#### `링커 > 입력` 에서 `추가 종속성`에 lib 파일 이름 넣기
![정적라이브러리파일이름추가](./img/CppStaticLib_0303.PNG)

  + ex. `lib_protocol.lib;lib_acl.lib;lib_acl_cpp.lib;libfiber.lib;libfiber_cpp.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)`

<br><br>


## 오류
### C++ 버전 오류 발생 시 명령줄에 추가하기
* 우리의 경우 lib_fiber 사용 시 Go를 사용한 코루틴 구문에 오류가 생겼다면, C++ 버전 관련 오류이다. (C++ 11 이상부터 지원)
  
#### `C/C++ > 명령줄` 에서 `추가 옵션`에 `/Zc:__cplusplus` 추가하기
![/Zc:__cplusplus명령줄추가](./img/CppStaticLib_04.PNG)





