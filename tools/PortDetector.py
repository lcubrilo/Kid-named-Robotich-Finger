import serial.tools.list_ports
import json
import os

def select_com_port(available_ports):
    # If no ports are found
    if not available_ports:
        print("Nothing found")
        return

    # If only one port is found
    elif len(available_ports) == 1:
        print(f"Found and wrote the port: {available_ports[0]} to your arduino.json")
        selected_idx = 0

    # If multiple ports are found
    else:
        print("Available COM ports:")
        for idx, port in enumerate(available_ports):
            print(f"{idx + 1}. {port}")
        selected_idx = int(input("Select the COM port where NodeMCU is connected (enter the number): ")) - 1
    
    return selected_idx
 
# Update arduino.json with the selected port
def write_com_port(arduino_port, config_path):
    with open(config_path, 'r+') as file:
        config = json.load(file)
        config["port"] = arduino_port
        file.seek(0)
        json.dump(config, file, indent=4)
        file.truncate()
    

if __name__ == "__main__":
    # Get the list of connected COM ports
    ports = list(serial.tools.list_ports.comports())
    available_ports = [port.device for port in ports]

    selected_id = select_com_port(available_ports)

    if 0 <= selected_id < len(available_ports): # if valid
        arduino_port = available_ports[selected_id]
        config_path = ".vscode/arduino.json"
        write_com_port(arduino_port, config_path)
    else:
        print("Invalid selection.")
