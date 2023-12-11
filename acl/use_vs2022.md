# Visual Studio 2022 에서 사용하기 
  
## 빌드 하기 
`acl` 디렉토리에 들어가서 `acl_cpp_vc2022.sln` 파일 열어서 빌드한다.  
솔루션에서 `lib_acl`, `lib_acl_cpp` 프로젝트를 각각 빌드한다. 64비트, static 버전만 빌드한다.   
위 2개의 프로젝트 속성에서 `C++ 언어 표준` 항목을 `미리 보기 - 최신 C++ 초안의 기능(/std:c++latest)`로 한다.   
빌드가 무사히 되면 `acl\x64`에 라이브러리가 만들어져 있다.    
`acl\x64` 에 있는 Debug, Release 디렉토리를(안에 빌드된 lib 파일이 있다) `acl\lib\win64` 디렉토리에 복사한다.    
    
  
