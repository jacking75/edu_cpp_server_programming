# Redis Json 사용법

RedisJSON을 사용하기 위해서는 Redis Stack 이미지를 다운로드해서 실행해야 하므로, 이미지를 추가해야 합니다.

## Redis 실행하기
1. **Docker 이미지 다운로드 후 실행**:
   Redis Stack 이미지를 다운로드해야 합니다. 이 `redislabs/rejson:latest` 이미지는 RedisJSON 모듈을 포함하고 있습니다. <br>
   아래 명령어를 통해, `6389 포트`에 `redis-json`의 이름으로 RedisJSON 모듈을 포함한 Redis를 실행시킬 수 있습니다.
   
   ```bash
   docker run -d --name redis-json -p 6389:6379 redislabs/rejson:latest
   ```

2. **Redis CLI로 접근**:
   `docker exec` 명령을 사용하여 컨테이너 내부의 Redis CLI에 접근할 수 있습니다. <br>
   다음 명령을 사용하여 Redis 컨테이너 내에서 CLI로 진입하세요:

    ```bash
    docker exec -it redis-json redis-cli
    ```


## RedisJSON 사용법

[🌐RedisJSON](https://redis.io/docs/latest/develop/data-types/json/)은 JSON 데이터를 Redis에 저장하고 조작할 수 있는 Redis 모듈입니다. <br>
Redis Stack 이미지를 사용하면 JSON 관련 명령어를 사용할 수 있습니다. <br>
RedisJSON 명령어는 `JSON.SET`, `JSON.GET` 등 JSON 데이터를 처리할 수 있는 기능을 제공합니다. <br>

### RedisJSON 기본 명령어

1. **JSON.SET**  
   JSON 형식의 데이터를 Redis에 저장하는 명령어입니다.   
   ```bash
   JSON.SET <key> <path> <json>
   ```
   - `key`: Redis 키
   - `path`: JSON 경로, 기본적으로 `$`를 사용합니다.
   - `json`: 저장할 JSON 데이터
   
   **예시**:
   ```bash
   JSON.SET user:1 $ '{"name": "John", "age": 30, "city": "New York"}'
   ```

2. **JSON.GET**  
   Redis에서 JSON 데이터를 조회하는 명령어입니다.  
   ```bash
   JSON.GET <key> <path>
   ```
   - `key`: Redis 키
   - `path`: 조회할 JSON 경로 (기본적으로 `$` 경로를 사용하여 전체 JSON을 가져옵니다)
   
   **예시**:
   ```bash
   JSON.GET user:1 $
   ```

3. **JSON.ARRAPPEND**  
   JSON 배열에 새로운 값을 추가하는 명령어입니다.  
   ```bash
   JSON.ARRAPPEND <key> <path> <json>
   ```
   - `key`: Redis 키
   - `path`: JSON 경로 (배열의 경로)
   - `json`: 배열에 추가할 값
   
   **예시**:
   ```bash
   JSON.ARRAPPEND user:1 $.addresses '{"city": "Boston"}'
   ```

4. **JSON.DEL**  
   JSON 데이터의 특정 경로 또는 전체 데이터를 삭제하는 명령어입니다.  
   ```bash
   JSON.DEL <key> <path>
   ```
   - `key`: Redis 키
   - `path`: 삭제할 JSON 경로
   
   **예시**:
   ```bash
   JSON.DEL user:1 $.age
   ```

5. **JSON.STRAPPEND**  
   JSON 문자열의 끝에 값을 추가하는 명령어입니다.  
   ```bash
   JSON.STRAPPEND <key> <path> <string>
   ```
   - `key`: Redis 키
   - `path`: JSON 경로
   - `string`: 추가할 문자열
   
   **예시**:
   ```bash
   JSON.STRAPPEND user:1 $.name ' Doe'
   ```

6. **JSON.NUMINCRBY**  
   JSON 숫자 값을 증가시키는 명령어입니다.  
   ```bash
   JSON.NUMINCRBY <key> <path> <number>
   ```
   - `key`: Redis 키
   - `path`: JSON 경로 (숫자가 저장된 경로)
   - `number`: 증가할 값
   
   **예시**:
   ```bash
   JSON.NUMINCRBY user:1 $.age 1
   ```

### RedisJSON 응용 예시

1. **사용자 데이터 추가**:
   ```bash
   JSON.SET user:1001 $ '{"name": "Alice", "age": 25, "hobbies": ["reading", "traveling"]}'
   ```

2. **사용자의 취미 추가**:
   ```bash
   JSON.ARRAPPEND user:1001 $.hobbies '"cooking"'
   ```

3. **사용자 나이 증가**:
   ```bash
   JSON.NUMINCRBY user:1001 $.age 1
   ```

4. **특정 데이터 조회**:
   ```bash
   JSON.GET user:1001 $.name
   ```

5. **사용자 정보 삭제**:
   ```bash
   JSON.DEL user:1001 $
   ```

### RedisJSON 활용 팁

- JSON 구조를 효율적으로 관리할 수 있도록 경로를 사용하여 데이터를 처리할 수 있습니다.
- 데이터를 업데이트할 때 필요한 필드만 변경하고 나머지 데이터는 유지할 수 있습니다.
- JSON 구조를 사용하면 더 복잡한 데이터 저장이 가능하며, 관계형 데이터베이스보다 더 유연하게 데이터를 관리할 수 있습니다.

## RedisJSON을 활용한 Acl 예제
- [📁RedisServerAcl](../RedisServerAcl)

