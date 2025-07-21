def request_image_path(ip, s):
    print("Entered requesting!")

    def flush_socket(sock):
        sock.setblocking(0)
        try:
            while sock.recv(1024):
                continue
        except BlockingIOError:
            pass
        finally:
            sock.setblocking(1)

    def receive_image(sock):
        data = b""
        try:
            while True:
                chunk = sock.recv(1024)
                if not chunk:
                    break
                data += chunk
                if b'\xff\xd9' in data:
                    break
        except socket.timeout:
            return None
        except socket.error:
            return None
        return data

    try:
        while True:
            frame_id = input("Enter frame ID (e.g., 1 to 4294967295 or 'exit'): ").strip()
            if frame_id.lower() == "exit":
                break
            if not frame_id.isdigit():
                print("Invalid input: Only numeric frame IDs are allowed.")
                continue
            frame_num = int(frame_id)
            if frame_num <= 0:
                print("Underflow: Frame ID must be greater than 0.")
                continue
            elif frame_num > 4294967295:
                print("Overflow: Frame ID must be 4294967295 or less.")
                continue
            padded_id = str(frame_num).zfill(10)
            flush_socket(s)
            try:
                s.sendall(padded_id.encode())
            except (BrokenPipeError, ConnectionResetError):
                print("Error: Connection lost while sending data.")
                break
            image_data = receive_image(s)
            if image_data is None:
                print("Timeout or socket error during image receive.")
                continue
            if b"Frame not found" in image_data:
                print(image_data.decode(errors="ignore").strip())
                continue
            if len(image_data) < 100 or not image_data.startswith(b'\xff\xd8') or b'\xff\xd9' not in image_data:
                print("Invalid or incomplete image data received.")
                continue
            filename = f"received_{padded_id}.jpeg"
            try:
                with open(filename, "wb") as f:
                    f.write(image_data[:image_data.index(b'\xff\xd9') + 2])
                print(f"Image saved as {filename}.")
            except IOError as e:
                print(f"Failed to save image: {e}")
    except socket.error as e:
        print(f"Socket error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
