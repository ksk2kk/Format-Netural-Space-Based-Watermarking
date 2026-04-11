import os
import re
import shutil
import argparse
import time

def simulate_log_rotation(filepath):
    """
    Simulates log rotation:
    1. Backup the current log file to filepath.1 (slicing)
    2. Truncate the original file and append new dummy data (simulating new log start)
    """
    if not os.path.exists(filepath):
        print(f"Error: File {filepath} not found.")
        return

    backup_path = filepath + ".1"
    try:
        # Simulate rotation: move current log to .1
        shutil.copy2(filepath, backup_path)
        print(f"[Rotation] Backed up {filepath} to {backup_path}")
        
        # Simulate new log start (append mode, though effectively new file content)
        with open(filepath, 'a') as f:
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
            f.write(f"\n[{timestamp}] [ROTATED] Log rotation simulated. New entries start here.\n")
        print(f"[Rotation] Appended rotation marker to {filepath}")
        
    except Exception as e:
        print(f"Error during rotation simulation: {e}")

def simulate_normalization(filepath):
    """
    Simulates malicious normalization:
    Replaces multiple spaces with a single space using regex (equivalent to tr -s ' ').
    Saves the result to xxxx_done.log in the same directory.
    """
    if not os.path.exists(filepath):
        print(f"Error: File {filepath} not found.")
        return

    try:
        # Construct output filename: xxxx.log -> xxxx_done.log
        dirname, filename = os.path.split(filepath)
        name, ext = os.path.splitext(filename)
        output_filename = f"{name}_done{ext}"
        output_path = os.path.join(dirname, output_filename)

        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        # Malicious normalization: collapse multiple spaces to one
        # Equivalent to sed 's/ */ /g' or tr -s ' '
        normalized_content = re.sub(r' +', ' ', content)

        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(normalized_content)
            
        print(f"[Normalization] Malicious formatting applied.")
        print(f"[Normalization] Output saved to: {output_path}")
        
    except Exception as e:
        print(f"Error during normalization simulation: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Log Attack Simulation Script")
    parser.add_argument("filepath", help="Path to the target log file")
    parser.add_argument("--mode", choices=['rotate', 'normalize'], required=True, 
                        help="Attack mode: 'rotate' (Simulate Log Rotation) or 'normalize' (Malicious Formatting)")

    args = parser.parse_args()

    if args.mode == 'rotate':
        simulate_log_rotation(args.filepath)
    elif args.mode == 'normalize':
        simulate_normalization(args.filepath)
