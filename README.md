## join_server

```bash
./build/join_server <port>
```

Пример сессии (`nc localhost 9000`):

```
INSERT A 0 lean
OK
INSERT A 0 understand
ERR duplicate 0
INTERSECTION
OK
```

Поддерживаемые команды:

- `INSERT table id name`
- `TRUNCATE table`
- `INTERSECTION`
- `SYMMETRIC_DIFFERENCE`

## Пакет

```bash
cd build
cpack -G DEB
sudo dpkg -i join_server-1.0.0-Linux.deb
```
