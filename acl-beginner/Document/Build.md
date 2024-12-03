# Acl 빌드 후 정적 라이브러리로 추가하기

`acl_cpp_vc2022.sln`의 필요한 프로젝트 빌드 후, <br>
lib 파일과 hpp 파일의 경로를 바탕으로 정적 라이브러리로 추가해주면 된다. <br><br>

<br>


## Acl 빌드하기 (window 기준)
[https://github.com/acl-dev/acl](https://github.com/acl-dev/acl)
위 깃허브를 받은 후, `acl_cpp_vc2022.sln` 솔루션에서 필요한 각 프로젝트의 빌드를 진행한다. <br>
이때 빌드 파일은 자신의 프로젝트의 구성(Debug/Release)과 플랫폼(x64/x86)과 일치해야만 한다. <br>
우리의 경우는, **Debug + x64**를 기본으로 하고 있다. (Release setting도 되어있다 )<br>

* lib_acl
* lib_protocol
* lib_acl_cpp
* libfiber
* libfiber_cpp

<br><br>
현재 나의 레포지토리 [📁acl/Debug_lib_Files](../acl/Debug_lib_Files) 에 빌드 파일이 위치하고 있다. <br>
따라서 추가로 빌드 과정을 거치고 싶지 않다면, 해당 위치에 존재하는 .lib 파일들을 그대로 자신의 로컬 [📁acl/Debug 폴더](../acl/Debug)에 옮긴 후 아래 과정을 (경로까지 같게) 똑같이 따라하면 된다. <br>
(해당 문서는 **[Debug]모드** 기준으로 작성되었습니다.)
- 만약 [Release]모드 빌드를 원하실 경우 링커>일반 부분의 경로만 다르게 설정하면 됩니다.



## 정적 라이브러리로 추가하기
솔루션 속성에서 
* 일단 자신이 빌드한 lib 파일의 플랫폼 버전에 맞게 지정 (x64)
  + 반드시 **구성 관리자** 까지 눌러서 확인하기

* C/C++ > 일반 > **추가 포함 디렉터리**에 헤더파일 경로 추가
  + `..\acl\lib_fiber\c\include;..\acl\lib_fiber\cpp\include;..\acl\lib_acl_cpp\include;..\acl\lib_protocol\include;..\acl\lib_acl\include;..\acl\include;%(AdditionalIncludeDirectories)`


* 링커 > 일반 > **추가 라이브러리 디렉터리**에 lib 파일 경로 추가
  + `..\acl\Debug_lib_Files;%(AdditionalLibraryDirectories)`
 
  + 이때 만약 [Release]모드 빌드를 원하실 경우
    * `..\acl\Release_lib_Files;%(AdditionalLibraryDirectories)`

* 링커 > 입력 > **추가 종속성**에 lib파일 이름 추가
  + `lib_protocol.lib;lib_acl.lib;lib_acl_cpp.lib;libfiber.lib;libfiber_cpp.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)`


<br>
이때 나의 경우, 빌드 파일과 플랫폼 모두 Win32(x64)로 진행했다. <br>


헷갈린다면 [📄친절한 정적 라이브러리 추가 방법(사진포함)](./HowToAddCPPStaticLibrary.md) 을 참고하자.

<br>

원문 출처: [BUILD.md](https://github.com/acl-dev/acl/blob/master/BUILD.md)
