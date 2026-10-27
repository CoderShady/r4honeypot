import socket
TARGET_IP = "YOUR_ARDUINO_IP"  # Replace with your Uno R4 WiFi's local IP
ports_to_test = [
    (80,   "HTTP-ADMIN-TRAP", b"GET /admin-panel HTTP/1.1\r\nHost: canary\r\nConnection: close\r\n\r\n"),
    (2222, "SSH-BANNER-TRAP", b"SSH-2.0-OpenSSH_Probe\r\n"),
    (23,   "TELNET-IOT-TRAP", b"root\r\n")
]
print("   R4HONEYPOT AUTOMATED TEST SCRIPT                    ")
print(f"[*] Targeting Arduino Canary Node: {TARGET_IP}\n")
for port, service, payload in ports_to_test:
    print(f"[-] Probing {service} (Port {port})...")
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        s.connect((TARGET_IP, port))
        s.sendall(payload)
        
        response = s.recv(1024)
        print(f"    [+] TRIGGERED! Deception banner received:")
        for line in response.decode('utf-8', errors='ignore').splitlines():
            if line.strip():
                print(f"        -> {line.strip()}")
                
        s.close()
    except socket.timeout:
        print(f"    [!] Connection timed out on port {port}.")
    except ConnectionRefusedError:
        print(f"    [!] Connection refused. Verify the IP and ensure the board is running.")
    except Exception as e:
        print(f"    [!] Error: {e}")
        
    print("-" * 55)

print("\n[+] Test complete! Check your Arduino Serial Monitor and LED matrix wave spike.")
