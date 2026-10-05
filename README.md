## Запуск

```bash
./build/bulk_server <port> <bulk_size>
```

Пример:

```bash
./build/bulk_server 9000 3
seq 0 9 | nc localhost 9000
```

В консоли появится:

```
bulk: 0, 1, 2
bulk: 3, 4, 5
bulk: 6, 7, 8
bulk: 9
```

и будет создано четыре файла с соответствующим содержимым.

## Пакет

```bash
cd build
cpack -G DEB
sudo dpkg -i bulk_server-1.0.0-Linux.deb
```
