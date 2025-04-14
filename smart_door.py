from ultralytics import YOLO
import cv2
import serial
import time
import os


# Setup serial communication
ser = serial.Serial('/dev/cu.usbserial-120', 9600, timeout=.1)

# Load model
model = YOLO('yolov8n.pt')

def read_from_arduino():
    data = ser.readline().strip().decode("utf-8")
    return data

while True:
    arduino_data = ""
    if ser.isOpen():
        arduino_data = read_from_arduino()
        if arduino_data == "PIR sensor detects movement":
            start_time = time.time()
            print(arduino_data)
            cap = cv2.VideoCapture(0)

            # Set a folder to save images
            output_folder = "captures"
            if not os.path.exists(output_folder):
                os.makedirs(output_folder)

            while True:
                ret, frame = cap.read()  # Capture frame from webcam

                if not ret:
                    break  # If no frame is read, exit the loop
                
                # Run YOLO inference to detect objects
                results = model(frame)

                # Get the detected objects' bounding boxes
                detections = results[0].boxes

                # Check if a person is detected
                person_detected = False  # Flag to indicate if a person was detected in the current frame
                for box in detections:
                    if box.cls == 0:  # '0' is the class index for 'person'
                        person_detected = True  # Set the flag to true if a person is detected

                # If a person is detected, take a photo and save it
                if person_detected:
                    ser.write("DETECTED".encode())
                    print("Person detected!")

                    timestamp = time.strftime("%Y%m%d-%H%M%S")  # Get the current timestamp
                    cv2.imwrite(f"{output_folder}/capture_{timestamp}.jpg", frame)  # Save the photo
                    print(f"Photo taken and saved as capture_{timestamp}.jpg")
                    break

                else:
                    ser.write("NOT DETECTED".encode())
                    print("No person detected!")
                    time.sleep(1)
                    break
        elif arduino_data != "":
             print(arduino_data) 
