import sys
from collections import Counter

def calculate_frequency(filename):
    try:
        with open(filename, 'rb') as f:
            data = f.read()
        
        if not data:
            print(f"文件 {filename} 为空。")
            return

        total_chars = len(data)
        counter = Counter(data)
        
        print(f"--- 文件: {filename} ---")
        print(f"总字节数: {total_chars}")
        print(f"{'字符(Hex)':<12} {'字符(Char)':<12} {'出现次数':<12} {'频率(%)':<12}")
        print("-" * 50)
        
        # 按出现频率排序
        for char_code, count in counter.most_common(10):
            char_repr = chr(char_code) if 32 <= char_code <= 126 else "N/A"
            percentage = (count / total_chars) * 100
            print(f"0x{char_code:02x}{'':<8} {char_repr:<12} {count:<12} {percentage:.4f}%")
            
        # 特别关注空格 (0x20)
        space_count = counter.get(0x20, 0)
        space_percentage = (space_count / total_chars) * 100
        print("-" * 50)
        print(f"空格 (0x20) 总计: {space_count}, 频率: {space_percentage:.4f}%")

    except FileNotFoundError:
        print(f"错误: 找不到文件 {filename}")
    except Exception as e:
        print(f"错误: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        filename = input("请输入要分析的文件名: ")
    else:
        filename = sys.argv[1]
    
    calculate_frequency(filename)
    input("\n按回车键退出...")
