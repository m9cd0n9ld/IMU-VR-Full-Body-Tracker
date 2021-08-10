import os
import sys
def resource_path(file):
    try:
        directory = os.path.abspath(sys._MEIPASS)
    except:
        directory = os.path.abspath('.')
    return os.path.join(directory, file)

from kivy.config import Config
Config.set('graphics', 'maxfps', '5000')
Config.set('input', 'mouse', 'mouse,disable_multitouch')
Config.set('graphics', 'width', '700')
Config.set('graphics', 'height', '700')
Config.set('graphics', 'minimum_width', '700')
Config.set('graphics', 'minimum_height', '700')
Config.set('kivy', 'window_icon', resource_path('icon.png'))
from kivy.app import App
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.popup import Popup
from kivy.uix.label import Label
from kivy.uix.spinner import Spinner
from kivy.uix.tabbedpanel import TabbedPanel, TabbedPanelItem
from kivy.uix.image import Image
from kivy.clock import Clock
from kivy.properties import ObjectProperty
from kivy.core.window import Window
import serial
import serial.tools.list_ports
from configparser import ConfigParser
import threading
import socket
import struct
import binascii
import time
import math

VERSION = "0.5"

ROLE = ['No role',
        'Left foot',
        'Right foot',
        'Left lower leg',
        'Right lower leg',
        'Left thigh',
        'Right thigh',
        'Waist',
        'Chest',
        'Left shoulder',
        'Right shoulder',
        'Left upper arm',
        'Right upper arm']

def battPercent(v):
    v = float(v)
    denominator = (1 + (v / 3.7)**80)**0.165
    percent = int(123 - 123 / denominator)
    if percent >= 100:
        percent = 100
    elif percent <= 0:
        percent = 0
    return percent

class TransformInput:
    tx = ObjectProperty()
    ty = ObjectProperty()
    tz = ObjectProperty()

class SegmentSlider:
    slider = ObjectProperty()
    
class OffsetSlider:
    slider1 = ObjectProperty()
    slider2 = ObjectProperty()
    
class OnOffSwitch:
    switch = ObjectProperty()
    
class Pop(FloatLayout):
    def __init__(self):
        super().__init__()
        self.size = (Window.size[1] * 0.8, Window.size[1] * 0.8)
        self.pos_hint = {'center_x': .5, 'center_y': .5}
        panel = TabbedPanel(do_default_tab=False,
                            size=(Window.size[1] * 0.8, Window.size[1] * 0.8),
                            pos_hint={'center_x': .5, 'center_y': .5},
                            tab_width=Window.size[1] * 0.8 / 2.1)
        
        panelOne = TabbedPanelItem(text='T-pose skeleton joint diagram')
        image_path = resource_path('T-pose_skeleton_diagram.png')
        img = Image(source=image_path, pos_hint = {'center_x': .5, 'center_y': .5})
        panelOne.add_widget(img)
        
        panelTwo = TabbedPanelItem(text='Tracker kinematic chain')
        image_path = resource_path('tracker_kinematic_chain.png')
        img = Image(source=image_path, pos_hint = {'center_x': .5, 'center_y': .5})
        panelTwo.add_widget(img)

        panel.add_widget(panelOne)
        panel.add_widget(panelTwo)
        
        self.add_widget(panel)

