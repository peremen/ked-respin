# ked

이 소스 트리는 hangul-archive 저장소에 있는
[ked](http://ftp.kaist.ac.kr/hangul/editor/ked/) 편집기를 현대적인 리눅스
환경에서 컴파일되도록 수정한 것입니다. 해당 편집기가 개발되었던 시점에는
아직까지 PC에서 조합형이 활발하게 사용되고 있었으므로, 현대적인 EUC-KR이나
UTF-8로 작성된 한글 문서는 이 편집기에서 열지 못할 수도 있습니다.

## 빌드 방법
Ubuntu 기준 libx11-dev, libxproto-dev 패키지가 필요합니다.

```
mkdir build
cd build
cmake ../
make
./ked
```

## 알려진 문제점
 * 마우스 휠이라는 것이 없었을 때 만들어진 편집기이므로 휠 스크롤을 지원하지
   않습니다.
 * XWayland 환경에서 실행할 때에 커서가 보이지 않을 수도 있습니다.
