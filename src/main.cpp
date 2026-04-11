#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <chrono>
#include <iomanip>
#include <limits>

using namespace std;

// 辅助函数：将字符串转换为二进制位流 (8 bits per char)
vector<int> stringToBits(const string& input) {
    vector<int> bits;
    for (char c : input) {
        bitset<8> b(c);
        // bitset[0]是最低位，bitset[7]是最高位
        // 我们按高位在前的顺序存储，方便阅读和处理
        for (int i = 7; i >= 0; --i) {
            bits.push_back(b[i]);
        }
    }
    return bits;
}

// 辅助函数：将二进制位流还原为字符串
string bitsToString(const vector<int>& bits) {
    string result = "";
    for (size_t i = 0; i + 7 < bits.size(); i += 8) {
        bitset<8> b;
        for (int j = 0; j < 8; ++j) {
            b[7 - j] = bits[i + j];
        }
        result += char(b.to_ulong());
    }
    return result;
}

// 辅助函数：读取整个文件到内存buffer
vector<char> readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file [" << filename << "]" << endl;
        return {};
    }
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        return buffer;
    }
    return {};
}

// 辅助函数：写回文件
void writeFile(const string& filename, const vector<char>& buffer) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot write file " << filename << endl;
        return;
    }
    file.write(buffer.data(), buffer.size());
}

// 辅助函数：统计文件中的可用空格槽位
size_t countSpaceSlots(const vector<char>& content) {
    size_t count = 0;
    for (size_t i = 0; i < content.size(); ++i) {
        if (content[i] == 0x20) {
            count++;
            size_t j = i + 1;
            while (j < content.size() && content[j] == 0x20) {
                j++;
            }
            i = j - 1;
        }
    }
    return count;
}

void doHideWrite() {
    string filename;
    cout << "请输入日志文件名 (默认为 log.txt): ";
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, filename);
    if (!filename.empty() && filename.back() == '\r') filename.pop_back();
    if (filename.empty()) filename = "log.txt";

    vector<char> fileContent = readFile(filename);
    if (fileContent.empty()) return;

    // 自动探测文件大小和容量
    double fileSizeKB = fileContent.size() / 1024.0;
    size_t totalSlots = countSpaceSlots(fileContent);
    size_t maxChars = totalSlots / 8;

    cout << "文件大小: " << fixed << setprecision(2) << fileSizeKB << " KB" << endl;
    cout << "可用空格槽位: " << totalSlots << endl;
    cout << "理论最大隐写字符数: " << maxChars << " (每KB可存约 " << (fileSizeKB > 0 ? maxChars / fileSizeKB : 0) << " 字符)" << endl;

    string secretMsg;
    cout << "请输入要隐写的字符串: ";
    getline(cin, secretMsg);
    if (!secretMsg.empty() && secretMsg.back() == '\r') secretMsg.pop_back();

    auto startTime = chrono::high_resolution_clock::now();

    // 1. 转换隐写信息为 bit 流
    vector<int> bits = stringToBits(secretMsg);
    
    // 2. 计算可重复隐写的次数
    // 每次隐写消耗的槽位 = bits.size() + 1 (终止符)
    size_t slotsPerMsg = bits.size() + 1;
    size_t repeatCount = 0;
    
    if (slotsPerMsg > 0) {
        repeatCount = totalSlots / slotsPerMsg;
    }
    
    if (repeatCount == 0) {
        cout << "警告：文件空间不足，无法完整写入一次信息！将尽可能写入。" << endl;
    } else {
        cout << "文件空间充足，将重复隐写 " << repeatCount << " 次。" << endl;
    }

    // 3. 开始隐写
    // 策略：
    // bit 0 -> 1个空格
    // bit 1 -> 2个空格
    // 结束符 -> 3个空格
    
    vector<char> newContent;
    size_t currentSlotIndex = 0;
    size_t writtenCount = 0; // 记录已完整写入的次数
    
    for (size_t i = 0; i < fileContent.size(); ++i) {
        if (fileContent[i] != 0x20) {
            newContent.push_back(fileContent[i]);
            continue;
        }

        // 找到一个空格槽位
        size_t j = i + 1;
        while (j < fileContent.size() && fileContent[j] == 0x20) {
            j++;
        }
        
        // 决定要写入多少个空格
        int spacesNeeded = 1; // 默认归一化为1个空格

        if (slotsPerMsg > 0) {
            // 计算当前槽位对应消息中的哪个位置
            // 我们不限制 repeatCount，只要有空间就一直写，直到文件结束
            // 这样能保证利用所有槽位
            size_t msgIndex = currentSlotIndex % slotsPerMsg;
            
            if (msgIndex < bits.size()) {
                // 写入数据位
                if (bits[msgIndex] == 0) spacesNeeded = 1;
                else spacesNeeded = 2;
            } else {
                // 写入终止符
                spacesNeeded = 3;
                // 如果这是终止符，说明完成了一次完整写入（仅用于统计）
                // 但我们在循环结束时统计更准确，或者在这里累加
                if ((currentSlotIndex + 1) % slotsPerMsg == 0) {
                    writtenCount++;
                }
            }
            currentSlotIndex++;
        } else {
            // 如果没有信息（比如空字符串），保持默认1个空格
            spacesNeeded = 1;
        }

        for (int k = 0; k < spacesNeeded; ++k) {
            newContent.push_back(0x20);
        }

        i = j - 1; // 跳过原文件中的连续空格
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    // 4. 写回文件
    writeFile(filename, newContent);

    // 5. 统计信息
    cout << "隐写完成！" << endl;
    cout << "用时: " << duration << " ms" << endl;
    cout << "信息长度: " << secretMsg.length() << " 字符" << endl;
    cout << "实际完整隐写次数: " << writtenCount << endl;
    
    // 计算总隐写字符数 (包含重复)
    size_t totalWrittenChars = writtenCount * secretMsg.length();
    // 加上最后一次可能未写完的部分
    size_t remainderSlots = currentSlotIndex % slotsPerMsg;
    if (remainderSlots > 0) {
        // 每8个槽位是一个字符
        totalWrittenChars += remainderSlots / 8;
    }
    
    cout << "总隐写字符数: " << totalWrittenChars << endl;
    double speed = (duration > 0) ? (totalWrittenChars * 1000.0 / duration) : 0;
    cout << "隐写效率: " << fixed << setprecision(2) << speed << " 字符/秒" << endl;
}