class RoleList(BoxLayout):
    def __init__(self, mac, ip, role):
        super().__init__()
        self.height = App.get_running_app().root.ids.layout_ref.height / 2.5
        self.size_hint_y = None
        self.spacing = 5
        L1 = Label(size_hint_x=0.3, text=str(mac))
        L2 = Label(size_hint_x=0.3, text=str(ip))
        App.get_running_app().root.ids[mac + '_role_mac'] = L1
        App.get_running_app().root.ids[mac + '_role_ip'] = L2
        S1 = Spinner(size_hint_x=0.4, text=str(role),
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
        self.height = App.get_running_app().root.ids.layout_ref.height / 2.5
        self.size_hint_y = None
        self.spacing = 5
        L1 = Label(size_hint_x=0.25, text=str(mac))
        L2 = Label(size_hint_x=0.25, text=str(ip))
        L3 = Label(size_hint_x=0.15, text=str(battery))
        L4 = Label(size_hint_x=0.15, text=str(battPercent(battery)))
        L5 = Label(size_hint_x=0.2, text=str(role))
        App.get_running_app().root.ids[mac + '_list_mac'] = L1
        App.get_running_app().root.ids[mac + '_list_ip'] = L2
        App.get_running_app().root.ids[mac + '_list_battery'] = L3
        App.get_running_app().root.ids[mac + '_list_percent'] = L4
        App.get_running_app().root.ids[mac + '_list_role'] = L5
        self.add_widget(L1)
        self.add_widget(L2)
        self.add_widget(L3)
        self.add_widget(L4)
        self.add_widget(L5)

class ImuFbtServer(App):
    devices_list = {}
    devices_online = {}

    focused = False
    temp_focus = ''
    
    def show_popup(self):
        show = Pop()
        size = (Window.size[1] * 0.8, Window.size[1] * 0.8)
        popupWindow = Popup(title="Help", content=show, size_hint=(None, None), size=size)
        popupWindow.open()

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
        self.wifi_thread_run = False
            
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
        
    def calibrate(self):
        self.root.ids.calibrate_imu_button.disabled = True
        i = 3
        while self.calibrate_run:
            self.root.ids.calibrate_imu_button.text = 'Calibrating in {}...'.format(i)
            i -= 1
            time.sleep(1)
            if i <= 0:
                payload = struct.pack('<B', 51)
                if self.driverPort != 0:
                    self.sock_listen.sendto(self.wrap_payload(payload), ('127.0.0.1', self.driverPort))
                break
        self.calibrate_run = False
        self.root.ids.calibrate_imu_button.text = 'Calibrate'
        self.root.ids.calibrate_imu_button.disabled = False
        
    def calibrate_imu(self):
        self.calibrate_run = True
        self.calibrate_process = threading.Thread(target=self.calibrate, args=())
        self.calibrate_process.start()
        
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
        self.sock_listen.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock_listen.bind(('0.0.0.0', serverPort))
        self.sock_listen.settimeout(0.1)
        t_tx_driver = time.perf_counter()
        while self.thread_run:
            try:
                payload, addr = self.sock_listen.recvfrom(32)
                if self.check_payload(payload):
                    payload = self.unwrap_payload(payload)
                    if len(payload) == struct.calcsize('<BB'):
                        if payload[0] == 77:
                            extension = payload[1]
                            ip = addr[0]
                            reply = struct.pack('<BBH?', 200, 0, self.driverPort, extension)
                            self.sock_listen.sendto(self.wrap_payload(reply), (ip, serverPort))
                    elif len(payload) == struct.calcsize('<BH'):
                        if payload[0] == 87:
                            _, self.driverPort = struct.unpack('<BH', payload)
                            if not self.focused:
                                self.set_driver_settings()
                            t_tx_driver = time.perf_counter()
                        elif payload[0] == 97:
                            _, self.driverPort = struct.unpack('<BH', payload)
                            self.set_init_settings()
                    elif len(payload) == struct.calcsize('<BBBBBBBfB'):
                        mac = binascii.hexlify(payload[1:7]).decode()
                        ip = addr[0]
                        battery, extension = struct.unpack('<f?', payload[7:])
                        battery = '{:.2f}'.format(battery)
                        role = ROLE[payload[0]]
                        if extension:
                            mac = mac + '_extend'
                        else:
                            mac = mac + '_main'
                        self.devices_online[mac] = time.perf_counter()
                        if mac + '_list' not in self.root.ids.keys():
                            DeviceList_widget = DeviceList(mac, ip, battery, role)
                            self.root.ids.stack_list.add_widget(DeviceList_widget)
                            self.root.ids[mac + '_list'] = DeviceList_widget
                        else:
                            self.root.ids[mac + '_list_mac'].text = str(mac)
                            self.root.ids[mac + '_list_ip'].text = str(ip)
                            self.root.ids[mac + '_list_battery'].text = str(battery)
                            self.root.ids[mac + '_list_percent'].text = str(battPercent(battery))
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
                        reply = struct.pack('<BBH?', 200, ROLE.index(self.devices_list[mac]), self.driverPort, extension)
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
                del self.root.ids[k + '_list_percent']
                del self.root.ids[k + '_list_role']
                del self.root.ids[k + '_role']
                del self.root.ids[k + '_role_mac']
                del self.root.ids[k + '_role_ip']
                del self.root.ids[k + '_role_role']
        self.sock_listen.close()

    def set_driver_settings(self):
        T_lfoot_drv = [math.radians(float(self.root.ids.lfoot_t.tx.text)),
                       math.radians(float(self.root.ids.lfoot_t.ty.text)),
                       math.radians(float(self.root.ids.lfoot_t.tz.text))]
        T_rfoot_drv = [math.radians(float(self.root.ids.rfoot_t.tx.text)),
                       math.radians(float(self.root.ids.rfoot_t.ty.text)),
                       math.radians(float(self.root.ids.rfoot_t.tz.text))]
        T_lshin_drv = [math.radians(float(self.root.ids.lshin_t.tx.text)),
                       math.radians(float(self.root.ids.lshin_t.ty.text)),
                       math.radians(float(self.root.ids.lshin_t.tz.text))]
        T_rshin_drv = [math.radians(float(self.root.ids.rshin_t.tx.text)),
                       math.radians(float(self.root.ids.rshin_t.ty.text)),
                       math.radians(float(self.root.ids.rshin_t.tz.text))]
        T_lthigh_drv = [math.radians(float(self.root.ids.lthigh_t.tx.text)),
                        math.radians(float(self.root.ids.lthigh_t.ty.text)),
                        math.radians(float(self.root.ids.lthigh_t.tz.text))]
        T_rthigh_drv = [math.radians(float(self.root.ids.rthigh_t.tx.text)),
                        math.radians(float(self.root.ids.rthigh_t.ty.text)),
                        math.radians(float(self.root.ids.rthigh_t.tz.text))]
        T_waist_drv = [math.radians(float(self.root.ids.waist_t.tx.text)),
                       math.radians(float(self.root.ids.waist_t.ty.text)),
                       math.radians(float(self.root.ids.waist_t.tz.text))]
        T_chest_drv = [math.radians(float(self.root.ids.chest_t.tx.text)),
                       math.radians(float(self.root.ids.chest_t.ty.text)),
                       math.radians(float(self.root.ids.chest_t.tz.text))]
        T_lshoulder_drv = [math.radians(float(self.root.ids.lshoulder_t.tx.text)),
                           math.radians(float(self.root.ids.lshoulder_t.ty.text)),
                           math.radians(float(self.root.ids.lshoulder_t.tz.text))]
        T_rshoulder_drv = [math.radians(float(self.root.ids.rshoulder_t.tx.text)),
                           math.radians(float(self.root.ids.rshoulder_t.ty.text)),
                           math.radians(float(self.root.ids.rshoulder_t.tz.text))]
        T_lupperarm_drv = [math.radians(float(self.root.ids.lupperarm_t.tx.text)),
                           math.radians(float(self.root.ids.lupperarm_t.ty.text)),
                           math.radians(float(self.root.ids.lupperarm_t.tz.text))]
        T_rupperarm_drv = [math.radians(float(self.root.ids.rupperarm_t.tx.text)),
                           math.radians(float(self.root.ids.rupperarm_t.ty.text)),
                           math.radians(float(self.root.ids.rupperarm_t.tz.text))]
        shin_h = float(self.root.ids.shin_h.slider.value)
        thigh_h = float(self.root.ids.thigh_h.slider.value)
        lback_h = float(self.root.ids.lback_h.slider.value)
        uback_h = float(self.root.ids.uback_h.slider.value)
        head_h = float(self.root.ids.head_h.slider.value)
        shoulder_h = float(self.root.ids.shoulder_h.slider.value)
        hip_width_h = float(self.root.ids.hip_width_h.slider.value)
        shoulder_width_h = float(self.root.ids.shoulder_width_h.slider.value)
        foot_sensor_h = float(self.root.ids.foot_sensor_h.slider.value)
        shin_sensor_h = float(self.root.ids.shin_sensor_h.slider.value)
        thigh_sensor_h = float(self.root.ids.thigh_sensor_h.slider.value)
        waist_sensor_h = float(self.root.ids.waist_sensor_h.slider.value)
        chest_sensor_h = float(self.root.ids.chest_sensor_h.slider.value)
        shoulder_sensor_h = float(self.root.ids.shoulder_sensor_h.slider.value)
        upperarm_sensor_h = float(self.root.ids.upperarm_sensor_h.slider.value)
        floor_offset = float(self.root.ids.floor_offset.slider.value)
        override_feet = self.root.ids.override_feet_en_check.switch.active
        payload = bytearray(209)
        struct.pack_into('<3f', payload, 0, *T_lfoot_drv)
        struct.pack_into('<3f', payload, 12, *T_rfoot_drv)
        struct.pack_into('<3f', payload, 24, *T_lshin_drv)
        struct.pack_into('<3f', payload, 36, *T_rshin_drv)
        struct.pack_into('<3f', payload, 48, *T_lthigh_drv)
        struct.pack_into('<3f', payload, 60, *T_rthigh_drv)
        struct.pack_into('<3f', payload, 72, *T_waist_drv)
        struct.pack_into('<3f', payload, 84, *T_chest_drv)
        struct.pack_into('<3f', payload, 96, *T_lshoulder_drv)
        struct.pack_into('<3f', payload, 108, *T_rshoulder_drv)
        struct.pack_into('<3f', payload, 120, *T_lupperarm_drv)
        struct.pack_into('<3f', payload, 132, *T_rupperarm_drv)
        struct.pack_into('<16f', payload, 144,
                         shin_h,
                         thigh_h,
                         lback_h,
                         uback_h,
                         head_h,
                         shoulder_h,
                         hip_width_h,
                         shoulder_width_h,
                         foot_sensor_h,
                         shin_sensor_h,
                         thigh_sensor_h,
                         waist_sensor_h,
                         chest_sensor_h,
                         shoulder_sensor_h,
                         upperarm_sensor_h,
                         floor_offset)
        struct.pack_into('<?', payload, 208, override_feet)
        self.sock_listen.sendto(self.wrap_payload(payload), ('127.0.0.1', self.driverPort))
        
        lfoot_1 = float(self.root.ids.lfoot_o.slider1.value)
        lfoot_2 = float(self.root.ids.lfoot_o.slider2.value)
        rfoot_1 = float(self.root.ids.rfoot_o.slider1.value)
        rfoot_2 = float(self.root.ids.rfoot_o.slider2.value)
        lshin_1 = float(self.root.ids.lshin_o.slider1.value)
        lshin_2 = float(self.root.ids.lshin_o.slider2.value)
        rshin_1 = float(self.root.ids.rshin_o.slider1.value)
        rshin_2 = float(self.root.ids.rshin_o.slider2.value)
        lthigh_1 = float(self.root.ids.lthigh_o.slider1.value)
        lthigh_2 = float(self.root.ids.lthigh_o.slider2.value)
        rthigh_1 = float(self.root.ids.rthigh_o.slider1.value)
        rthigh_2 = float(self.root.ids.rthigh_o.slider2.value)
        waist_1 = float(self.root.ids.waist_o.slider1.value)
        waist_2 = float(self.root.ids.waist_o.slider2.value)
        chest_1 = float(self.root.ids.chest_o.slider1.value)
        chest_2 = float(self.root.ids.chest_o.slider2.value)
        lshoulder_1 = float(self.root.ids.lshoulder_o.slider1.value)
        lshoulder_2 = float(self.root.ids.lshoulder_o.slider2.value)
        rshoulder_1 = float(self.root.ids.rshoulder_o.slider1.value)
        rshoulder_2 = float(self.root.ids.rshoulder_o.slider2.value)
        lupperarm_1 = float(self.root.ids.lupperarm_o.slider1.value)
        lupperarm_2 = float(self.root.ids.lupperarm_o.slider2.value)
        rupperarm_1 = float(self.root.ids.rupperarm_o.slider1.value)
        rupperarm_2 = float(self.root.ids.rupperarm_o.slider2.value)
        head_1 = float(self.root.ids.head_o.slider1.value)
        head_2 = float(self.root.ids.head_o.slider2.value)
        payload = bytearray(104)
        struct.pack_into('<2f', payload, 0, lfoot_1, lfoot_2)
        struct.pack_into('<2f', payload, 8, rfoot_1, rfoot_2)
        struct.pack_into('<2f', payload, 16, lshin_1, lshin_2)
        struct.pack_into('<2f', payload, 24, rshin_1, rshin_2)
        struct.pack_into('<2f', payload, 32, lthigh_1, lthigh_2)
        struct.pack_into('<2f', payload, 40, rthigh_1, rthigh_2)
        struct.pack_into('<2f', payload, 48, waist_1, waist_2)
        struct.pack_into('<2f', payload, 56, chest_1, chest_2)
        struct.pack_into('<2f', payload, 64, lshoulder_1, lshoulder_2)
        struct.pack_into('<2f', payload, 72, rshoulder_1, rshoulder_2)
        struct.pack_into('<2f', payload, 80, lupperarm_1, lupperarm_2)
        struct.pack_into('<2f', payload, 88, rupperarm_1, rupperarm_2)
        struct.pack_into('<2f', payload, 96, head_1, head_2)
        self.sock_listen.sendto(self.wrap_payload(payload), ('127.0.0.1', self.driverPort))
        
    def set_init_settings(self):
        feet_enable = self.root.ids.feet_en_check.switch.active
        shin_enable = self.root.ids.shin_en_check.switch.active
        thigh_enable = self.root.ids.thigh_en_check.switch.active
        waist_enable = self.root.ids.waist_en_check.switch.active
        chest_enable = self.root.ids.chest_en_check.switch.active
        shoulder_enable = self.root.ids.shoulder_en_check.switch.active
        upperarm_enable = self.root.ids.upperarm_en_check.switch.active
        payload = struct.pack('<7B', 
                              feet_enable,
                              shin_enable,
                              thigh_enable,
                              waist_enable,
                              chest_enable,
                              shoulder_enable,
                              upperarm_enable)
        self.sock_listen.sendto(self.wrap_payload(payload), ('127.0.0.1', self.driverPort))

    def on_start(self):
        self.title = 'IMU FBT Server'
        
        self.settings_bak = os.path.join(os.path.expanduser('~'),
                                'Documents',
                                'imuFBT_settings_bak_{}.ini'.format(VERSION))
        if os.path.isfile(self.settings_bak):
            config = ConfigParser()
            config.read(self.settings_bak)
            
            self.root.ids.lfoot_t.tx.text = config.get('lfoot', 'x')
            self.root.ids.lfoot_t.ty.text = config.get('lfoot', 'y')
            self.root.ids.lfoot_t.tz.text = config.get('lfoot', 'z')

            self.root.ids.rfoot_t.tx.text = config.get('rfoot', 'x')
            self.root.ids.rfoot_t.ty.text = config.get('rfoot', 'y')
            self.root.ids.rfoot_t.tz.text = config.get('rfoot', 'z')

            self.root.ids.lshin_t.tx.text = config.get('lshin', 'x')
            self.root.ids.lshin_t.ty.text = config.get('lshin', 'y')
            self.root.ids.lshin_t.tz.text = config.get('lshin', 'z')

            self.root.ids.rshin_t.tx.text = config.get('rshin', 'x')
            self.root.ids.rshin_t.ty.text = config.get('rshin', 'y')
            self.root.ids.rshin_t.tz.text = config.get('rshin', 'z')

            self.root.ids.lthigh_t.tx.text = config.get('lthigh', 'x')
            self.root.ids.lthigh_t.ty.text = config.get('lthigh', 'y')
            self.root.ids.lthigh_t.tz.text = config.get('lthigh', 'z')

            self.root.ids.rthigh_t.tx.text = config.get('rthigh', 'x')
            self.root.ids.rthigh_t.ty.text = config.get('rthigh', 'y')
            self.root.ids.rthigh_t.tz.text = config.get('rthigh', 'z')

            self.root.ids.waist_t.tx.text = config.get('waist', 'x')
            self.root.ids.waist_t.ty.text = config.get('waist', 'y')
            self.root.ids.waist_t.tz.text = config.get('waist', 'z')
            
            self.root.ids.chest_t.tx.text = config.get('chest', 'x')
            self.root.ids.chest_t.ty.text = config.get('chest', 'y')
            self.root.ids.chest_t.tz.text = config.get('chest', 'z')
            
            self.root.ids.lshoulder_t.tx.text = config.get('lshoulder', 'x')
            self.root.ids.lshoulder_t.ty.text = config.get('lshoulder', 'y')
            self.root.ids.lshoulder_t.tz.text = config.get('lshoulder', 'z')
            
            self.root.ids.rshoulder_t.tx.text = config.get('rshoulder', 'x')
            self.root.ids.rshoulder_t.ty.text = config.get('rshoulder', 'y')
            self.root.ids.rshoulder_t.tz.text = config.get('rshoulder', 'z')
            
            self.root.ids.lupperarm_t.tx.text = config.get('lupperarm', 'x')
            self.root.ids.lupperarm_t.ty.text = config.get('lupperarm', 'y')
            self.root.ids.lupperarm_t.tz.text = config.get('lupperarm', 'z')
            
            self.root.ids.rupperarm_t.tx.text = config.get('rupperarm', 'x')
            self.root.ids.rupperarm_t.ty.text = config.get('rupperarm', 'y')
            self.root.ids.rupperarm_t.tz.text = config.get('rupperarm', 'z')

            self.root.ids.shin_h.slider.value = float(config.get('parameter', 'shin_h'))
            self.root.ids.thigh_h.slider.value = float(config.get('parameter', 'thigh_h'))
            self.root.ids.lback_h.slider.value = float(config.get('parameter', 'lback_h'))
            self.root.ids.uback_h.slider.value = float(config.get('parameter', 'uback_h'))
            self.root.ids.head_h.slider.value = float(config.get('parameter', 'head_h'))
            self.root.ids.shoulder_h.slider.value = float(config.get('parameter', 'shoulder_h'))
            self.root.ids.hip_width_h.slider.value = float(config.get('parameter', 'hip_width_h'))
            self.root.ids.shoulder_width_h.slider.value = float(config.get('parameter', 'shoulder_width_h'))
            self.root.ids.foot_sensor_h.slider.value = float(config.get('parameter', 'foot_sensor_h'))
            self.root.ids.shin_sensor_h.slider.value = float(config.get('parameter', 'shin_sensor_h'))
            self.root.ids.thigh_sensor_h.slider.value = float(config.get('parameter', 'thigh_sensor_h'))
            self.root.ids.waist_sensor_h.slider.value = float(config.get('parameter', 'waist_sensor_h'))
            self.root.ids.chest_sensor_h.slider.value = float(config.get('parameter', 'chest_sensor_h'))
            self.root.ids.shoulder_sensor_h.slider.value = float(config.get('parameter', 'shoulder_sensor_h'))
            self.root.ids.upperarm_sensor_h.slider.value = float(config.get('parameter', 'upperarm_sensor_h'))
            self.root.ids.floor_offset.slider.value = float(config.get('parameter', 'floor_offset'))
            self.root.ids.override_feet_en_check.switch.active = int(config.get('parameter', 'override_feet_en_check'))
            
            self.root.ids.feet_en_check.switch.active = int(config.get('activation', 'feet_en_check'))
            self.root.ids.shin_en_check.switch.active = int(config.get('activation', 'shin_en_check'))
            self.root.ids.thigh_en_check.switch.active = int(config.get('activation', 'thigh_en_check'))
            self.root.ids.waist_en_check.switch.active = int(config.get('activation', 'waist_en_check'))
            self.root.ids.chest_en_check.switch.active = int(config.get('activation', 'chest_en_check'))
            self.root.ids.shoulder_en_check.switch.active = int(config.get('activation', 'shoulder_en_check'))
            self.root.ids.upperarm_en_check.switch.active = int(config.get('activation', 'upperarm_en_check'))
            
            self.root.ids.lfoot_o.slider1.value = float(config.get('offset', 'lfoot_pos_1'))
            self.root.ids.lfoot_o.slider2.value = float(config.get('offset', 'lfoot_pos_2'))
            self.root.ids.rfoot_o.slider1.value = float(config.get('offset', 'rfoot_pos_1'))
            self.root.ids.rfoot_o.slider2.value = float(config.get('offset', 'rfoot_pos_2'))
            self.root.ids.lshin_o.slider1.value = float(config.get('offset', 'lshin_pos_1'))
            self.root.ids.lshin_o.slider2.value = float(config.get('offset', 'lshin_pos_2'))
            self.root.ids.rshin_o.slider1.value = float(config.get('offset', 'rshin_pos_1'))
            self.root.ids.rshin_o.slider2.value = float(config.get('offset', 'rshin_pos_2'))
            self.root.ids.lthigh_o.slider1.value = float(config.get('offset', 'lthigh_pos_1'))
            self.root.ids.lthigh_o.slider2.value = float(config.get('offset', 'lthigh_pos_2'))
            self.root.ids.rthigh_o.slider1.value = float(config.get('offset', 'rthigh_pos_1'))
            self.root.ids.rthigh_o.slider2.value = float(config.get('offset', 'rthigh_pos_2'))
            self.root.ids.waist_o.slider1.value = float(config.get('offset', 'waist_pos_1'))
            self.root.ids.waist_o.slider2.value = float(config.get('offset', 'waist_pos_2'))
            self.root.ids.chest_o.slider1.value = float(config.get('offset', 'chest_pos_1'))
            self.root.ids.chest_o.slider2.value = float(config.get('offset', 'chest_pos_2'))
            self.root.ids.lshoulder_o.slider1.value = float(config.get('offset', 'lshoulder_pos_1'))
            self.root.ids.lshoulder_o.slider2.value = float(config.get('offset', 'lshoulder_pos_2'))
            self.root.ids.rshoulder_o.slider1.value = float(config.get('offset', 'rshoulder_pos_1'))
            self.root.ids.rshoulder_o.slider2.value = float(config.get('offset', 'rshoulder_pos_2'))
            self.root.ids.lupperarm_o.slider1.value = float(config.get('offset', 'lupperarm_pos_1'))
            self.root.ids.lupperarm_o.slider2.value = float(config.get('offset', 'lupperarm_pos_2'))
            self.root.ids.rupperarm_o.slider1.value = float(config.get('offset', 'rupperarm_pos_1'))
            self.root.ids.rupperarm_o.slider2.value = float(config.get('offset', 'rupperarm_pos_2'))
            self.root.ids.head_o.slider1.value = float(config.get('offset', 'head_pos_1'))
            self.root.ids.head_o.slider2.value = float(config.get('offset', 'head_pos_2'))

            for k in config.items('devices'):
                self.devices_list[k[0]] = k[1]

        self.thread_run = True
        self.thread_process = threading.Thread(target=self.udp, args=())
        self.thread_process.start()
        
        Clock.schedule_interval(self.com_port_scanner, 1)

    def on_stop(self):
        if os.path.isfile(self.settings_bak):
            os.remove(self.settings_bak)
            
        config = ConfigParser()
        config.read(self.settings_bak)
        
        config.add_section('lfoot')
        config.set('lfoot', 'x', self.root.ids.lfoot_t.tx.text)
        config.set('lfoot', 'y', self.root.ids.lfoot_t.ty.text)
        config.set('lfoot', 'z', self.root.ids.lfoot_t.tz.text)
        
        config.add_section('rfoot')
        config.set('rfoot', 'x', self.root.ids.rfoot_t.tx.text)
        config.set('rfoot', 'y', self.root.ids.rfoot_t.ty.text)
        config.set('rfoot', 'z', self.root.ids.rfoot_t.tz.text)

        config.add_section('lshin')
        config.set('lshin', 'x', self.root.ids.lshin_t.tx.text)
        config.set('lshin', 'y', self.root.ids.lshin_t.ty.text)
        config.set('lshin', 'z', self.root.ids.lshin_t.tz.text)

        config.add_section('rshin')
        config.set('rshin', 'x', self.root.ids.rshin_t.tx.text)
        config.set('rshin', 'y', self.root.ids.rshin_t.ty.text)
        config.set('rshin', 'z', self.root.ids.rshin_t.tz.text)

        config.add_section('lthigh')
        config.set('lthigh', 'x', self.root.ids.lthigh_t.tx.text)
        config.set('lthigh', 'y', self.root.ids.lthigh_t.ty.text)
        config.set('lthigh', 'z', self.root.ids.lthigh_t.tz.text)

        config.add_section('rthigh')
        config.set('rthigh', 'x', self.root.ids.rthigh_t.tx.text)
        config.set('rthigh', 'y', self.root.ids.rthigh_t.ty.text)
        config.set('rthigh', 'z', self.root.ids.rthigh_t.tz.text)

        config.add_section('waist')
        config.set('waist', 'x', self.root.ids.waist_t.tx.text)
        config.set('waist', 'y', self.root.ids.waist_t.ty.text)
        config.set('waist', 'z', self.root.ids.waist_t.tz.text)
        
        config.add_section('chest')
        config.set('chest', 'x', self.root.ids.chest_t.tx.text)
        config.set('chest', 'y', self.root.ids.chest_t.ty.text)
        config.set('chest', 'z', self.root.ids.chest_t.tz.text)
        
        config.add_section('lshoulder')
        config.set('lshoulder', 'x', self.root.ids.lshoulder_t.tx.text)
        config.set('lshoulder', 'y', self.root.ids.lshoulder_t.ty.text)
        config.set('lshoulder', 'z', self.root.ids.lshoulder_t.tz.text)
        
        config.add_section('rshoulder')
        config.set('rshoulder', 'x', self.root.ids.rshoulder_t.tx.text)
        config.set('rshoulder', 'y', self.root.ids.rshoulder_t.ty.text)
        config.set('rshoulder', 'z', self.root.ids.rshoulder_t.tz.text)
        
        config.add_section('lupperarm')
        config.set('lupperarm', 'x', self.root.ids.lupperarm_t.tx.text)
        config.set('lupperarm', 'y', self.root.ids.lupperarm_t.ty.text)
        config.set('lupperarm', 'z', self.root.ids.lupperarm_t.tz.text)
        
        config.add_section('rupperarm')
        config.set('rupperarm', 'x', self.root.ids.rupperarm_t.tx.text)
        config.set('rupperarm', 'y', self.root.ids.rupperarm_t.ty.text)
        config.set('rupperarm', 'z', self.root.ids.rupperarm_t.tz.text)

        config.add_section('parameter')
        config.set('parameter', 'shin_h', str(self.root.ids.shin_h.slider.value))
        config.set('parameter', 'thigh_h', str(self.root.ids.thigh_h.slider.value))
        config.set('parameter', 'lback_h', str(self.root.ids.lback_h.slider.value))
        config.set('parameter', 'uback_h', str(self.root.ids.uback_h.slider.value))
        config.set('parameter', 'head_h', str(self.root.ids.head_h.slider.value))
        config.set('parameter', 'shoulder_h', str(self.root.ids.shoulder_h.slider.value))
        config.set('parameter', 'hip_width_h', str(self.root.ids.hip_width_h.slider.value))
        config.set('parameter', 'shoulder_width_h', str(self.root.ids.shoulder_width_h.slider.value))
        config.set('parameter', 'foot_sensor_h', str(self.root.ids.foot_sensor_h.slider.value))
        config.set('parameter', 'shin_sensor_h', str(self.root.ids.shin_sensor_h.slider.value))
        config.set('parameter', 'thigh_sensor_h', str(self.root.ids.thigh_sensor_h.slider.value))
        config.set('parameter', 'waist_sensor_h', str(self.root.ids.waist_sensor_h.slider.value))
        config.set('parameter', 'chest_sensor_h', str(self.root.ids.chest_sensor_h.slider.value))
        config.set('parameter', 'shoulder_sensor_h', str(self.root.ids.shoulder_sensor_h.slider.value))
        config.set('parameter', 'upperarm_sensor_h', str(self.root.ids.upperarm_sensor_h.slider.value))
        config.set('parameter', 'floor_offset', str(self.root.ids.floor_offset.slider.value))
        config.set('parameter', 'override_feet_en_check', str(int(self.root.ids.override_feet_en_check.switch.active)))
        
        config.add_section('activation')
        config.set('activation', 'feet_en_check', str(int(self.root.ids.feet_en_check.switch.active)))
        config.set('activation', 'shin_en_check', str(int(self.root.ids.shin_en_check.switch.active)))
        config.set('activation', 'thigh_en_check', str(int(self.root.ids.thigh_en_check.switch.active)))
        config.set('activation', 'waist_en_check', str(int(self.root.ids.waist_en_check.switch.active)))
        config.set('activation', 'chest_en_check', str(int(self.root.ids.chest_en_check.switch.active)))
        config.set('activation', 'shoulder_en_check', str(int(self.root.ids.shoulder_en_check.switch.active)))
        config.set('activation', 'upperarm_en_check', str(int(self.root.ids.upperarm_en_check.switch.active)))

        config.add_section('offset')
        config.set('offset', 'lfoot_pos_1', str(self.root.ids.lfoot_o.slider1.value))
        config.set('offset', 'lfoot_pos_2', str(self.root.ids.lfoot_o.slider2.value))
        config.set('offset', 'rfoot_pos_1', str(self.root.ids.rfoot_o.slider1.value))
        config.set('offset', 'rfoot_pos_2', str(self.root.ids.rfoot_o.slider2.value))
        config.set('offset', 'lshin_pos_1', str(self.root.ids.lshin_o.slider1.value))
        config.set('offset', 'lshin_pos_2', str(self.root.ids.lshin_o.slider2.value))
        config.set('offset', 'rshin_pos_1', str(self.root.ids.rshin_o.slider1.value))
        config.set('offset', 'rshin_pos_2', str(self.root.ids.rshin_o.slider2.value))
        config.set('offset', 'lthigh_pos_1', str(self.root.ids.lthigh_o.slider1.value))
        config.set('offset', 'lthigh_pos_2', str(self.root.ids.lthigh_o.slider2.value))
        config.set('offset', 'rthigh_pos_1', str(self.root.ids.rthigh_o.slider1.value))
        config.set('offset', 'rthigh_pos_2', str(self.root.ids.rthigh_o.slider2.value))
        config.set('offset', 'waist_pos_1', str(self.root.ids.waist_o.slider1.value))
        config.set('offset', 'waist_pos_2', str(self.root.ids.waist_o.slider2.value))
        config.set('offset', 'chest_pos_1', str(self.root.ids.chest_o.slider1.value))
        config.set('offset', 'chest_pos_2', str(self.root.ids.chest_o.slider2.value))
        config.set('offset', 'lshoulder_pos_1', str(self.root.ids.lshoulder_o.slider1.value))
        config.set('offset', 'lshoulder_pos_2', str(self.root.ids.lshoulder_o.slider2.value))
        config.set('offset', 'rshoulder_pos_1', str(self.root.ids.rshoulder_o.slider1.value))
        config.set('offset', 'rshoulder_pos_2', str(self.root.ids.rshoulder_o.slider2.value))
        config.set('offset', 'lupperarm_pos_1', str(self.root.ids.lupperarm_o.slider1.value))
        config.set('offset', 'lupperarm_pos_2', str(self.root.ids.lupperarm_o.slider2.value))
        config.set('offset', 'rupperarm_pos_1', str(self.root.ids.rupperarm_o.slider1.value))
        config.set('offset', 'rupperarm_pos_2', str(self.root.ids.rupperarm_o.slider2.value))
        config.set('offset', 'head_pos_1', str(self.root.ids.head_o.slider1.value))
        config.set('offset', 'head_pos_2', str(self.root.ids.head_o.slider2.value))

        config.add_section('devices')
        for k, v in self.devices_list.items():
            config.set('devices', k, v)

        sections = ['lfoot', 'rfoot', 'lshin', 'rshin', 'lthigh', 'rthigh', 
                    'waist', 'chest', 'lshoulder', 'rshoulder', 'lupperarm', 'rupperarm']
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
        
        self.calibrate_run = False
        try:
            self.calibrate_process.join()
        except:
            pass

        self.thread_run = False
        self.thread_process.join()

    def build(self):                
        return Builder.load_file(resource_path('main.kv'))


if __name__ == '__main__':
    ImuFbtServer().run()
