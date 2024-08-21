import numpy as np
import cv2
from sklearn.cluster import KMeans
import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageTk

# Load and preprocess the image
def load_image(filename):
    image = cv2.imread(filename)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    image = cv2.resize(image, (300, 300))  # Resize for display
    return image

# Apply K-Means clustering
def apply_kmeans(image, n_clusters):
    # Convert the image to the HSV color space
    hsv_image = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)

    # Define a mask for the leaf area (you may need to customize this mask)
    lower_green = np.array([35, 50, 50])  # Lower bound for green in HSV
    upper_green = np.array([90, 255, 255])  # Upper bound for green in HSV
    leaf_mask = cv2.inRange(hsv_image, lower_green, upper_green)

    # Apply the mask to the image
    masked_image = cv2.bitwise_and(hsv_image, hsv_image, mask=leaf_mask)

    # Reshape the masked image for clustering
    reshaped_image = masked_image.reshape(-1, 3)

    # Apply K-Means clustering
    kmeans = KMeans(n_clusters=n_clusters)
    kmeans.fit(reshaped_image)

    # Get clustered labels
    segmented_image = kmeans.labels_.reshape(image.shape[:-1])
    return segmented_image

# Simulate disease detection
def detect_disease(segmented_image):
    # In this function, red is disease, green is healthy, black is unwanted
    unique_clusters = np.unique(segmented_image)
    print(unique_clusters)
    if 2 in unique_clusters:
        return "Disease Detected"
    elif 1 in unique_clusters:
        return "No Disease Detected"
    else:
        return "No Disease Detected"

# Create a function to capture an image from the camera
def capture_image():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return
    ret, frame = cap.read()
    if ret:
        cv2.imwrite("captured_image.jpg", frame)
        cap.release()
        return "captured_image.jpg"
    else:
        print("Error: Could not capture image.")
        cap.release()
        return None

# Callback for the "Open" button
def open_image():
    choice = tk.messagebox.askquestion("Image Source", "Capture Image from Camera?")
    if choice == 'yes':
        file_path = capture_image()
    else:
        file_path = filedialog.askopenfilename()

    if file_path:
        image = load_image(file_path)
        segmented_image = apply_kmeans(image, n_clusters=3)

        # Customize the visualization
        visualization = np.zeros_like(image)
        visualization[segmented_image == 2] = [45, 255, 255]  # Yellow for disease in HSV [45, 255, 255]
        visualization[segmented_image == 1] = [250, 255, 255]  # Green for healthy in HSV
        visualization[segmented_image == 0] = [0, 0, 0]      # Black for unwanted

        display_image(visualization)

        disease_result = detect_disease(segmented_image)
        disease_label.config(text=disease_result)

# Display an image in the GUI
def display_image(image):
    img = Image.fromarray(image)
    img = ImageTk.PhotoImage(img)
    label.config(image=img)
    label.image = img

app = tk.Tk()
app.title("Plant Disease Detection App")

# Capture button
capture_button = tk.Button(app, text="Capture Image", command=open_image)
capture_button.pack()

# Open button
open_button = tk.Button(app, text="Open Image", command=open_image)
open_button.pack()

# Image display label
label = tk.Label(app)
label.pack()

# Disease detection label
disease_label = tk.Label(app, text="", font=("Helvetica", 16))
disease_label.pack()

app.mainloop()
