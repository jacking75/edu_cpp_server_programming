# 애니멀 챗
  
## API Server 
### API List
#### 계정 생성 
- Path: API/CreateAccount
- Request  
	```
	{
		"ID":"dsdsd",
		"PW":"dsdsd",
		"NickName":"dsds"
	}
	```   
ID 최대 길이 16, PW 최대 길이 16, 닉네임 최대 길이: 16    
- Response  
	```
	{
		"Result":"OK",
	}
	```
    
#### 로그인
- Path: API/Login
- Request  
	```
	{
		"ID":"dsdsd",
		"PW":"dsdsd",
	}
	```  
- Response  
	```
	{
		"Result":"OK",
		"AuthToken":3432432423
	}
	```  
AuthToken의 값은 Int64  
  
#### 게임 서버 접속
- Path: API/ConnectGameServer
- Request  
	```
	{
		"ID":"dsdsd",
		"AuthToken":3432432423
	}
	```  
- Response  
	```
	{
		"Result":"OK",
		"IP":"127.0.0.1",
		"Port":11021
	}
	```    
  
  