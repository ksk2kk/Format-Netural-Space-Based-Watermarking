import math
import sys
from collections import Counter

def calculate_entropy(filename):
    try:
        with open(filename, 'rb') as f:
            data = f.read()
        
        if not data:
            print(f"File {filename} is empty.")
            return 0.0

        total_len = len(data)
        counter = Counter(data)
        
        entropy = 0.0
        
        print(f"--- File: {filename} ---")
        print(f"Total number of bytes: {total_len}")
        
        # Calculate Shannon entropy
        for count in counter.values():
            probability = count / total_len
            if probability > 0:
                entropy -= probability * math.log2(probability)
        
        print(f"Shannon Entropy: {entropy:.8f} bits/byte")
        
        # Theoretical maximum entropy (octet = 8.0)
        max_entropy = 8.0
        ratio = (entropy / max_entropy) * 100
        print(f"Randomness ratio: {ratio:.2f}% (the higher, the closer to complete randomness)")
        
        return entropy

    except FileNotFoundError:
        print(f"Error: File {filename} not found")
        return 0.0
    except Exception as e:
        print(f"Error: {e}")
        return 0.0

if __name__ == "__main__":
    if len(sys.argv) < 2:
        filename = input("Please enter the file name to be analyzed:")
    else:
        filename = sys.argv[1]
    
    calculate_entropy(filename)
    input("\nPress Enter to exit...")
