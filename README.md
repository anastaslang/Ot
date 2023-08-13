ChatQt
Client, Server Chat, Qt, QTcpServer, QTcpSocket, SQLite data base, GUI


Итоговый проект по курсу «Профессия C++ разработчик».

Клиент-серверный чат с графическим интерфейсом, с использованием сокетов и хранением информации в базе данных.

Проект выполнен в Qt Creator (Qt Creator 9.0.2, Based on Qt 6.4.2 (GCC 11.2.0, x86_64) и Qt Creator 10.0.0 (Based on Qt 6.4.2 (MSVC 2019, x86_64))), используемая база данных SQLite.

Возможности серверной части:
- Подключение до 10 клиентов;
- Связь с базой данных и получение информации от нее;
- Отслеживание количества подключенных клиентов, авторизованных пользователей, общего количества зарегистрированных пользователей;
- Получение, идентификация и отображение запросов, полученных от подключенных клиентов и авторизованных пользователей;
- Отправка ответных сообщений клиентам;
- Просмотр всех общих сообщений, просмотр приватных сообщений (приватный чат) между двумя пользователями;
- Просмотр списка всех пользователей, пользователей онлайн, пользователей в списке «бан»;
- Возможность отправлять пользователей в бан и разбанивать их, а также отключать пользователя от сервера.

Возможности клиентской части:
 - Подключение к серверу по IP адресу и номеру порта;
 - Отправка запросов серверу, получение и идентификация ответных сообщений от него;
 - Регистрация новых пользователей (при вводе имени, логина и пароля учитываются ограничения на ввод определенных символов, установленные регулярными выражениями) и авторизация уже зарегистрированных пользователей;
 - Получение и отображение общих сообщений для всех, а также информации о том, какой пользователь вошел в чат или вышел из чата;
 - Получение и отображение приватных сообщений, и просмотр чата с выбранным пользователем;
 - Отправка сообщений всем онлайн пользователям чата и конкретному пользователю из списка (при этом показывается онлайн пользователь или оффлайн в данный момент);
 - Информация о том, какие пользователи сейчас онлайн;
 - Выход из чата и закрытие соединения с сервером

Для начала работы с сервером необходимо в файле sqlitedatabase.cpp в конструкторе прописать правильный путь к базе данных _chatdb.db, которая находится в папке ChatServer/db/, например

QString _path_to_database = "D:/ChatQt/ChatServer/db/_chatdb.db";

_db.setDatabaseName(_path_to_database);

В базе данных уже хранятся тестовые пользователи, общие сообщения и приватные сообщения.
Информация об успешности подключения к базе данных отобразится при запуске.
Если подключение и создание (или поиск) таблиц прошло успешно, станет активной кнопка Connect. 

Сначала запускается и подключается сервер, потом уже запускаются и подключаются клиенты.

Для простоты проверки можно залогинить следующих пользователей:

User: Anna		Password: 1234

User: Kate		Password: qwerty123

User: Gamer		Password: qwerty
