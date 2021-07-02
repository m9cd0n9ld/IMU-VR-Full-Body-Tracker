from kivy.config import Config
Config.set('graphics', 'maxfps', '1000')
Config.set('input', 'mouse', 'mouse,disable_multitouch')
Config.set('graphics', 'width', '600')
Config.set('graphics', 'height', '700')
Config.set('graphics', 'minimum_width', '600')
Config.set('graphics', 'minimum_height', '700')
Config.set('kivy','window_icon','icon.ico')
from kivy.app import App
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label
from kivy.uix.spinner import Spinner
from kivy.clock import Clock
import serial
import serial.tools.list_ports
import os
import sys
from configparser import ConfigParser
import threading
import socket
import struct
import binascii
import time
import math

VERSION = "0.2"

ROLE = ['No role',
        'Left leg',
        'Right leg',
        'Left thigh',
        'Right thigh',
        'Waist',
        'Chest']

def resource_path(file):
    try:
        directory = os.path.abspath(sys._MEIPASS)
    except:
        directory = os.path.abspath('.')
    return os.path.join(directory, file)

class RoleList(BoxLayout):
    def __init__(self, mac, ip, role):
        super().__init__()
        self.height = App.get_running_app().root.ids.boxlayout_ref.height/2
        self.size_hint_y = None
        self.spacing = 5
        L1 = Label(size_hint_x=0.25, text=str(mac))
        L2 = Label(size_hint_x=0.25, text=str(ip))
        App.get_running_app().root.ids[mac + '_role_mac'] = L1
        App.get_running_app().root.ids[mac + '_role_ip'] = L2
        S1 = Spinner(size_hint_x=0.5, text=str(role),
                     values=ROLE)
        App.get_running_app().root.ids[mac + '_role_role'] = S1
        S1.bind(text=self.onText)
        self.add_widget(L1)
        self.add_widget(L2)
        self.add_widget(S1)

    def onText(self, spinner, text):
        for k, v in App.get_running_app().root.ids.items():
            if v == spinner:
                mac = k.replace('_role_role', '')
                role = text
                ImuFbtServer.devices_list[mac] = role

class DeviceList(BoxLayout):
    def __init__(self, mac, ip, battery, role):
        super().__init__()
        self.height = App.get_running_app().root.ids.boxlayout_ref.height/2
        self.size_hint_y = None
        self.spacing = 5
        L1 = Label(size_hint_x=0.25, text=str(mac))
        L2 = Label(size_hint_x=0.25, text=str(ip))
        L3 = Label(size_hint_x=0.25, text=str(battery))
        L4 = Label(size_hint_x=0.25, text=str(role))
        App.get_running_app().root.ids[mac + '_list_mac'] = L1
        App.get_running_app().root.ids[mac + '_list_ip'] = L2
        App.get_running_app().root.ids[mac + '_list_battery'] = L3
        App.get_running_app().root.ids[mac + '_list_role'] = L4
        self.add_widget(L1)
        self.add_widget(L2)
        self.add_widget(L3)
        self.add_widget(L4)

