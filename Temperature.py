'''кнопка есть, вывод числа есть. Для примера парсится рандомное число, так же будет
парситься значение температуры с ардуино. Надо теперь с уведомлением разобраться. 
Потом собирать в апк файл и проверять на телефоне. Если получится смотреть на часах, то 
уже делать дизайн и допиливать косяки.'''



from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.button import Button
from kivy.uix.switch import Switch

from plyer import notification 
'''эта же библиотека будет нужна для wifi'''

import requests
from bs4 import BeautifulSoup

from kivy.uix.screenmanager import ScreenManager, Screen


class BackgroundTask(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.background_task = False

    def on_start(self):
        self.background_task = True
        print('фон вкл')

    def on_stop(self):
        self.background_task = False
        print('фон выкл')


class TemperatureApp(App):
    def __init__(self):
        super().__init__()
        self.label = Label(text='Тут температура')

    def btn_pressed(self, *args):
        url = 'https://randstuff.ru/number/'
        response = requests.get(url)
        soup = BeautifulSoup(response.text, 'lxml')
        numbers = soup.find_all('span')

        for numb in numbers:
            try:
                int(numb.text)
                self.label.text = numb.text

                notification.notify(
                    app_name = 'Temperature',
                    message=f'Temp = {numb.text}')
            except:
                pass

    def switch_call(self, inst, value):
        print('the switch', 'is', value)
        if value:
            BackgroundTask.on_start(self)
        else:
            BackgroundTask.on_stop(self)
        

    def build(self):
        box = BoxLayout()

        btn = Button(text='Update')
        btn.bind(on_press=self.btn_pressed)

        sw = Switch()
        sw.bind(active=self.switch_call)

        box.add_widget(sw)
        box.add_widget(btn)
        box.add_widget(self.label)

        return box
    

if __name__ == '__main__':
    TemperatureApp().run()
