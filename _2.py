import socket
from ctypes import *
import argparse
import sys
import time
import os
import struct

import socket
import select
# Platform-specific imports
if os.name == 'nt':  # Windows
    import msvcrt
else:  # Unix (Linux, macOS)
    import termios
    import tty
    import select
from .common import *    

# --- Cross-platform keypress detection ---
def is_key_pressed():
    if os.name == 'nt':
        return msvcrt.kbhit()
    else:
        dr, _, _ = select.select([sys.stdin], [], [], 0)
        return bool(dr)

def read_key():
    if os.name == 'nt':
        return msvcrt.getch().decode()
    else:
        return sys.stdin.read(1)

def set_input_mode():
    if os.name != 'nt':
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        tty.setcbreak(fd)
        return fd, old_settings
    return None, None

def validate_ip(ip_str):
    try:
        ip = ipaddress.ip_address(ip_str)
        return True
    except ValueError:
        return False

def restore_input_mode(fd, old_settings):
    if os.name != 'nt' and fd is not None:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
                    
def save_config(ip,s, y):
                 transmit_packet = additional_packet()
                 transmit_packet.command = 65
                 transmit_packet.length = 512
                 s.sendall(transmit_packet)
                 time.sleep(1/100)
                 received = s.recv(517)
                 file_status = file.from_buffer_copy(received,5)
                 print(f"   {y}. All the Camera controls of {ip} is saved.")
                 byte_array = bytearray(file_status.status)
                 print("           Status: ",str(file_status.status,'UTF-8'))
                 print("")

def read_inference_data(ip,s):
                     #s.connect((ip, 8080))

                     frame_counter = 0
                     print("Press any key to stop reading inference data")
                     stop_loop = True
                     while stop_loop:
                        try:
                              if is_key_pressed():
                                 key = read_key()
                                 print(f"\nKey '{key}' pressed. Stopping.")
                                 break
                        

                              data = s.recv(4005)
                              if not data:
                                 print("No data received. Server may have closed connection.")
                                 break

                              if len(data) < 5 + sizeof(DETECTION):
                                 #print("Incomplete data.")
                                 continue

                              detection = DETECTION.from_buffer_copy(data[5:])
                              print(f"\nFrame: {detection.frame_num}, Detected: {detection.objects.cnt}")
                              
                              frame_counter += 1
                              if frame_counter % 100 == 0:
                                    request_image_path(ip, detection.frame_num)     

                              for i in range(detection.objects.cnt):
                                 obj = detection.objects.obj[i]
                                 print(f"[{i}] x_min={obj.x_min}, x_max={obj.x_max}, y_min={obj.y_min}, "
                                       f"y_max={obj.y_max}, class={obj.npu_class}, score={obj.npu_score}")


                        except Exception as e:
                              print(f"Error: {e}")
                              break

def request_image_path(ip, frame_id):
    print("Entered requesting!")
    print("Requesting frame id : ", frame_id)
    s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("[DEBUG] Socket created.")
    s2.connect((ip, 8080))
    print(f"[DEBUG] Connected to {(ip, 8080)}")

    def flush_socket(sock):
        print("[DEBUG] Flushing socket...")
        sock.setblocking(0)
        try:
            while sock.recv(1024):
                print("[DEBUG] Flushed some data.")
                continue
        except BlockingIOError:
            print("[DEBUG] Socket flush complete (no more data).")
        finally:
            sock.setblocking(1)

    def receive_image(sock):
        print("[DEBUG] Receiving image data...")
        data = b""
        sock.settimeout(2)  # Set timeout to 5 seconds
        try:
            while True:
                chunk = sock.recv(1024)
                print(f"[DEBUG] Received chunk of size {len(chunk)}")
                if not chunk:
                    print("[DEBUG] No more data received.")
                    break
                data += chunk
                if b'\xff\xd9' in data:
                    print("[DEBUG] JPEG end marker found.")
                    break
        except socket.timeout:
            print("[DEBUG] Socket timeout occurred (5 seconds exceeded).")
            return None
        except socket.error as e:
            print(f"[DEBUG] Socket error: {e}")
            return None
        print(f"[DEBUG] Total received data size: {len(data)}")
        return data

    if not str(frame_id).isdigit():
        print("Invalid input: Only numeric frame IDs are allowed.")
        return
    frame_num = int(frame_id)
    if frame_num <= 0:
        print("Underflow: Frame ID must be greater than 0.")
        return
    elif frame_num > 4294967295:
        print("Overflow: Frame ID must be 4294967295 or less.")
        return

    padded_id = str(frame_num).zfill(10)
    print(f"[DEBUG] Padded Frame ID: {padded_id}")
    flush_socket(s2)
    try:
        s2.sendall(padded_id.encode())
        print("[DEBUG] Sent padded frame ID.")
    except (BrokenPipeError, ConnectionResetError):
        print("Error: Connection lost while sending data.")
        return

    image_data = receive_image(s2)
    if image_data is None:
        print("Timeout or socket error during image receive.")
        return
    if b"Frame not found" in image_data:
        print("[DEBUG] Frame not found response received.")
        print(image_data.decode(errors="ignore").strip())
        return
    if len(image_data) < 100 or not image_data.startswith(b'\xff\xd8') or b'\xff\xd9' not in image_data:
        print("[DEBUG] Invalid or incomplete image data received.")
        return

    filename = f"received_{padded_id}.jpeg"
    try:
        with open(filename, "wb") as f:
            f.write(image_data[:image_data.index(b'\xff\xd9') + 2])
        print(f"Image saved as {filename}.")
    except IOError as e:
        print(f"Failed to save image: {e}")


                 
