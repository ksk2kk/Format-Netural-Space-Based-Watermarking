import re
import argparse
import os

def count_eight_spaces(filename):
    """
    Counts the number of occurrences of exactly 8 consecutive spaces in a file.
    This corresponds to the 'Sync Header' logic in the main C++ program,
    where a slot is considered a sync header only if it has exactly 8 spaces.
    """
    if not os.path.exists(filename):
        print(f"Error: File '{filename}' not found.")
        return

    try:
        # Open in binary mode to ensure exact byte processing
        with open(filename, 'rb') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading file: {e}")
        return

    # Find all sequences of spaces (0x20)
    # The regex b' +' finds one or more consecutive spaces
    space_sequences = re.finditer(b' +', content)
    
    count = 0
    total_space_sequences = 0
    
    print(f"Analyzing file: {filename} ...")
    
    for match in space_sequences:
        total_space_sequences += 1
        sequence_len = len(match.group())
        
        # Check if the sequence length is exactly 8
        if sequence_len == 8:
            count += 1
            # Optional: Print offset of the match
            # print(f"Found 8 spaces at offset: {match.start()}")

    print("-" * 30)
    print(f"Total space sequences found: {total_space_sequences}")
    print(f"Occurrences of EXACTLY 8 spaces: {count}")
    print("-" * 30)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Count occurrences of exactly 8 consecutive spaces (Sync Headers).")
    parser.add_argument("filename", nargs='?', default="log.txt", help="Path to the log file (default: log.txt)")
    args = parser.parse_args()
    
    count_eight_spaces(args.filename)
