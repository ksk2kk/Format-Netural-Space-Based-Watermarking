import sys
from collections import Counter

def calculate_frequency(filename):
    try:
        with open(filename, 'rb') as f:
            data = f.read()
        
        if not data:
            print(f"File {filename} is empty.")
            return

        total_chars = len(data)
        counter = Counter(data)
        
        print(f"--- File: {filename} ---")
        print(f"Total bytes: {total_chars}")
        print(f"{'Character (Hex)':<12} {'Character (Char)':<12} {'Number of occurrences':<12} {'Frequency (%)':<12}")
        print("-" * 50)
        
        # Sort by frequency of occurrence
        for char_code, count in counter.most_common(10):
            char_repr = chr(char_code) if 32 <= char_code <= 126 else "N/A"
            percentage = (count / total_chars) * 100
            print(f"0x{char_code:02x}{'':<8} {char_repr:<12} {count:<12} {percentage:.4f}%")
            
        # Pay special attention to spaces (0x20)
        space_count = counter.get(0x20, 0)
        space_percentage = (space_count / total_chars) * 100
        print("-" * 50)
        print(f"Spaces (0x20) Total: {space_count}, Frequency: {space_percentage:.4f}%")

    except FileNotFoundError:
        print(f"Error: File {filename} not found")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        filename = input("Please enter the file name to be analyzed:")
    else:
        filename = sys.argv[1]
    
    calculate_frequency(filename)
    input("\nPress Enter to exit...")
