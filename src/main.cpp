#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <chrono>
#include <iomanip>
#include <limits>

using namespace std;

// Helper function: Convert string to binary bit stream (8 bits per char)
vector<int> stringToBits(const string& input) {
    vector<int> bits;
    for (char c : input) {
        bitset<8> b(c);
        // bitset[0] is the lowest bit, bitset[7] is the highest bit
        // We store them in high-order order to facilitate reading and processing.
        for (int i = 7; i >= 0; --i) {
            bits.push_back(b[i]);
        }
    }
    return bits;
}

// Helper function: restore binary bit stream to string
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

// Auxiliary function: read the entire file into the memory buffer
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

// Helper function: write back to file
void writeFile(const string& filename, const vector<char>& buffer) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot write file " << filename << endl;
        return;
    }
    file.write(buffer.data(), buffer.size());
}

// Helper function: Statistics of available space slots in the file
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
    cout << "Please enter a log file name (default is log.txt):";
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, filename);
    if (!filename.empty() && filename.back() == '\r') filename.pop_back();
    if (filename.empty()) filename = "log.txt";

    vector<char> fileContent = readFile(filename);
    if (fileContent.empty()) return;

    // Automatically detect file size and capacity
    double fileSizeKB = fileContent.size() / 1024.0;
    size_t totalSlots = countSpaceSlots(fileContent);
    size_t maxChars = totalSlots / 8;

    cout << "File size:" << fixed << setprecision(2) << fileSizeKB << " KB" << endl;
    cout << "Available space slots:" << totalSlots << endl;
    cout << "Theoretical maximum number of steganographic characters:" << maxChars << "(Each KB can be reserved" << (fileSizeKB > 0 ? maxChars / fileSizeKB : 0) << "character)" << endl;

    string secretMsg;
    cout << "Please enter the string to be hidden:";
    getline(cin, secretMsg);
    if (!secretMsg.empty() && secretMsg.back() == '\r') secretMsg.pop_back();

    auto startTime = chrono::high_resolution_clock::now();

    // 1. Convert steganographic information into bit stream
    vector<int> bits = stringToBits(secretMsg);
    
    // 2. Calculate the number of times steganography can be repeated
    // Slots consumed for each steganography = bits.size() + 1 (terminator)
    size_t slotsPerMsg = bits.size() + 1;
    size_t repeatCount = 0;
    
    if (slotsPerMsg > 0) {
        repeatCount = totalSlots / slotsPerMsg;
    }
    
    if (repeatCount == 0) {
        cout << "Warning: Insufficient file space to completely write information once! Will write as much as possible." << endl;
    } else {
        cout << "The file space is sufficient and duplicate steganography will be performed" << repeatCount << "Second-rate." << endl;
    }

    // 3. Start steganography
    // Strategy:
    // bit 0 -> 1 space
    // bit 1 -> 2 spaces
    // Terminator -> 3 spaces
    
    vector<char> newContent;
    size_t currentSlotIndex = 0;
    size_t writtenCount = 0; // Record the number of complete writes
    
    for (size_t i = 0; i < fileContent.size(); ++i) {
        if (fileContent[i] != 0x20) {
            newContent.push_back(fileContent[i]);
            continue;
        }

        // Find a space slot
        size_t j = i + 1;
        while (j < fileContent.size() && fileContent[j] == 0x20) {
            j++;
        }
        
        // Decide how many spaces to write
        int spacesNeeded = 1; // Normalized to 1 space by default

        if (slotsPerMsg > 0) {
            // Calculate which position in the message the current slot corresponds to
            // We do not limit repeatCount and keep writing as long as there is space until the end of the file.
            // This ensures that all slots are used
            size_t msgIndex = currentSlotIndex % slotsPerMsg;
            
            if (msgIndex < bits.size()) {
                // Write data bits
                if (bits[msgIndex] == 0) spacesNeeded = 1;
                else spacesNeeded = 2;
            } else {
                // write terminator
                spacesNeeded = 3;
                // If this is the terminator, a complete write is completed (for statistics only)
                // But it's more accurate if we count at the end of the loop, or add up here
                if ((currentSlotIndex + 1) % slotsPerMsg == 0) {
                    writtenCount++;
                }
            }
            currentSlotIndex++;
        } else {
            // If there is no information (such as an empty string), keep the default of 1 space
            spacesNeeded = 1;
        }

        for (int k = 0; k < spacesNeeded; ++k) {
            newContent.push_back(0x20);
        }

        i = j - 1; // Skip consecutive spaces in the original file
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    // 4. Write back the file
    writeFile(filename, newContent);

    // 5. Statistics
    cout << "Steganography completed!" << endl;
    cout << "Usage:" << duration << " ms" << endl;
    cout << "Message length:" << secretMsg.length() << "character" << endl;
    cout << "Actual number of complete steganography:" << writtenCount << endl;
    
    // Calculate the total number of stego characters (including repetitions)
    size_t totalWrittenChars = writtenCount * secretMsg.length();
    // Plus the parts that may have been left unfinished the last time
    size_t remainderSlots = currentSlotIndex % slotsPerMsg;
    if (remainderSlots > 0) {
        // Every 8 slots is one character
        totalWrittenChars += remainderSlots / 8;
    }
    
    cout << "Total number of stego characters:" << totalWrittenChars << endl;
    double speed = (duration > 0) ? (totalWrittenChars * 1000.0 / duration) : 0;
    cout << "Steganography efficiency:" << fixed << setprecision(2) << speed << "characters/second" << endl;
}