def get_sdk_ver(ip, s, y, flag):
                     #s.connect((ip, 8080))
                     transmit_packet = packet()
                     obj_get_sdk = get_sdk()
                     transmit_packet.command = 23
                     transmit_packet.length = 512
                     s.sendall(transmit_packet)
                     time.sleep(1/100)
                     received = s.recv(517)                      
                     obj_get_sdk = get_sdk.from_buffer_copy(received,5)
                     if flag == 0:
                        print(f"{y}. Camera IP:",ip)
                        print("      SDK version:",bytes(obj_get_sdk.firmware_version).decode())
                        print("")
                     return bytes(obj_get_sdk.firmware_version).decode()          
                     
def get_video(ip, s, y, flag):
                     #s.connect((ip, 8080))
                     transmit_packet = packet()
                     obj_get_video_params =get_video_params()
                     transmit_packet.command = 35
                     transmit_packet.length = 512
                     s.sendall(transmit_packet)
                     time.sleep(1/100)
                     received = s.recv(517)
                     obj_get_video_params =get_video_params.from_buffer_copy(received,5)
                     fmt = bytearray(obj_get_video_params.fmt)
                     if flag==0:
                          print(f"{y}. Camera IP: ",ip)
                          print("      Format: ",str(fmt,'UTF-8'))
                          print("      Width:  ",obj_get_video_params.width)
                          print("      Height: ",obj_get_video_params.height)
                          print("      Port:   ",obj_get_video_params.port)
                          print("      Fps:    ",obj_get_video_params.fps)
                          print("")
                     #s.close()          
                     return str(fmt,'UTF-8'), obj_get_video_params.width, obj_get_video_params.height, obj_get_video_params.port, obj_get_video_params.fps                              

def get_dhcp_mode(s):
    transmit_packet = packet()
    transmit_packet.command = 34
    transmit_packet.length = 512

    s.sendall(transmit_packet)
    received = s.recv(517)

    obj_dhcp = dhcp_mode.from_buffer_copy(received[5:5 + sizeof(dhcp_mode)])

    print("\n\t DHCP: ", end="")
    if obj_dhcp.mode == 0:
        print("OFF")
    elif obj_dhcp.mode == 1:
        print("ON")
    elif obj_dhcp.mode == 2:
        print("ON - FALL BACK")
    elif obj_dhcp.mode == 3:
        print("ON - No Timeout")

    print(f"\t ip_address: {obj_dhcp.ip_addr.decode().strip(chr(0))}")
    print(f"\t netmask: {obj_dhcp.netmask.decode().strip(chr(0))}")
    print(f"\t gateway_ip: {obj_dhcp.gateway.decode().strip(chr(0))}")   


def set_dhcp_mode(s, mode, ip, netmask, gateway):
    obj_dhcp = dhcp_mode()
    obj_dhcp.mode = mode

    print("\n\t DHCP: ", end="")
    if obj_dhcp.mode == 0:
        print("OFF")
    elif obj_dhcp.mode == 1:
        print("ON")
    

    

    if mode == 0:  # Static IP
        # Validate IP

        if not validate_ip(ip):
            print("Invalid IP address.")
            return

        # Validate netmask
        if not validate_ip(netmask):
            print("Invalid netmask.")
            return

        # Validate gateway
        if not validate_ip(gateway):
            print("Invalid gateway IP.")
            return
        

        obj_dhcp.ip_addr = ip.encode().ljust(20, b'\0')
        obj_dhcp.netmask = netmask.encode().ljust(20, b'\0')
        obj_dhcp.gateway = gateway.encode().ljust(20, b'\0')
        print("\n\t ip_address: ", ip)
        print("\n\t netmask: ", netmask)
        print("\n\t gateway_ip: ", gateway)

    elif mode in [1, 2]:  # DHCP modes
        obj_dhcp.ip_addr = b'\0' * 20
        obj_dhcp.netmask = b'\0' * 20
        obj_dhcp.gateway = b'\0' * 20
    else:
        print("Invalid mode")
        return

    # Fill the transmit packet
    tx = packet()
    tx.command = 32
    tx.length = 517

    # Fill message with dhcp_mode structure
    raw_dhcp = bytes(obj_dhcp)
    for i in range(len(raw_dhcp)):
        tx.message[i] = raw_dhcp[i]

    # Send it
    s.sendall(bytes(tx))
    print("DHCP mode set successfully.")  




        
def set_video(ip,s,form, wid, hei, frame_rate, por, y):
                         transmit_packet = packet()
                         obj_set_video_params = set_video_params()
                         obj_set_video_params.command = 3
                         obj_set_video_params.length = 512
                         for i in range(0,4):
                            obj_set_video_params.fmt[i] = ord(form[i])   
                         obj_set_video_params.width = int(wid)
                         obj_set_video_params.height = int(hei)
                         obj_set_video_params.fps = int(frame_rate)                           
                         obj_set_video_params.port = int(por)
                         s.sendall(obj_set_video_params)
                         time.sleep(1/100)
                         #time.sleep(1)
                         transmit_packet = packet()
                         obj_get_video_params =get_video_params()
                         transmit_packet.command = 35
                         transmit_packet.length = 512
                         s.sendall(transmit_packet)
                         time.sleep(1/100)
                         received = s.recv(517)
                         obj_get_video_params =get_video_params.from_buffer_copy(received,5)
                         fmt = bytearray(obj_get_video_params.fmt)
                         print(f"{y}. Camera IP: ",ip)
                         print("      Format: ",str(fmt,'UTF-8'))
                         print("      Width:  ",obj_get_video_params.width)
                         print("      Height: ",obj_get_video_params.height)
                         print("      Port:   ",obj_get_video_params.port)
                         print("      Fps:    ",obj_get_video_params.fps)
                         print("")     
