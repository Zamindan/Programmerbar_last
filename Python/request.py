import requests
import time
import csv

ESP32_IP = "http://<ESP32_IP_ADDRESS>"
DATA_ENDPOINT = "/data"
INTERVAL = 1  # seconds
DATA_FILE = "measurements.csv"

def get_measurement_data():
    try:
        response = requests.get(ESP32_IP + DATA_ENDPOINT)
        response.raise_for_status()
        return response.json()
    except requests.exceptions.RequestException as e:
        print(f"Error fetching data: {e}")
        return None

def main():
    with open(DATA_FILE, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Timestamp", "Current", "Voltage", "Power", "Temperature"])

        while True:
            data = get_measurement_data()
            if data:
                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
                writer.writerow([timestamp, data["current"], data["voltage"], data["power"], data["temperature"]])
                print(f"Data logged at {timestamp}")
            time.sleep(INTERVAL)

if __name__ == "__main__":
    main()