void doRestore() {
    string filename;
    cout << "Please enter a log file name (default is log.txt):";
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
            
            // parse slot
            if (count == 1) {
                currentBits.push_back(0);
            } else if (count == 2) {
                currentBits.push_back(1);
            } else if (count >= 3) {
                // Terminator encountered
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
    
    // Check if there is still data in the last paragraph
    if (!currentBits.empty()) {
        messages.push_back(bitsToString(currentBits) + " [Incomplete]");
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    cout << "Restore completed!" << endl;
    cout << "Number of terminators detected:" << terminatorCount << endl;
    cout << "Number of information fragments restored:" << messages.size() << endl;
    
    if (messages.empty()) {
        cout << "No valid information found." << endl;
    } else {
        cout << "The first complete message: [" << messages[0] << "]" << endl;
        if (messages.size() > 1) {
             // Check if all information is the same
             bool allSame = true;
             for(size_t k=1; k<messages.size(); ++k) {
                 // Ignore last possible incomplete token
                 string s1 = messages[0];
                 string s2 = messages[k];
                 if (s2.find("[Incomplete]") != string::npos) continue;
                 if (s1 != s2) {
                     allSame = false;
                     break;
                 }
             }
             if (allSame) {
                 cout << "All complete segments have the same content (total" << (currentBits.empty() ? messages.size() : messages.size()-1) << "times repeated)" << endl;
             } else {
                 cout << "Note: The content of the restored fragments is not completely consistent!" << endl;
                 for(size_t k=0; k<messages.size(); ++k) {
                     cout << "fragment" << k+1 << ": " << messages[k] << endl;
                 }
             }
        }
    }

    cout << "Usage:" << duration << " ms" << endl;
}

void getUserChoose() {
    cout << "Please select function:" << endl;
    cout << "1. Stegolog (Hide)" << endl;
    cout << "2. Restore log (Restore)" << endl;
    cout << "Input selection (1/2):";
    
    int choice;
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input" << endl;
        return;
    }
    // Eat Enter
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        doHideWrite();
    } else if (choice == 2) {
        doRestore();
    } else {
        cout << "Invalid input" << endl;
    }
}

int main() {
    // Set console encoding to prevent garbled characters (Windows)
    system("chcp 65001 > nul");
    
    while(true) {
        getUserChoose();
        cout << endl;
    }
    return 0;
}
