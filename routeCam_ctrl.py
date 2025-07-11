def request_image_path(ip, s):
    print("Entered requesting!")

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
            try:
                s.sendall(padded_id.encode())
            except (BrokenPipeError, ConnectionResetError):
                print("Error: Connection lost while sending data.")
                break

            image_data = b""
            try:
                while True:
                    chunk = s.recv(1024)
                    if not chunk:
                        break
                    image_data += chunk
                    if len(chunk) < 1024:
                        break
            except socket.timeout:
                print("Timeout while waiting for data.")
                continue
            except socket.error as e:
                print(f"Socket error while receiving data: {e}")
                continue

            # Check for "Frame not found"
            if b"Frame not found" in image_data:
                print(image_data.decode().strip())
                continue

            # Validate size and basic JPEG header
            if len(image_data) < 100 or not image_data.startswith(b'\xff\xd8'):
                print("Invalid or no image data received.")
                continue

            filename = f"received_{padded_id}.jpeg"
            try:
                with open(filename, "wb") as f:
                    f.write(image_data)
                print(f"Image saved as {filename}.")
            except IOError as e:
                print(f"Failed to save image: {e}")

    except socket.error as e:
        print(f"Socket error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")


