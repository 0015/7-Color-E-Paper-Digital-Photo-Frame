from datetime import datetime, time, timedelta
from flask import Flask, send_file, jsonify
import os
import random
import subprocess

app = Flask(__name__)

# Folder containing .h files
H_FILES_FOLDER = './h_files'
sent_files = []
all_files = []

# Start monitor.py as a background process
def start_monitor_script():
    subprocess.Popen(['python3', 'monitor.py'])

def load_files():
    global all_files
    current_files = set(all_files + sent_files)  # Avoid duplicates
    new_files = [f for f in os.listdir(H_FILES_FOLDER) if f.endswith('.h') and f not in current_files]
    all_files.extend(new_files)
    random.shuffle(all_files)  # Optional if you want a new order every time

# Route to get the next .h file
@app.route('/get-img-data', methods=['GET'])
def get_img_data():
    global sent_files, all_files

    # Reshuffle and reset sent_files if all files have been sent
    if not all_files:
        load_files()
        sent_files = []
    
    # Get the next file to send
    next_file = all_files.pop()
    sent_files.append(next_file)

    # Send the file
    file_path = os.path.join(H_FILES_FOLDER, next_file)
    return send_file(file_path, as_attachment=True)

# Optional route to get the status
@app.route('/status', methods=['GET'])
def status():
    load_files()
    return jsonify({
        "remaining_files": len(all_files),
        "sent_files": len(sent_files)
    })

@app.route('/wakeup-interval', methods=['GET'])
def wakeup_interval():
    now = datetime.now()
    current_time = now.time()

    # Define time boundaries
    morning_time = time(8, 0)  # 8 AM
    evening_time = time(20, 0)  # 8 PM

    if morning_time <= current_time < evening_time:
        interval = 3600  # 1 hour in seconds
    else:
        # Calculate seconds until the next 8 AM
        next_morning = datetime.combine(now.date() + timedelta(days=1), morning_time)
        interval = int((next_morning - now).total_seconds())

    return jsonify(interval=interval)

# Run the Flask server
if __name__ == '__main__':
    start_monitor_script()  # Start monitor.py when Flask server starts
    app.run(host='0.0.0.0', port=9999)
