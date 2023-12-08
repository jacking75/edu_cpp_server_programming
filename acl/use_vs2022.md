# Visual Studio 2022 에서 사용하기 
  
## 빌드 하기 
`acl` 디렉토리에 들어가서 `acl_cpp_vc2022.sln` 파일 열어서 빌드한다.  
솔루션에서 `lib_acl`, `lib_acl_cpp` 프로젝트를 각각 빌드한다. 64비트, static 버전만 빌드한다.   
위 2개의 프로젝트 속성에서 `C++ 언어 표준` 항목을 `미리 보기 - 최신 C++ 초안의 기능(/std:c++latest)`로 한다.   
빌드가 무사히 되면 `acl\x64`에 라이브러리가 만들어져 있다.    
`acl\x64` 에 있는 Debug, Release 디렉토리를(안에 빌드된 lib 파일이 있다) `acl\lib\win64` 디렉토리에 복사한다.    
  
  
## 샘플 프로그램  
  
### socket_stream  
acl 에서 제공하는 샘플 프로그램을 `samples\socket_stream` 디렉토리에 옮긴 것이다.  
  
빌드를 위한 프로젝트 속성
- `C++ 언어 표준` 항목을 `미리 보기 - 최신 C++ 초안의 기능(/std:c++latest)`로 한다.   
- C/C++ - 일반 - 추가 포함 디렉토리 
    - `..\..\acl\include;..\..\acl\lib_acl\include;..\..\acl\lib_acl_cpp\include` 를 추가한다
- 링커 - 일반 - 추가 라이브러리 디렉토리 
    - `..\..\acl\lib\win64\Debug;..\..\..\dist\lib\win64\Debug` 를 추가한다.  
- 링커 - 입력 - 추가 종속성
    - `ws2_32.lib;lib_acl.lib;lib_acl_cpp.lib` 을 추가한다.  
	
위처럼 하였다면 빌드가 잘 될 것이다.   
  
  