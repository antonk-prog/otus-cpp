## MapReduce

Исходники находятся в `src/`.

Сборка:

```bash
make all
```

Запуск на датасете из `input/`:

```bash
./run_locally.sh
```

Ожидаемый результат для `AB_NYC_2019.csv`:

```
mean
mean	152.72068718682891
variance
variance	57672.845698433593
```

`variance` — генеральная дисперсия (`E[X^2] - E[X]^2`).

Тесты:

```bash
make test
```

Бинарники:

- `bin/mapper_mean`, `bin/reducer_mean`
- `bin/mapper_variance`, `bin/reducer_variance`