void doRestore() {
    string filename;
    cout << "请输入日志文件名 (默认为 log.txt): ";
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, filename);
    if (!filename.empty() && filename.back() == '\r') filename.pop_back();
    if (filename.empty()) filename = "log.txt";

    auto startTime = chrono::high_resolution_clock::now();

    vector<char> fileContent = readFile(filename);
    if (fileContent.empty()) return;

    vector<string> messages;
    vector<int> currentBits;
    bool foundTerminator = false;
    int terminatorCount = 0;

    for (size_t i = 0; i < fileContent.size(); ++i) {
        if (fileContent[i] == 0x20) {
            int count = 1;
            size_t j = i + 1;
            while (j < fileContent.size() && fileContent[j] == 0x20) {
                count++;
                j++;
            }
            
            // 解析槽位
            if (count == 1) {
                currentBits.push_back(0);
            } else if (count == 2) {
                currentBits.push_back(1);
            } else if (count >= 3) {
                // 遇到终止符
                foundTerminator = true;
                terminatorCount++;
                if (!currentBits.empty()) {
                    messages.push_back(bitsToString(currentBits));
                    currentBits.clear();
                }
            }

            i = j - 1;
        }
    }
    
    // 检查最后一段是否还有数据
    if (!currentBits.empty()) {
        messages.push_back(bitsToString(currentBits) + " [Incomplete]");
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    cout << "还原完成！" << endl;
    cout << "检测到终止符次数: " << terminatorCount << endl;
    cout << "还原出的信息片段数量: " << messages.size() << endl;
    
    if (messages.empty()) {
        cout << "未找到有效信息。" << endl;
    } else {
        cout << "第一条完整信息: [" << messages[0] << "]" << endl;
        if (messages.size() > 1) {
             // 检查是否所有信息都相同
             bool allSame = true;
             for(size_t k=1; k<messages.size(); ++k) {
                 // 忽略最后可能的不完整标记
                 string s1 = messages[0];
                 string s2 = messages[k];
                 if (s2.find("[Incomplete]") != string::npos) continue;
                 if (s1 != s2) {
                     allSame = false;
                     break;
                 }
             }
             if (allSame) {
                 cout << "所有完整片段内容一致 (共 " << (currentBits.empty() ? messages.size() : messages.size()-1) << " 次重复)" << endl;
             } else {
                 cout << "注意：还原出的片段内容不完全一致！" << endl;
                 for(size_t k=0; k<messages.size(); ++k) {
                     cout << "片段 " << k+1 << ": " << messages[k] << endl;
                 }
             }
        }
    }

    cout << "用时: " << duration << " ms" << endl;
}

void getUserChoose() {
    cout << "请选择功能:" << endl;
    cout << "1. 隐写日志 (Hide)" << endl;
    cout << "2. 还原日志 (Restore)" << endl;
    cout << "输入选择 (1/2): ";
    
    int choice;
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "无效输入" << endl;
        return;
    }
    // 吃掉回车
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        doHideWrite();
    } else if (choice == 2) {
        doRestore();
    } else {
        cout << "无效输入" << endl;
    }
}

int main() {
    // 设置控制台编码，防止乱码 (Windows)
    system("chcp 65001 > nul");
    
    while(true) {
        getUserChoose();
        cout << endl;
    }
    return 0;
}
