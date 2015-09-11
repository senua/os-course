# Контейнеры

Вам выданы логины/пароли от контейнеров, для вашего удобства вам не надо ставить линукс на ваши ноуты достаточно зайти в контейнер.

Почитать что такое контейнеры можно тут [OpenVZ containers](http://ru.wikipedia.org/wiki/OpenVZ).

Чтобы в них зайти юзаем информацию от сюда [support](http://support.vdi.mipt.ru)

Чтобы создать нового юзера и сменить пароль, заходим под рутом и делаем:
```
adduser <your desired user name>
usermod -a -G sudo <your desired user name>
#visudo
```