class ImuFbtServer(App):
    devices_list = {}
    devices_online = {}

    focused = False
    temp_focus = ''

    def textinput_focus(self, *args):
        instance = args[0][0]
        value = args[0][1]
        if value:
            self.focused = True
            self.temp_focus = instance.text
            Clock.schedule_once(lambda dt: instance.select_all())
        else:
            if instance.text == '':
                instance.text = self.temp_focus
            self.focused = False
            
    def wifi_thread(self):
        self.root.ids.wifi_configure.disabled = True
        try:
            port = self.root.ids.com_port_list.text
            ssid = self.root.ids.wifi_ssid.text
            password = self.root.ids.wifi_password.text
            if len(port) == 0 or len(ssid) == 0 or len(password) == 0:
                self.root.ids.wifi_configure_status.text = 'Invalid settings'
                self.root.ids.wifi_configure.disabled = False
                return
            ser = serial.Serial(port, 115200, timeout=0.1, write_timeout=0.1,
                                xonxoff=False, rtscts=False, dsrdtr=False)
            payload = '111\n{}\n{}\n'.format(ssid, password)
            ser.write(payload.encode())
            start = time.perf_counter()
            while self.wifi_thread_run:
                msg = ser.read(1)
                if len(msg) > 0 and msg[0] == 110:
                    self.root.ids.wifi_configure_status.text = 'WiFi configured'
                    break
                if time.perf_counter() - start >= 3:
                    self.root.ids.wifi_configure_status.text = 'Serial timeout error'
                    break
                time.sleep(0.1)
            ser.close()
        except:
            self.root.ids.wifi_configure_status.text = 'Serial connection error'
        self.root.ids.wifi_configure.disabled = False
            
    def configure_wifi(self):
        self.wifi_thread_run = True
        self.wifi_thread_process = threading.Thread(target=self.wifi_thread, args=())
        self.wifi_thread_process.start()
        
    def com_port_scanner(self, dt):
        com_list = serial.tools.list_ports.comports()
        port_list = []
        for port in com_list:
            port_list.append(port[0])
        if self.root.ids.com_port_list.text not in port_list:
            self.root.ids.com_port_list.text = ''
        self.root.ids.com_port_list.values = port_list
        
    def calibrate_imu(self):
        payload = struct.pack('<B', 51)
        if self.driverPort != 0:
            self.sock_listen.sendto(self.wrap_payload(payload), ('127.0.0.1', self.driverPort))
        
    def check_payload(self, payload):
        if payload[0] == ord('I') and payload[-1] == ord('i'):
            return True
        else:
            return False
        
    def wrap_payload(self, payload):
        header = b'I'
        footer = b'i'
        return header + payload + footer
    
    def unwrap_payload(self, payload):
        return payload[1:-1]
        
    def udp(self):
        self.sock_listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        serverPort = 6969
        self.driverPort = 0
        self.sock_listen.bind(('0.0.0.0', serverPort))
        self.sock_listen.settimeout(0.1)
        t_tx_driver = time.perf_counter()
        while self.thread_run:
            try:
                payload, addr = self.sock_listen.recvfrom(32)
                if self.check_payload(payload):
                    payload = self.unwrap_payload(payload)
                    if len(payload) == 1:
                        if payload[0] == 77:
                            ip = addr[0]
                            reply = struct.pack('<BBH', 200, 0, self.driverPort)
                            self.sock_listen.sendto(self.wrap_payload(reply), (ip, serverPort))
                    elif len(payload) == 3:
                        if payload[0] == 87:
                            _, self.driverPort = struct.unpack('<BH', payload)
                            if not self.focused:
                                self.set_driver_settings()
                            t_tx_driver = time.perf_counter()
                    elif len(payload) == 11:
                        mac = binascii.hexlify(payload[1:7]).decode()
                        ip = addr[0]
                        battery = struct.unpack('<f', payload[7:])[0]
                        battery = '{:.2f}'.format(battery)
                        role = ROLE[payload[0]]
                        self.devices_online[mac] = time.perf_counter()
                        if mac + '_list' not in self.root.ids.keys():
                            DeviceList_widget = DeviceList(mac, ip, battery, role)
                            self.root.ids.stack_list.add_widget(DeviceList_widget)
                            self.root.ids[mac + '_list'] = DeviceList_widget
                        else:
                            self.root.ids[mac + '_list_mac'].text = str(mac)
                            self.root.ids[mac + '_list_ip'].text = str(ip)
                            self.root.ids[mac + '_list_battery'].text = str(battery)
                            self.root.ids[mac + '_list_role'].text = str(role)
                        if mac + '_role' not in self.root.ids.keys():
                            if mac in self.devices_list.keys():
                                role = self.devices_list[mac]
                            else:
                                role = ROLE[0]
                                self.devices_list[mac] = role
                            RoleList_widget = RoleList(mac, ip, role)
                            self.root.ids.stack_role.add_widget(RoleList_widget)
                            self.root.ids[mac + '_role'] = RoleList_widget
                        else:
                            self.root.ids[mac + '_role_mac'].text = str(mac)
                            self.root.ids[mac + '_role_ip'].text = str(ip)
                        reply = struct.pack('<BBH', 200, ROLE.index(self.devices_list[mac]), self.driverPort)
                        self.sock_listen.sendto(self.wrap_payload(reply), (ip, serverPort))                        
            except socket.timeout:
                pass
            if time.perf_counter() - t_tx_driver >= 3:
                self.driverPort = 0
            to_del = []
            for k, v in self.devices_online.items():
                if time.perf_counter() - v >= 3:
                    to_del.append(k)
            for k in to_del:
                del self.devices_online[k]
                self.root.ids.stack_list.remove_widget(self.root.ids[k + '_list'])
                self.root.ids.stack_role.remove_widget(self.root.ids[k + '_role'])
                del self.root.ids[k + '_list']
                del self.root.ids[k + '_list_mac']
                del self.root.ids[k + '_list_ip']
                del self.root.ids[k + '_list_battery']
                del self.root.ids[k + '_list_role']
                del self.root.ids[k + '_role']
                del self.root.ids[k + '_role_mac']
                del self.root.ids[k + '_role_ip']
                del self.root.ids[k + '_role_role']
        self.sock_listen.close()

    def set_driver_settings(self):
        T_lshin_drv = [math.radians(float(self.root.ids.lshin_x_text.text)),
                       math.radians(float(self.root.ids.lshin_y_text.text)),
                       math.radians(float(self.root.ids.lshin_z_text.text))]
        T_rshin_drv = [math.radians(float(self.root.ids.rshin_x_text.text)),
                       math.radians(float(self.root.ids.rshin_y_text.text)),
                       math.radians(float(self.root.ids.rshin_z_text.text))]
        T_lthigh_drv = [math.radians(float(self.root.ids.lthigh_x_text.text)),
                       math.radians(float(self.root.ids.lthigh_y_text.text)),
                       math.radians(float(self.root.ids.lthigh_z_text.text))]
        T_rthigh_drv = [math.radians(float(self.root.ids.rthigh_x_text.text)),
                       math.radians(float(self.root.ids.rthigh_y_text.text)),
                       math.radians(float(self.root.ids.rthigh_z_text.text))]
        T_waist_drv = [math.radians(float(self.root.ids.waist_x_text.text)),
                       math.radians(float(self.root.ids.waist_y_text.text)),
                       math.radians(float(self.root.ids.waist_z_text.text))]
        T_chest_drv = [math.radians(float(self.root.ids.chest_x_text.text)),
                       math.radians(float(self.root.ids.chest_y_text.text)),
                       math.radians(float(self.root.ids.chest_z_text.text))]
        shin_h = float(self.root.ids.shin_h.value)
        thigh_h = float(self.root.ids.thigh_h.value)
        back_h = float(self.root.ids.back_h.value)
        head_h = float(self.root.ids.head_h.value)
        hip_width_h = float(self.root.ids.hip_width_h.value)
        shin_sensor_h = float(self.root.ids.shin_sensor_h.value)
        thigh_sensor_h = float(self.root.ids.thigh_sensor_h.value)
        waist_sensor_h = float(self.root.ids.waist_sensor_h.value)
        chest_sensor_h = float(self.root.ids.chest_sensor_h.value)
        chest_enable = self.root.ids.chest_en_check.active
        payload = bytearray(109)
        struct.pack_into('<3f', payload, 0, *T_lshin_drv)
        struct.pack_into('<3f', payload, 12, *T_rshin_drv)
        struct.pack_into('<3f', payload, 24, *T_lthigh_drv)
        struct.pack_into('<3f', payload, 36, *T_rthigh_drv)
        struct.pack_into('<3f', payload, 48, *T_waist_drv)
        struct.pack_into('<3f', payload, 60, *T_chest_drv)
        struct.pack_into('<9f', payload, 72,
                         shin_h,
                         thigh_h,
                         back_h,
                         head_h,
                         hip_width_h,
                         shin_sensor_h,
                         thigh_sensor_h,
                         waist_sensor_h,
                         chest_sensor_h)
        struct.pack_into('<?', payload, 108, chest_enable)
        self.sock_listen.sendto(self.wrap_payload(payload), ('127.0.0.1', self.driverPort))

    def on_start(self):
        self.settings_bak = os.path.join(os.path.expanduser('~'),
                                'Documents',
                                'imuFBT_settings_bak_{}.ini'.format(VERSION))
        if os.path.isfile(self.settings_bak):
            config = ConfigParser()
            config.read(self.settings_bak)

            self.root.ids.lshin_x_text.text = config.get('lshin', 'x')
            self.root.ids.lshin_y_text.text = config.get('lshin', 'y')
            self.root.ids.lshin_z_text.text = config.get('lshin', 'z')

            self.root.ids.rshin_x_text.text = config.get('rshin', 'x')
            self.root.ids.rshin_y_text.text = config.get('rshin', 'y')
            self.root.ids.rshin_z_text.text = config.get('rshin', 'z')

            self.root.ids.lthigh_x_text.text = config.get('lthigh', 'x')
            self.root.ids.lthigh_y_text.text = config.get('lthigh', 'y')
            self.root.ids.lthigh_z_text.text = config.get('lthigh', 'z')

            self.root.ids.rthigh_x_text.text = config.get('rthigh', 'x')
            self.root.ids.rthigh_y_text.text = config.get('rthigh', 'y')
            self.root.ids.rthigh_z_text.text = config.get('rthigh', 'z')

            self.root.ids.waist_x_text.text = config.get('waist', 'x')
            self.root.ids.waist_y_text.text = config.get('waist', 'y')
            self.root.ids.waist_z_text.text = config.get('waist', 'z')
            
            self.root.ids.chest_x_text.text = config.get('chest', 'x')
            self.root.ids.chest_y_text.text = config.get('chest', 'y')
            self.root.ids.chest_z_text.text = config.get('chest', 'z')

            self.root.ids.shin_h.value = float(config.get('parameter', 'shin_h'))
            self.root.ids.thigh_h.value = float(config.get('parameter', 'thigh_h'))
            self.root.ids.back_h.value = float(config.get('parameter', 'back_h'))
            self.root.ids.head_h.value = float(config.get('parameter', 'head_h'))
            self.root.ids.hip_width_h.value = float(config.get('parameter', 'hip_width_h'))
            self.root.ids.shin_sensor_h.value = float(config.get('parameter', 'shin_sensor_h'))
            self.root.ids.thigh_sensor_h.value = float(config.get('parameter', 'thigh_sensor_h'))
            self.root.ids.waist_sensor_h.value = float(config.get('parameter', 'waist_sensor_h'))
            self.root.ids.chest_sensor_h.value = float(config.get('parameter', 'chest_sensor_h'))
            self.root.ids.chest_en_check.active = int(config.get('parameter', 'chest_en_check'))

            for k in config.items('devices'):
                self.devices_list[k[0]] = k[1]

        self.thread_run = True
        self.thread_process = threading.Thread(target=self.udp, args=())
        self.thread_process.start()
        
        Clock.schedule_interval(self.com_port_scanner, 0.5)

        self.title = 'IMU FBT Server'

    def on_stop(self):
        if os.path.isfile(self.settings_bak):
            os.remove(self.settings_bak)
            
        config = ConfigParser()
        config.read(self.settings_bak)

        config.add_section('lshin')
        config.set('lshin', 'x', self.root.ids.lshin_x_text.text)
        config.set('lshin', 'y', self.root.ids.lshin_y_text.text)
        config.set('lshin', 'z', self.root.ids.lshin_z_text.text)

        config.add_section('rshin')
        config.set('rshin', 'x', self.root.ids.rshin_x_text.text)
        config.set('rshin', 'y', self.root.ids.rshin_y_text.text)
        config.set('rshin', 'z', self.root.ids.rshin_z_text.text)

        config.add_section('lthigh')
        config.set('lthigh', 'x', self.root.ids.lthigh_x_text.text)
        config.set('lthigh', 'y', self.root.ids.lthigh_y_text.text)
        config.set('lthigh', 'z', self.root.ids.lthigh_z_text.text)

        config.add_section('rthigh')
        config.set('rthigh', 'x', self.root.ids.rthigh_x_text.text)
        config.set('rthigh', 'y', self.root.ids.rthigh_y_text.text)
        config.set('rthigh', 'z', self.root.ids.rthigh_z_text.text)

        config.add_section('waist')
        config.set('waist', 'x', self.root.ids.waist_x_text.text)
        config.set('waist', 'y', self.root.ids.waist_y_text.text)
        config.set('waist', 'z', self.root.ids.waist_z_text.text)
        
        config.add_section('chest')
        config.set('chest', 'x', self.root.ids.chest_x_text.text)
        config.set('chest', 'y', self.root.ids.chest_y_text.text)
        config.set('chest', 'z', self.root.ids.chest_z_text.text)

        config.add_section('parameter')
        config.set('parameter', 'shin_h', str(self.root.ids.shin_h.value))
        config.set('parameter', 'thigh_h', str(self.root.ids.thigh_h.value))
        config.set('parameter', 'back_h', str(self.root.ids.back_h.value))
        config.set('parameter', 'head_h', str(self.root.ids.head_h.value))
        config.set('parameter', 'hip_width_h', str(self.root.ids.hip_width_h.value))
        config.set('parameter', 'shin_sensor_h', str(self.root.ids.shin_sensor_h.value))
        config.set('parameter', 'thigh_sensor_h', str(self.root.ids.thigh_sensor_h.value))
        config.set('parameter', 'waist_sensor_h', str(self.root.ids.waist_sensor_h.value))
        config.set('parameter', 'chest_sensor_h', str(self.root.ids.chest_sensor_h.value))
        config.set('parameter', 'chest_en_check', str(int(self.root.ids.chest_en_check.active)))

        config.add_section('devices')
        for k, v in self.devices_list.items():
            config.set('devices', k, v)

        sections = ['lshin', 'rshin', 'lthigh', 'rthigh', 'waist', 'chest', 'parameter']
        for section in sections:
            for item in config.items(section):
                if item[1] == '':
                    config.set(section, item[0], '0')

        with open(self.settings_bak, 'w') as f:
            config.write(f)
            
        self.wifi_thread_run = False
        try:
            self.wifi_thread_process.join()
        except:
            pass

        self.thread_run = False
        self.thread_process.join()

    def build(self):                
        return Builder.load_file(resource_path('main.kv'))


if __name__ == '__main__':
    ImuFbtServer().run()
