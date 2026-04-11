import math
import sys
from collections import Counter

def calculate_entropy(filename):
    try:
        with open(filename, 'rb') as f:
            data = f.read()
        
        if not data:
            print(f"文件 {filename} 为空。")
            return 0.0

        total_len = len(data)
        counter = Counter(data)
        
        entropy = 0.0
        
        print(f"--- 文件: {filename} ---")
        print(f"总字节数: {total_len}")
        
        # 计算香农熵
        for count in counter.values():
            probability = count / total_len
            if probability > 0:
                entropy -= probability * math.log2(probability)
        
        print(f"香农熵 (Shannon Entropy): {entropy:.8f} bits/byte")
        
        # 理论最大熵 (8位字节 = 8.0)
        max_entropy = 8.0
        ratio = (entropy / max_entropy) * 100
        print(f"随机性比率: {ratio:.2f}% (越高越接近完全随机)")
        
        return entropy

    except FileNotFoundError:
        print(f"错误: 找不到文件 {filename}")
        return 0.0
    except Exception as e:
        print(f"错误: {e}")
        return 0.0

if __name__ == "__main__":
    if len(sys.argv) < 2:
        filename = input("请输入要分析的文件名: ")
    else:
        filename = sys.argv[1]
    
    calculate_entropy(filename)
    input("\n按回车键退出...")
