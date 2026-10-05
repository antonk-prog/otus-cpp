## Использование библиотеки

```cpp
#include "async.h"

auto handle = async::connect(5);
async::receive(handle, "cmd1\ncmd2\n", 10);
async::disconnect(handle);
```

## Демонстрация

```bash
./build/async_demo 3 <<'EOF'
1
2
3
{
a
b
}
c
EOF
```
