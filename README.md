### BASFS File System

# DOCUMENTATION

Made by Blagov, Arkhipov, Syslicov

TODO: Docs
<br><br>

## ⚙️ Быстрый старт

```bash
# компиляция (Linux/macOS, требуется gcc)
$ gcc -std=c11 -Wall -Wextra -o basfs main.c filesystem.c

# запуск
$ ./basfs
BASFS loaded. Enter HELP for help.

Enter command
> 
```

▶️ Если файл-диск `disk.filesystem` отсутствует, создаётся пустая ФС.
При завершении работы через команду `EXIT` все изменения автоматически
сохраняются в этот файл.

<br><br>
---
## 📂 Структура репозитория

| Файл/папка         | Назначение                                                        |
|--------------------|-------------------------------------------------------------------|
| `main.c`           | CLI-утилита: парсинг команд, взаимодействие с API файловой системы|
| `filesystem.c`     | Реализация in-memory ФС и загрузка/выгрузка в `disk.filesystem`   |
| `filesystem.h`     | Публичный API (`fs_load`, `fs_select`, …)                         |
| `disk.filesystem`  | Сам «диск»; текстовый контейнер с данными                        |
| `README.md`        | Вы читаете его сейчас                                            |
| `presentation.pptx`| Презентация (архитектура, демо, перспективы)                     |
| `logo.png`         | Логотип проекта                                                  |

---
* Строка, начинающаяся с символа `/`, — **имя файла** (полный путь).
* Все последующие строки вплоть до следующего `/` (или конца файла)
  относятся к содержимому текущего файла и записываются **как есть**.
* Обязательного завершающего `\n` у содержимого быть не должно — при
  сохранении библиотека добавит его сама, чтобы сохранить корректность парсинга.

> Формат максимально прост: это плюсы для читабельности и отладки, но минус –
> нет поддержки символа `\n/` внутри контента, подкаталоги хранятся как часть
> текста пути, отсутствует индексация.

<br><br>
---
| Команда | Описание 
|---------|----------
| `HELP` | Вывести список доступных команд 
| `OPEN <path>` | Показать содержимое файла 
| `ADD <path>` | Создать новый пустой файл 
| `UPDATE <path>` | Перезаписать содержимое файла (многострочный ввод с завершением `.`) 
| `REMOVE <path>` | Удалить файл 
| `LIST` | Показать список всех файлов
| `SAVE` | Сохранить изменения на диск
| `EXIT` | Выйти с сохранением изменений

---
<br><br>

## Пример сеанса


```shell
user@user-V:~/Desktop/BASFS-dev$ ./basfs
BASFS loaded. Enter HELP for help.

Enter command
> help
Commands:
  HELP
  LIST
  OPEN <path>
  ADD <path>
  UPDATE <path>
       (last line must be '.')
  REMOVE <path>
  SAVE
  EXIT

Enter command
> list
File list (1):
  - /abc

Enter command
> open /abc
abc

Enter command
> add /test       
OK

Enter command
> update /test
Enter new content (last line must be '.'):
1234
5678
.
Updated

Enter command
> list
File list (2):
  - /abc
  - /test

Enter command
> remove /abc       
Deleted

Enter command
> open /test
1234
5678

Enter command
> save
Saved

Enter command
> exit

Bye!
```
<br>
---
* **In-memory** хранение: при загрузке весь диск читается в RAM → высокая
  скорость операций, однако размер ограничен доступной памятью.
* Линейный поиск по массиву `FileEntry` — O(_n_). Для крупных архивов можно
  заменить на хеш-таблицу или сбалансированное дерево.
* `fs_load()` и `fs_save()` строго следуют описанному формату;
  заголовков, метаданных или индексов нет.
* Подкаталоги — это просто часть строки пути (`/dir/sub/file`). Физической
  иерархии нет, но команды работают с абсолютным путём, так что проблем не возникает.
* Код придерживается C11, warning-clean при `-Wall -Wextra`.

---
