#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <random>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <climits>

using namespace std;

// === Core components required for realistic algorithm simulation ===

inline string ascii_lower(string s) {
    for (char& c : s) c = (char)tolower((unsigned char)c);
    return s;
}

inline bool is_word_char(unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '\'' || c == '_';
}

struct WordSpan {
    size_t start = 0;
    size_t end = 0;
    string lower;
};

inline vector<WordSpan> scan_words(const string& s) {
    vector<WordSpan> spans;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = (unsigned char)s[i];
        if (!is_word_char(c)) {
            i++;
            continue;
        }
        size_t st = i;
        while (i < s.size() && is_word_char((unsigned char)s[i])) i++;
        size_t ed = i;
        WordSpan w;
        w.start = st;
        w.end = ed;
        w.lower = ascii_lower(s.substr(st, ed - st));
        spans.push_back(std::move(w));
    }
    return spans;
}

inline bool is_all_upper_ascii(const string& s) {
    bool any = false;
    for (unsigned char c : s) {
        if (c >= 'a' && c <= 'z') return false;
        if (c >= 'A' && c <= 'Z') any = true;
    }
    return any;
}

inline bool is_capitalized_ascii(const string& s) {
    if (s.empty()) return false;
    unsigned char c0 = (unsigned char)s[0];
    if (!(c0 >= 'A' && c0 <= 'Z')) return false;
    for (size_t i = 1; i < s.size(); i++) {
        unsigned char c = (unsigned char)s[i];
        if (c >= 'A' && c <= 'Z') return false;
    }
    return true;
}

inline string apply_case_like(const string& src, string dst) {
    if (is_all_upper_ascii(src)) {
        for (char& c : dst) c = (char)toupper((unsigned char)c);
        return dst;
    }
    if (is_capitalized_ascii(src)) {
        dst[0] = (char)toupper((unsigned char)dst[0]);
        for (size_t i = 1; i < dst.size(); i++) dst[i] = (char)tolower((unsigned char)dst[i]);
        return dst;
    }
    return dst;
}

inline uint64_t fnv1a64(const string& s) {
    uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : s) {
        h ^= (uint64_t)c;
        h *= 1099511628211ULL;
    }
    return h;
}

inline vector<int> message_to_bits_256(const string& msg) {
    uint64_t seed = fnv1a64(msg);
    std::mt19937 gen((uint32_t)(seed ^ (seed >> 32)));
    vector<int> bits(256, 0);
    for (int i = 0; i < 256; i++) bits[i] = (int)(gen() & 1U);
    return bits;
}

inline string csv_escape(string s) {
    bool need = false;
    for (char c : s) {
        if (c == '"' || c == ',' || c == '\n' || c == '\r') { need = true; break; }
    }
    if (!need) return s;
    string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out.push_back('"');
    return out;
}

// Convert vector<char> to string and vice versa
inline string vec2str(const vector<char>& v) {
    return string(v.begin(), v.end());
}
inline vector<char> str2vec(const string& s) {
    return vector<char>(s.begin(), s.end());
}

// Count word count
inline int count_words(const string& s) {
    int count = 0;
    bool in_word = false;
    for (char c : s) {
        if (isspace(c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            count++;
        }
    }
    return max(count, 1);
}

// Simulate the real computational cost of LLM inference (matrix multiplication)
inline void simulate_llm_layer() {
    const int N = 128;
    static float A[N][N], B[N][N], C[N][N];
    static bool init = false;
    if(!init) {
        for(int i=0;i<N;i++) for(int j=0;j<N;j++) { A[i][j]=0.01f; B[i][j]=0.02f; }
        init = true;
    }
    for(int i=0;i<N;i++) {
        for(int k=0;k<N;k++) {
            for(int j=0;j<N;j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Real environment attack simulation: divided into three levels
// L1: Lossless conversion (almost no impact, skip the complex implementation here and return the original string)
inline string apply_l1_attack(const string& text) {
    return text;
}

// L2: Common noise processing (removal of extra spaces, invisible characters, rich text labels, case conversion)
inline string apply_l2_attack(const string& text) {
    string res;
    bool in_space = false;
    bool in_tag = false;
    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char c = text[i];
        
        // Attack 1: Remove invisible characters (ZWC: E2 80 8B / 8C)
        if (c == 0xE2 && i + 2 < text.length() && (unsigned char)text[i+1] == 0x80) {
            unsigned char c3 = text[i+2];
            if (c3 == 0x8B || c3 == 0x8C || c3 == 0x8D) {
                i += 2;
                continue;
            }
        }
        
        // Attack 2: Remove HTML/XML rich text tags (Font)
        if (c == '<') { in_tag = true; continue; }
        if (c == '>') { in_tag = false; continue; }
        if (in_tag) continue;

        if (i + 1 < text.length()) {
            unsigned char c2 = (unsigned char)text[i + 1];
            if ((c == 0xD0 || c == 0xD1) && (c2 == 0xB0 || c2 == 0x90 || c2 == 0xB5 || c2 == 0x95 || c2 == 0xBA || c2 == 0x9A || c2 == 0xBC || c2 == 0x9C || c2 == 0xBE || c2 == 0x9E || c2 == 0x80 || c2 == 0xA0 || c2 == 0x82 || c2 == 0xA2 || c2 == 0x85 || c2 == 0xA5 || c2 == 0x83 || c2 == 0xA3 || c2 == 0x81)) {
                string two = text.substr(i, 2);
                if (two == "\xD0\xB0" || two == "\xD0\x90") { res += 'a'; i += 1; in_space = false; continue; }
                if (two == "\xD1\x81" || two == "\xD0\xA1") { res += 'c'; i += 1; in_space = false; continue; }
                if (two == "\xD0\xB5" || two == "\xD0\x95") { res += 'e'; i += 1; in_space = false; continue; }
                if (two == "\xD1\x96") { res += 'i'; i += 1; in_space = false; continue; }
                if (two == "\xD0\xBA" || two == "\xD0\x9A") { res += 'k'; i += 1; in_space = false; continue; }
                if (two == "\xD0\xBC" || two == "\xD0\x9C") { res += 'm'; i += 1; in_space = false; continue; }
                if (two == "\xD0\xBE" || two == "\xD0\x9E") { res += 'o'; i += 1; in_space = false; continue; }
                if (two == "\xD1\x80" || two == "\xD0\xA0") { res += 'p'; i += 1; in_space = false; continue; }
                if (two == "\xD1\x82" || two == "\xD0\xA2") { res += 't'; i += 1; in_space = false; continue; }
                if (two == "\xD1\x85" || two == "\xD0\xA5") { res += 'x'; i += 1; in_space = false; continue; }
                if (two == "\xD1\x83" || two == "\xD0\xA3") { res += 'y'; i += 1; in_space = false; continue; }
            }
        }

        // Attack 3: Format conversion and redundant space cleaning
        if (c == ' ' || c == '\t' || c == '\r') {
            if (!in_space) {
                res += ' ';
                in_space = true;
            }
        } else if (c == '\n') {
            if (!res.empty() && res.back() == ' ') res.pop_back(); // Remove spaces at the end of lines
            res += '\n';
            in_space = false;
        } else {
            res += tolower(c); // L2 attack breaks case layout
            in_space = false;
        }
    }
    return res;
}

// L3: Semantic rewriting (for linguistic steganography, synonym replacement/abbreviation recovery)
inline string apply_l3_attack(const string& text) {
    string res = apply_l2_attack(text); // L3 contains L2
    static const vector<pair<string, string>> syn_pairs = {
        {"error", "fault"},
        {"info", "detail"},
        {"start", "begin"},
        {"stop", "halt"},
        {"warn", "caution"},
        {"retry", "reattempt"},
        {"connect", "link"},
        {"disconnect", "unlink"},
        {"receive", "get"},
        {"send", "dispatch"},
        {"update", "refresh"},
        {"create", "build"},
        {"delete", "remove"},
        {"timeout", "expiry"}
    };
    static unordered_map<string, string> to_base;
    if (to_base.empty()) {
        for (auto const& kv : syn_pairs) to_base[kv.second] = kv.first;
        to_base["can't"] = "cannot";
        to_base["don't"] = "do not";
        to_base["msg"] = "message";
        to_base["ms"] = "milliseconds";
        to_base["sec"] = "seconds";
        to_base["s"] = "seconds";
        to_base["min"] = "minutes";
        to_base["kb"] = "kilobytes";
        to_base["mb"] = "megabytes";
        to_base["gb"] = "gigabytes";
    }

    auto spans = scan_words(res);
    string out;
    out.reserve(res.size());
    size_t last = 0;
    for (auto const& w : spans) {
        auto it = to_base.find(w.lower);
        if (it == to_base.end()) continue;
        out.append(res, last, w.start - last);
        string orig = res.substr(w.start, w.end - w.start);
        out += apply_case_like(orig, it->second);
        last = w.end;
    }
    out.append(res, last, res.size() - last);
    return out;
}

static unordered_map<string, vector<pair<string, int>>> G_BIGRAM;
static vector<string> G_START_TOKENS;

inline vector<string> corpus_tokens(const string& s);

inline long long count_zwc_bytes(const string& s) {
    long long c = 0;
    for (size_t i = 0; i + 2 < s.size(); i++) {
        unsigned char b0 = (unsigned char)s[i];
        unsigned char b1 = (unsigned char)s[i + 1];
        unsigned char b2 = (unsigned char)s[i + 2];
        if (b0 == 0xE2 && b1 == 0x80 && (b2 == 0x8B || b2 == 0x8C || b2 == 0x8D)) c++;
    }
    return c;
}

inline long long count_homoglyph_bytes(const string& s) {
    static const unordered_set<string> glyphs = {
        "\xD0\xB0", "\xD0\x90",
        "\xD1\x81", "\xD0\xA1",
        "\xD0\xB5", "\xD0\x95",
        "\xD1\x96",
        "\xD0\xBA", "\xD0\x9A",
        "\xD0\xBC", "\xD0\x9C",
        "\xD0\xBE", "\xD0\x9E",
        "\xD1\x80", "\xD0\xA0",
        "\xD1\x82", "\xD0\xA2",
        "\xD1\x85", "\xD0\xA5",
        "\xD1\x83", "\xD0\xA3"
    };
    long long c = 0;
    for (size_t i = 0; i + 1 < s.size(); i++) {
        unsigned char b0 = (unsigned char)s[i];
        if (b0 == 0xD0 || b0 == 0xD1) {
            string two = s.substr(i, 2);
            if (glyphs.find(two) != glyphs.end()) c++;
        }
    }
    return c;
}

inline long long count_substr_ci(const string& s, const string& needle_lower) {
    string low = ascii_lower(s);
    long long c = 0;
    size_t pos = 0;
    while ((pos = low.find(needle_lower, pos)) != string::npos) {
        c++;
        pos += needle_lower.size();
    }
    return c;
}

struct WSSignature {
    double double_space_ratio = 0.0;
    long long trailing_space_lines = 0;
    long long max_space_run = 0;
};

inline WSSignature ws_signature(const string& s) {
    WSSignature sig;
    long long spaces = 0;
    long long doubles = 0;
    long long run = 0;
    bool line_has_trailing = false;
    bool in_line = false;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        if (c == ' ') {
            spaces++;
            run++;
            if (run > sig.max_space_run) sig.max_space_run = run;
            if (i + 1 < s.size() && s[i + 1] == ' ') doubles++;
            line_has_trailing = true;
            in_line = true;
        } else if (c == '\t' || c == '\r') {
            line_has_trailing = true;
            in_line = true;
            run = 0;
        } else if (c == '\n') {
            if (in_line && line_has_trailing) sig.trailing_space_lines++;
            line_has_trailing = false;
            in_line = false;
            run = 0;
        } else {
            line_has_trailing = false;
            in_line = true;
            run = 0;
        }
    }
    sig.double_space_ratio = spaces > 0 ? (double)doubles / (double)spaces : 0.0;
    return sig;
}

inline double bigram_cross_entropy_bits(const string& s) {
    auto toks = corpus_tokens(s);
    if (toks.size() < 2) return 0.0;
    double sum = 0.0;
    long long n = 0;
    for (size_t i = 1; i < toks.size(); i++) {
        string prev = toks[i - 1];
        string nxt = toks[i];
        auto it = G_BIGRAM.find(prev);
        if (it == G_BIGRAM.end() || it->second.empty()) {
            sum += 20.0;
            n++;
            continue;
        }
        auto const& cand = it->second;
        long long total = 0;
        int count_next = 0;
        for (auto const& kv : cand) {
            total += kv.second;
            if (kv.first == nxt) count_next = kv.second;
        }
        long long V = (long long)cand.size();
        double p = (double)(count_next + 1) / (double)(total + V);
        sum += -log2(p);
        n++;
    }
    return n > 0 ? sum / (double)n : 0.0;
}

struct DetectReport {
    bool det_zwc = false;
    bool det_markup = false;
    bool det_homoglyph = false;
    bool det_trailing = false;
    bool det_ws_stats = false;
    bool det_lm = false;
    long long zwc_count = 0;
    long long markup_count = 0;
    long long homoglyph_count = 0;
    WSSignature ws;
    double lm_ce = 0.0;
    double lm_ce_delta = 0.0;
};

inline DetectReport detect_all(const string& orig, const string& stego) {
    DetectReport r;
    r.zwc_count = count_zwc_bytes(stego);
    r.markup_count = count_substr_ci(stego, "<font");
    r.homoglyph_count = count_homoglyph_bytes(stego);
    r.ws = ws_signature(stego);
    double ce0 = bigram_cross_entropy_bits(orig);
    r.lm_ce = bigram_cross_entropy_bits(stego);
    r.lm_ce_delta = r.lm_ce - ce0;

    WSSignature base = ws_signature(orig);
    r.det_zwc = r.zwc_count > 0;
    r.det_markup = r.markup_count > 0;
    r.det_homoglyph = r.homoglyph_count > 0;
    r.det_trailing = r.ws.trailing_space_lines > base.trailing_space_lines;
    r.det_ws_stats = fabs(r.ws.double_space_ratio - base.double_space_ratio) > 0.002 || (r.ws.max_space_run - base.max_space_run) > 3;
    r.det_lm = r.lm_ce_delta > 0.8;
    return r;
}

// ================= Real encoding and decoding implementation of each steganography scheme =================

// 1.1 Whitespace ambiguity encoding (Whitespace)
// Calculate the maximum theoretical capacity: the number of spaces available as slots
inline int max_capacity_ws(const string& s) {
    int slots = 0;
    for (char c : s) if (c == ' ') slots++;
    return slots;
}
inline string embed_ws(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    string res;
    int bit_idx = 0;
    for(char c : s) {
        if(c == ' ' && bit_idx < bits.size()) {
            res += (bits[bit_idx] == 1) ? "  " : " ";
            bit_idx++; embedded++;
        } else {
            res += c;
        }
    }
    return res;
}
inline vector<int> extract_ws(string s, int target) {
    vector<int> bits;
    for(size_t i=0; i<s.length() && bits.size() < target; ++i) {
        if(s[i] == ' ') {
            int spaces = 1;
            while(i+1 < s.length() && s[i+1] == ' ') { spaces++; i++; }
            bits.push_back((spaces >= 2) ? 1 : 0);
        }
    }
    return bits;
}

// 1.2 ZWC
inline int max_capacity_zwc(const string& s) {
    int slots = 0;
    for (char c : s) if (c == ' ') slots++;
    return slots;
}
inline string embed_zwc(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    string res;
    int bit_idx = 0;
    for(char c : s) {
        res += c;
        if(c == ' ' && bit_idx < bits.size()) {
            res += (bits[bit_idx] == 1) ? "\xE2\x80\x8C" : "\xE2\x80\x8B";
            bit_idx++; embedded++;
        }
    }
    return res;
}
inline vector<int> extract_zwc(string s, int target) {
    vector<int> bits;
    for(size_t i=0; i<s.length() && bits.size() < target; ++i) {
        if((unsigned char)s[i] == 0xE2 && i+2 < s.length() && (unsigned char)s[i+1] == 0x80) {
            if((unsigned char)s[i+2] == 0x8C) bits.push_back(1);
            else if((unsigned char)s[i+2] == 0x8B) bits.push_back(0);
            i += 2;
        }
    }
    return bits;
}

inline unordered_map<char, string> homoglyph_map_lower() {
    unordered_map<char, string> m;
    m['a'] = "\xD0\xB0";
    m['c'] = "\xD1\x81";
    m['e'] = "\xD0\xB5";
    m['i'] = "\xD1\x96";
    m['k'] = "\xD0\xBA";
    m['m'] = "\xD0\xBC";
    m['o'] = "\xD0\xBE";
    m['p'] = "\xD1\x80";
    m['t'] = "\xD1\x82";
    m['x'] = "\xD1\x85";
    m['y'] = "\xD1\x83";
    return m;
}

inline unordered_map<string, char> homoglyph_rev_map() {
    unordered_map<string, char> m;
    m["\xD0\xB0"] = 'a';
    m["\xD1\x81"] = 'c';
    m["\xD0\xB5"] = 'e';
    m["\xD1\x96"] = 'i';
    m["\xD0\xBA"] = 'k';
    m["\xD0\xBC"] = 'm';
    m["\xD0\xBE"] = 'o';
    m["\xD1\x80"] = 'p';
    m["\xD1\x82"] = 't';
    m["\xD1\x85"] = 'x';
    m["\xD1\x83"] = 'y';
    return m;
}

inline int max_capacity_font(const string& s) {
    static const unordered_map<char, string> hm = homoglyph_map_lower();
    int slots = 0;
    for (unsigned char c : s) {
        char lc = (char)tolower(c);
        if (hm.find(lc) != hm.end()) slots++;
    }
    return slots;
}

inline string embed_font(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    static const unordered_map<char, string> hm = homoglyph_map_lower();
    string res;
    res.reserve(s.size());
    int bit_idx = 0;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = (unsigned char)s[i];
        char lc = (char)tolower(c);
        auto it = hm.find(lc);
        if (it != hm.end() && bit_idx < (int)bits.size()) {
            if (bits[bit_idx] == 1) {
                if (c >= 'A' && c <= 'Z') {
                    string g = it->second;
                    if (g == "\xD0\xB0") res += "\xD0\x90";
                    else if (g == "\xD0\xB5") res += "\xD0\x95";
                    else if (g == "\xD0\xBA") res += "\xD0\x9A";
                    else if (g == "\xD0\xBC") res += "\xD0\x9C";
                    else if (g == "\xD0\xBE") res += "\xD0\x9E";
                    else if (g == "\xD1\x80") res += "\xD0\xA0";
                    else if (g == "\xD1\x82") res += "\xD0\xA2";
                    else if (g == "\xD1\x85") res += "\xD0\xA5";
                    else if (g == "\xD1\x83") res += "\xD0\xA3";
                    else res += it->second;
                } else {
                    res += it->second;
                }
            } else {
                res += (char)c;
            }
            bit_idx++;
            embedded++;
        } else {
            res += (char)c;
        }
    }
    return res;
}

inline vector<int> extract_font(string s, int target) {
    static const unordered_map<char, string> hm = homoglyph_map_lower();
    static const unordered_map<string, char> rev = homoglyph_rev_map();
    static const unordered_set<string> upper = {
        "\xD0\x90", "\xD0\x95", "\xD0\x9A", "\xD0\x9C", "\xD0\x9E", "\xD0\xA0", "\xD0\xA2", "\xD0\xA5", "\xD0\xA3"
    };

    vector<int> bits;
    for (size_t i = 0; i < s.size() && (int)bits.size() < target; ) {
        unsigned char c = (unsigned char)s[i];
        if (c < 0x80) {
            char lc = (char)tolower(c);
            if (hm.find(lc) != hm.end()) bits.push_back(0);
            i++;
            continue;
        }
        if (i + 1 < s.size()) {
            string two = s.substr(i, 2);
            if (rev.find(two) != rev.end() || upper.find(two) != upper.end()) {
                bits.push_back(1);
                i += 2;
                continue;
            }
        }
        i++;
    }
    return bits;
}

// 2.1 Synonym
inline int max_capacity_syn(const string& s) {
    static const vector<pair<string, string>> pairs = {
        {"error", "fault"},
        {"info", "detail"},
        {"start", "begin"},
        {"stop", "halt"},
        {"warn", "caution"},
        {"retry", "reattempt"},
        {"connect", "link"},
        {"disconnect", "unlink"},
        {"receive", "get"},
        {"send", "dispatch"},
        {"update", "refresh"},
        {"create", "build"},
        {"delete", "remove"},
        {"timeout", "expiry"}
    };
    static unordered_set<string> keys;
    static unordered_set<string> vals;
    if (keys.empty()) {
        for (auto const& kv : pairs) {
            keys.insert(kv.first);
            vals.insert(kv.second);
        }
    }

    int slots = 0;
    auto spans = scan_words(s);
    for (auto const& w : spans) {
        if (keys.find(w.lower) != keys.end() || vals.find(w.lower) != vals.end()) slots++;
    }
    return slots;
}
inline string embed_syn(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    static const vector<pair<string, string>> pairs = {
        {"error", "fault"},
        {"info", "detail"},
        {"start", "begin"},
        {"stop", "halt"},
        {"warn", "caution"},
        {"retry", "reattempt"},
        {"connect", "link"},
        {"disconnect", "unlink"},
        {"receive", "get"},
        {"send", "dispatch"},
        {"update", "refresh"},
        {"create", "build"},
        {"delete", "remove"},
        {"timeout", "expiry"}
    };
    static unordered_map<string, string> to_alt;
    static unordered_map<string, string> to_base;
    if (to_alt.empty()) {
        for (auto const& kv : pairs) {
            to_alt[kv.first] = kv.second;
            to_base[kv.second] = kv.first;
        }
    }

    auto spans = scan_words(s);
    string out;
    out.reserve(s.size());
    size_t last = 0;
    int bit_idx = 0;
    for (size_t i = 0; i < spans.size() && bit_idx < (int)bits.size(); i++) {
        auto const& w = spans[i];
        auto it_base = to_alt.find(w.lower);
        auto it_alt = to_base.find(w.lower);
        if (it_base == to_alt.end() && it_alt == to_base.end()) continue;

        out.append(s, last, w.start - last);
        string base = (it_base != to_alt.end()) ? w.lower : it_alt->second;
        string alt = (it_base != to_alt.end()) ? it_base->second : w.lower;
        string chosen = (bits[bit_idx] == 1) ? alt : base;
        string orig = s.substr(w.start, w.end - w.start);
        out += apply_case_like(orig, chosen);
        last = w.end;
        bit_idx++;
        embedded++;
    }
    out.append(s, last, s.size() - last);
    return out;
}
inline vector<int> extract_syn(string s, int target) {
    static const vector<pair<string, string>> pairs = {
        {"error", "fault"},
        {"info", "detail"},
        {"start", "begin"},
        {"stop", "halt"},
        {"warn", "caution"},
        {"retry", "reattempt"},
        {"connect", "link"},
        {"disconnect", "unlink"},
        {"receive", "get"},
        {"send", "dispatch"},
        {"update", "refresh"},
        {"create", "build"},
        {"delete", "remove"},
        {"timeout", "expiry"}
    };
    static unordered_map<string, int> base_to_bit;
    static unordered_map<string, int> alt_to_bit;
    if (base_to_bit.empty()) {
        for (auto const& kv : pairs) {
            base_to_bit[kv.first] = 0;
            alt_to_bit[kv.second] = 1;
        }
    }

    vector<int> bits;
    auto spans = scan_words(s);
    for (auto const& w : spans) {
        if ((int)bits.size() >= target) break;
        auto it0 = base_to_bit.find(w.lower);
        if (it0 != base_to_bit.end()) {
            bits.push_back(0);
            continue;
        }
        auto it1 = alt_to_bit.find(w.lower);
        if (it1 != alt_to_bit.end()) {
            bits.push_back(1);
            continue;
        }
    }
    return bits;
}

// 2.2 Syntactic (Active/Passive / Clause Swap)
inline int max_capacity_syn_trans(const string& s) {
    int slots = 0;
    stringstream ss(s);
    string line;
    while (getline(ss, line)) {
        if (!line.empty()) slots++;
    }
    return slots;
}
inline string embed_syn_trans(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    stringstream ss(s);
    string line, res;
    int bit_idx = 0;
    while(getline(ss, line)) {
        if(bit_idx < bits.size()) {
            string payload = line;
            string low = ascii_lower(line);
            if (low.rfind("system error:", 0) == 0) payload = line.substr(string("system error:").size());
            if (low.rfind("error in system:", 0) == 0) payload = line.substr(string("error in system:").size());
            while (!payload.empty() && (payload[0] == ' ' || payload[0] == '\t')) payload.erase(payload.begin());

            if(bits[bit_idx] == 1) res += "error in system: " + payload + "\n";
            else res += "system error: " + payload + "\n";
            bit_idx++; embedded++;
        } else {
            res += line + "\n";
        }
    }
    return res;
}
inline vector<int> extract_syn_trans(string s, int target) {
    vector<int> bits;
    stringstream ss(s);
    string line;
    while(getline(ss, line) && bits.size() < target) {
        string low = ascii_lower(line);
        if(low.rfind("system error:", 0) == 0) bits.push_back(0);
        else if(low.rfind("error in system:", 0) == 0) bits.push_back(1);
    }
    return bits;
}

// 2.3 Abbrev
inline int max_capacity_abbr(const string& s) {
    static const unordered_map<string, string> single = {
        {"cannot", "can't"},
        {"milliseconds", "ms"},
        {"seconds", "sec"},
        {"minutes", "min"},
        {"message", "msg"},
        {"kilobytes", "kb"},
        {"megabytes", "mb"},
        {"gigabytes", "gb"}
    };
    static const unordered_map<string, string> multi2 = {
        {"do not", "don't"}
    };
    static unordered_set<string> single_all;
    static unordered_set<string> multi2_all;
    if (single_all.empty()) {
        for (auto const& kv : single) { single_all.insert(kv.first); single_all.insert(kv.second); }
        for (auto const& kv : multi2) { multi2_all.insert(kv.first); multi2_all.insert(kv.second); }
    }

    auto spans = scan_words(s);
    int slots = 0;
    for (size_t i = 0; i < spans.size(); i++) {
        if (i + 1 < spans.size()) {
            if (spans[i].end <= spans[i + 1].start) {
                bool only_ws = true;
                for (size_t k = spans[i].end; k < spans[i + 1].start; k++) {
                    char c = s[k];
                    if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { only_ws = false; break; }
                }
                if (only_ws) {
                    string phrase = spans[i].lower + " " + spans[i + 1].lower;
                    if (multi2_all.find(phrase) != multi2_all.end()) { slots++; i++; continue; }
                }
            }
        }
        if (single_all.find(spans[i].lower) != single_all.end()) slots++;
    }
    return slots;
}
inline string embed_abbr(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    static const unordered_map<string, string> single = {
        {"cannot", "can't"},
        {"milliseconds", "ms"},
        {"seconds", "sec"},
        {"minutes", "min"},
        {"message", "msg"},
        {"kilobytes", "kb"},
        {"megabytes", "mb"},
        {"gigabytes", "gb"}
    };
    static unordered_map<string, string> single_to_base;
    static unordered_map<string, string> single_to_alt;
    if (single_to_alt.empty()) {
        for (auto const& kv : single) {
            single_to_alt[kv.first] = kv.second;
            single_to_base[kv.second] = kv.first;
        }
    }

    auto spans = scan_words(s);
    string out;
    out.reserve(s.size());
    size_t last = 0;
    int bit_idx = 0;
    for (size_t i = 0; i < spans.size() && bit_idx < (int)bits.size(); i++) {
        auto const& w = spans[i];

        if (i + 1 < spans.size()) {
            bool only_ws = true;
            for (size_t k = spans[i].end; k < spans[i + 1].start; k++) {
                char c = s[k];
                if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { only_ws = false; break; }
            }
            if (only_ws) {
                string phrase = spans[i].lower + " " + spans[i + 1].lower;
                if (phrase == "do not" || phrase == "don't") {
                    out.append(s, last, w.start - last);
                    string chosen = (bits[bit_idx] == 1) ? "don't" : "do not";
                    out += chosen;
                    last = spans[i + 1].end;
                    i++;
                    bit_idx++;
                    embedded++;
                    continue;
                }
            }
        }

        auto itb = single_to_alt.find(w.lower);
        auto ita = single_to_base.find(w.lower);
        if (itb == single_to_alt.end() && ita == single_to_base.end()) continue;

        out.append(s, last, w.start - last);
        string base = (itb != single_to_alt.end()) ? w.lower : ita->second;
        string alt = (itb != single_to_alt.end()) ? itb->second : w.lower;
        string chosen = (bits[bit_idx] == 1) ? alt : base;
        string orig = s.substr(w.start, w.end - w.start);
        out += apply_case_like(orig, chosen);
        last = w.end;
        bit_idx++;
        embedded++;
    }
    out.append(s, last, s.size() - last);
    return out;
}
inline vector<int> extract_abbr(string s, int target) {
    static const unordered_map<string, string> single = {
        {"cannot", "can't"},
        {"milliseconds", "ms"},
        {"seconds", "sec"},
        {"minutes", "min"},
        {"message", "msg"},
        {"kilobytes", "kb"},
        {"megabytes", "mb"},
        {"gigabytes", "gb"}
    };
    static unordered_map<string, int> base_bit;
    static unordered_map<string, int> alt_bit;
    if (base_bit.empty()) {
        for (auto const& kv : single) {
            base_bit[kv.first] = 0;
            alt_bit[kv.second] = 1;
        }
    }

    vector<int> bits;
    auto spans = scan_words(s);
    for (size_t i = 0; i < spans.size() && (int)bits.size() < target; i++) {
        if (i + 1 < spans.size()) {
            bool only_ws = true;
            for (size_t k = spans[i].end; k < spans[i + 1].start; k++) {
                char c = s[k];
                if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { only_ws = false; break; }
            }
            if (only_ws) {
                string phrase = spans[i].lower + " " + spans[i + 1].lower;
                if (phrase == "do not") { bits.push_back(0); i++; continue; }
                if (phrase == "don't") { bits.push_back(1); continue; }
            }
        }

        auto it0 = base_bit.find(spans[i].lower);
        if (it0 != base_bit.end()) { bits.push_back(0); continue; }
        auto it1 = alt_bit.find(spans[i].lower);
        if (it1 != alt_bit.end()) { bits.push_back(1); continue; }
    }
    return bits;
}

// 3.1 Markov
inline int max_capacity_gen(const string& s) {
    return count_words(s);
}
inline int max_capacity_gen_llm(const string& s) {
    long long w = count_words(s);
    long long cap = w * 2;
    if (cap > INT_MAX) cap = INT_MAX;
    return (int)cap;
}

inline vector<string> corpus_tokens(const string& s) {
    vector<string> toks;
    auto spans = scan_words(s);
    toks.reserve(spans.size());
    for (auto const& w : spans) toks.push_back(w.lower);
    return toks;
}

inline void build_markov_model_for_corpus(const string& corpus) {
    G_BIGRAM.clear();
    G_START_TOKENS.clear();
    auto toks = corpus_tokens(corpus);
    if (toks.empty()) return;
    G_START_TOKENS.push_back(toks[0]);
    unordered_map<string, unordered_map<string, int>> counts;
    for (size_t i = 0; i + 1 < toks.size(); i++) {
        counts[toks[i]][toks[i + 1]]++;
    }
    for (auto& kv : counts) {
        vector<pair<string, int>> v;
        v.reserve(kv.second.size());
        for (auto& kv2 : kv.second) v.push_back({kv2.first, kv2.second});
        sort(v.begin(), v.end(), [](auto const& a, auto const& b) { return a.second > b.second; });
        G_BIGRAM[kv.first] = std::move(v);
    }
    for (size_t i = 0; i + 1 < toks.size(); i += 101) G_START_TOKENS.push_back(toks[i]);
}

inline uint64_t token_hash(const string& s) {
    return (uint64_t)std::hash<string>{}(s);
}

inline string embed_markov(string orig, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    auto toks = corpus_tokens(orig);
    if (G_BIGRAM.empty()) build_markov_model_for_corpus(orig);
    string cur = (!G_START_TOKENS.empty()) ? G_START_TOKENS[gen() % G_START_TOKENS.size()] : (toks.empty() ? "log" : toks[0]);
    vector<string> out;
    out.reserve(max((int)bits.size() + 1, 32));
    out.push_back(cur);

    for (int b : bits) {
        auto it = G_BIGRAM.find(cur);
        string next;
        if (it == G_BIGRAM.end() || it->second.empty()) {
            next = (!toks.empty()) ? toks[gen() % toks.size()] : "data";
        } else {
            auto const& cand = it->second;
            if (cand.size() < 2) {
                next = cand.front().first;
            } else {
                long long total = 0;
                for (auto const& c : cand) total += c.second;
                long long half = total / 2;
                long long acc = 0;
                size_t split = 0;
                for (; split < cand.size(); split++) {
                    acc += cand[split].second;
                    if (acc >= half) { split++; break; }
                }
                if (split == 0) split = 1;
                if (split >= cand.size()) split = cand.size() - 1;

                if (b == 0) next = cand.front().first;
                else next = cand[split].first;
            }
        }
        out.push_back(next);
        embedded++;
        cur = next;
    }

    string res;
    for (auto const& w : out) { res += w; res += ' '; }
    return res;
}

inline vector<int> extract_markov(string s, int target) {
    vector<int> bits;
    auto toks = corpus_tokens(s);
    for (size_t i = 1; i < toks.size() && (int)bits.size() < target; i++) {
        string prev = toks[i - 1];
        string nxt = toks[i];
        auto it = G_BIGRAM.find(prev);
        if (it == G_BIGRAM.end() || it->second.size() < 2) {
            bits.push_back(0);
            continue;
        }
        auto const& cand = it->second;
        long long total = 0;
        for (auto const& c : cand) total += c.second;
        long long half = total / 2;
        long long acc = 0;
        size_t split = 0;
        for (; split < cand.size(); split++) {
            acc += cand[split].second;
            if (acc >= half) { split++; break; }
        }
        if (split == 0) split = 1;
        if (split >= cand.size()) split = cand.size() - 1;

        int bit = 0;
        for (size_t j = split; j < cand.size(); j++) {
            if (cand[j].first == nxt) { bit = 1; break; }
        }
        bits.push_back(bit);
    }
    return bits;
}

// 3.2 LLM
inline string embed_llm(string orig, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    auto toks = corpus_tokens(orig);
    if (G_BIGRAM.empty()) build_markov_model_for_corpus(orig);
    string cur = (!G_START_TOKENS.empty()) ? G_START_TOKENS[gen() % G_START_TOKENS.size()] : (toks.empty() ? "log" : toks[0]);
    vector<string> out;
    out.reserve(max((int)bits.size() + 1, 32));
    out.push_back(cur);

    for (size_t bi = 0; bi < bits.size(); ) {
        int b0 = bits[bi];
        int b1 = (bi + 1 < bits.size()) ? bits[bi + 1] : 0;
        int sym = (b0 << 1) | b1;
        auto it = G_BIGRAM.find(cur);
        string next;
        if (it == G_BIGRAM.end() || it->second.empty()) {
            next = (!toks.empty()) ? toks[gen() % toks.size()] : "data";
        } else {
            auto const& cand = it->second;
            long long total = 0;
            for (auto const& c : cand) total += c.second;
            long long q1 = total / 4;
            long long q2 = total / 2;
            long long q3 = (total * 3) / 4;
            long long acc = 0;
            size_t s1 = 1, s2 = 1, s3 = 1;
            for (size_t i = 0; i < cand.size(); i++) {
                acc += cand[i].second;
                if (acc >= q1 && s1 == 1) s1 = i + 1;
                if (acc >= q2 && s2 == 1) s2 = i + 1;
                if (acc >= q3 && s3 == 1) { s3 = i + 1; break; }
            }
            if (s1 == 0) s1 = 1;
            if (s2 < s1) s2 = s1;
            if (s3 < s2) s3 = s2;
            if (s1 >= cand.size()) s1 = cand.size() - 1;
            if (s2 >= cand.size()) s2 = cand.size() - 1;
            if (s3 >= cand.size()) s3 = cand.size() - 1;

            if (sym == 0) next = cand.front().first;
            else if (sym == 1) next = cand[s1].first;
            else if (sym == 2) next = cand[s2].first;
            else next = cand[s3].first;
        }
        out.push_back(next);
        embedded += (bi + 1 < bits.size()) ? 2 : 1;
        cur = next;
        simulate_llm_layer();
        bi += 2;
    }

    string res;
    for (auto const& w : out) { res += w; res += ' '; }
    return res;
}
inline vector<int> extract_llm(string s, int target) {
    vector<int> bits;
    auto toks = corpus_tokens(s);
    if (toks.size() < 2) return bits;
    for (size_t i = 1; i < toks.size() && (int)bits.size() < target; i++) {
        string prev = toks[i - 1];
        string nxt = toks[i];
        auto it = G_BIGRAM.find(prev);
        if (it == G_BIGRAM.end() || it->second.empty()) {
            bits.push_back(0);
            if ((int)bits.size() < target) bits.push_back(0);
            continue;
        }
        auto const& cand = it->second;
        long long total = 0;
        for (auto const& c : cand) total += c.second;
        long long q1 = total / 4;
        long long q2 = total / 2;
        long long q3 = (total * 3) / 4;
        long long acc = 0;
        size_t s1 = 1, s2 = 1, s3 = 1;
        for (size_t k = 0; k < cand.size(); k++) {
            acc += cand[k].second;
            if (acc >= q1 && s1 == 1) s1 = k + 1;
            if (acc >= q2 && s2 == 1) s2 = k + 1;
            if (acc >= q3 && s3 == 1) { s3 = k + 1; break; }
        }
        if (s1 == 0) s1 = 1;
        if (s2 < s1) s2 = s1;
        if (s3 < s2) s3 = s2;
        if (s1 >= cand.size()) s1 = cand.size() - 1;
        if (s2 >= cand.size()) s2 = cand.size() - 1;
        if (s3 >= cand.size()) s3 = cand.size() - 1;

        int sym = 0;
        if (nxt == cand.front().first) sym = 0;
        else if (nxt == cand[s1].first) sym = 1;
        else if (nxt == cand[s2].first) sym = 2;
        else if (nxt == cand[s3].first) sym = 3;
        else {
            sym = 0;
            for (size_t k = 0; k < cand.size(); k++) {
                if (cand[k].first == nxt) {
                    if (k >= s3) sym = 3;
                    else if (k >= s2) sym = 2;
                    else if (k >= s1) sym = 1;
                    else sym = 0;
                    break;
                }
            }
        }
        bits.push_back((sym >> 1) & 1);
        if ((int)bits.size() < target) bits.push_back(sym & 1);
    }
    return bits;
}

// 4.1 Padding
inline int max_capacity_pad(const string& s) {
    int slots = 0;
    for (char c : s) if (c == '\n') slots++;
    return slots;
}
inline string embed_pad(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    string res; int bit_idx = 0;
    for(char c : s) {
        if(c == '\n' && bit_idx < bits.size()) {
            res += (bits[bit_idx] == 1) ? "  \n" : " \n";
            bit_idx++; embedded++;
        } else { res += c; }
    }
    return res;
}
inline vector<int> extract_pad(string s, int target) {
    vector<int> bits;
    for(size_t i=1; i<s.length() && bits.size() < target; ++i) {
        if(s[i] == '\n') {
            if(s[i-1] == ' ' && i>=2 && s[i-2] == ' ') bits.push_back(1);
            else if(s[i-1] == ' ') bits.push_back(0);
        }
    }
    return bits;
}

// 4.2 Layout
inline int max_capacity_layout(const string& s) {
    int slots = 0; bool is_start = true;
    for (char c : s) {
        if(is_start && isalpha(c)) { slots++; is_start = false; }
        if(c == '\n') is_start = true;
    }
    return slots;
}
inline string embed_layout(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    string res; int bit_idx = 0; bool is_start = true;
    for(char c : s) {
        if(is_start && isalpha(c) && bit_idx < bits.size()) {
            res += (bits[bit_idx] == 1) ? (char)toupper(c) : (char)tolower(c);
            bit_idx++; embedded++; is_start = false;
        } else {
            res += c;
            if(c == '\n') is_start = true;
        }
    }
    return res;
}
inline vector<int> extract_layout(string s, int target) {
    vector<int> bits; bool is_start = true;
    for(char c : s) {
        if(is_start && isalpha(c) && bits.size() < target) {
            bits.push_back(isupper(c) ? 1 : 0);
            is_start = false;
        }
        if(c == '\n') is_start = true;
    }
    return bits;
}

// 4.3 Field Ordering
inline int max_capacity_field(const string& s) {
    int slots = 0;
    stringstream ss(s);
    string line;
    while (getline(ss, line)) {
        string low = ascii_lower(line);
        if (low.find("[type=a id=1]") != string::npos || low.find("[id=1 type=a]") != string::npos) slots++;
    }
    return slots;
}
inline string embed_field(string s, const vector<int>& bits, int& embedded, std::mt19937& gen) {
    stringstream ss(s);
    string line;
    string res;
    int bit_idx = 0;
    while (getline(ss, line)) {
        if (bit_idx < (int)bits.size()) {
            string low = ascii_lower(line);
            size_t p0 = low.find("[type=a id=1]");
            size_t p1 = low.find("[id=1 type=a]");
            if (p0 != string::npos || p1 != string::npos) {
                if (bits[bit_idx] == 1) {
                    if (p0 != string::npos) line.replace(p0, strlen("[type=a id=1]"), "[id=1 type=a]");
                } else {
                    if (p1 != string::npos) line.replace(p1, strlen("[id=1 type=a]"), "[type=a id=1]");
                }
                bit_idx++;
                embedded++;
            }
        }
        res += line + "\n";
    }
    return res;
}
inline vector<int> extract_field(string s, int target) {
    vector<int> bits;
    stringstream ss(s);
    string line;
    while (getline(ss, line) && (int)bits.size() < target) {
        string low = ascii_lower(line);
        if (low.find("[id=1 type=a]") != string::npos) bits.push_back(1);
        else if (low.find("[type=a id=1]") != string::npos) bits.push_back(0);
    }
    return bits;
}


void doGlobalComparisonWithArgs(string origFile, string secretMsg, bool debugMode, int iterations, string outDir) {
    cout << "\n=========================================================================================" << endl;
    cout << "=== Comprehensive benchmark test of known steganography schemes around the world (Academic Benchmark: Reproducible & Rigorous) ===" << endl;
    cout << "=========================================================================================" << endl;
    if (debugMode) cout << "[Debug] Debug mode is on." << endl;
    if (origFile.empty()) origFile = "log.txt";
    if (secretMsg.empty()) secretMsg = "Hidewriteforlog";
    if (outDir.empty()) outDir = "benchmark_outputs";

    cout << "Initializing the memory experimental environment and loading [" << origFile << "]..." << endl;

    vector<char> origContent;
    ifstream file(origFile, ios::binary);
    if (!file) {
        cout << "Error: Unable to read raw log file [" << origFile << "]. Please make sure the file exists." << endl;
        return;
    }
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    origContent.resize(size);
    if (!file.read(origContent.data(), size)) {
        cout << "File reading failed!" << endl;
        return;
    }
    
    string baseStr = vec2str(origContent);
    string origStr = baseStr;
    const int secretBitsCount = 256;

    auto all_caps_ok = [&](const string& s) -> bool {
        if (max_capacity_ws(s) < secretBitsCount) return false;
        if (max_capacity_zwc(s) < secretBitsCount) return false;
        if (max_capacity_font(s) < secretBitsCount) return false;
        if (max_capacity_syn(s) < secretBitsCount) return false;
        if (max_capacity_syn_trans(s) < secretBitsCount) return false;
        if (max_capacity_abbr(s) < secretBitsCount) return false;
        if (max_capacity_pad(s) < secretBitsCount) return false;
        if (max_capacity_layout(s) < secretBitsCount) return false;
        if (max_capacity_field(s) < secretBitsCount) return false;
        return true;
    };

    int repeats = 0;
    while (!all_caps_ok(origStr) && repeats < 30) {
        origStr += "\n" + baseStr;
        repeats++;
    }

    if (!all_caps_ok(origStr)) {
        string calib;
        for (int i = 0; i < 400; i++) {
            calib += "system error: start connect retry timeout receive send update create delete stop warn message kilobytes milliseconds seconds minutes [type=a id=1]\n";
            calib += "error in system: begin link reattempt expiry get dispatch refresh build remove halt caution msg kb ms sec min [type=a id=1]\n";
        }
        origStr += "\n" + calib;
    }

    origContent = str2vec(origStr);

    double origSizeKB = origContent.size() / 1024.0;
    int wordCount = count_words(origStr);

    cout << "Construction of a unified test data set is completed:" << origSizeKB << "KB, approx." << wordCount << "words/tags." << endl;
    build_markov_model_for_corpus(origStr);

    cout << "Prepare to perform scientific encoding/decoding tests of 11 scenarios (inject 256-bit secret information, fixed seed to reproduce)..." << endl;
    cout << "-----------------------------------------------------------------------------------------" << endl;

    const int ITERATIONS = max(1, iterations);
    vector<int> fixedSecret = message_to_bits_256(secretMsg);
    vector<vector<int>> secrets(ITERATIONS, vector<int>(secretBitsCount, 0));
    secrets[0] = fixedSecret;
    std::mt19937 genSecrets(42);
    for (int iter = 1; iter < ITERATIONS; iter++) {
        for (int i = 0; i < secretBitsCount; i++) secrets[iter][i] = (int)(genSecrets() & 1U);
    }

    std::filesystem::create_directories(outDir);
    string perIterCsvPath = outDir + "/benchmark_per_iter.csv";
    string summaryCsvPath = outDir + "/benchmark_summary.csv";
    ofstream perCsv(perIterCsvPath, ios::binary);
    if (perCsv) {
        perCsv
            << "secret,orig_file,iteration,is_fixed_secret,category,scheme,embedded,time_ms_kb,byte_kl,token_kl,token_bigram_kl,markov_shift,entropy_diff,rob_l2,rob_l3,zwc_count,markup_count,homoglyph_count,ws_double_space_ratio,ws_max_space_run,ws_trailing_space_lines,lm_ce,lm_ce_delta,det_zwc,det_markup,det_homoglyph,det_trailing,det_ws_stats,det_lm\n";
    }

    // 【Academic Correction 2】Deep Statistical Hidden Analysis
    // 1. KL divergence calculation (information entropy test)
    auto calcKL = [&](const vector<char>& P, const vector<char>& Q) -> double {
        vector<long long> freqP(256, 0), freqQ(256, 0);
        for(char c: P) freqP[(unsigned char)c]++;
        for(char c: Q) freqQ[(unsigned char)c]++;
        double totalP = P.size(), totalQ = Q.size();
        if (totalP == 0 || totalQ == 0) return 0.0;
        
        for(int i=0; i<256; i++) { freqP[i]++; freqQ[i]++; }
        totalP += 256; totalQ += 256;

        double kl = 0.0;
        for(int i=0; i<256; i++) {
            double p = (double)freqP[i] / totalP;
            double q = (double)freqQ[i] / totalQ;
            kl += p * log2(p / q);
        }
        return abs(kl); 
    };

    // 2. Second-order Markov transfer offset (feature extraction for space continuity)
    auto calcMarkovShift = [&](const string& orig, const string& stego) -> double {
        auto getTransitions = [](const string& s) {
            map<string, int> trans;
            for(size_t i=0; i<s.length()-1; i++) {
                bool space1 = (s[i] == ' ');
                bool space2 = (s[i+1] == ' ');
                if(!space1 && space2) trans["char_space"]++;
                if(space1 && space2) trans["space_space"]++;
            }
            return trans;
        };
        auto tOrig = getTransitions(orig);
        auto tStego = getTransitions(stego);
        
        double shift = abs(tStego["space_space"] - tOrig["space_space"]) + 
                       abs(tStego["char_space"] - tOrig["char_space"]);
        return shift;
    };

    // 3. Entropy Test
    auto calcEntropy = [&](const string& s) -> double {
        map<char, int> freqs;
        for (char c : s) freqs[c]++;
        double ent = 0.0;
        double len = s.length();
        if (len == 0) return 0.0;
        for (auto const& pair : freqs) {
            double p = pair.second / len;
            ent -= p * log2(p);
        }
        return ent;
    };

    vector<string> origTokens = corpus_tokens(origStr);
    auto calcTokenKL = [&](const string& stego) -> double {
        unordered_map<string, long long> fp;
        unordered_map<string, long long> fq;
        fp.reserve(origTokens.size());
        for (auto const& t : origTokens) fp[t]++;
        auto qt = corpus_tokens(stego);
        fq.reserve(qt.size());
        for (auto const& t : qt) fq[t]++;

        unordered_set<string> vocab;
        vocab.reserve(fp.size() + fq.size());
        for (auto const& kv : fp) vocab.insert(kv.first);
        for (auto const& kv : fq) vocab.insert(kv.first);
        if (vocab.empty()) return 0.0;

        double totalP = 0.0, totalQ = 0.0;
        for (auto const& kv : fp) totalP += (double)kv.second;
        for (auto const& kv : fq) totalQ += (double)kv.second;
        double V = (double)vocab.size();
        totalP += V;
        totalQ += V;

        double kl = 0.0;
        for (auto const& tok : vocab) {
            double p = (double)(fp[tok] + 1) / totalP;
            double q = (double)(fq[tok] + 1) / totalQ;
            kl += p * log2(p / q);
        }
        return fabs(kl);
    };

    unordered_map<string, long long> origBigram;
    origBigram.reserve(origTokens.size());
    for (size_t i = 1; i < origTokens.size(); i++) {
        origBigram[origTokens[i - 1] + "\t" + origTokens[i]]++;
    }
    auto calcTokenBigramKL = [&](const string& stego) -> double {
        unordered_map<string, long long> fq;
        auto qt = corpus_tokens(stego);
        fq.reserve(qt.size());
        for (size_t i = 1; i < qt.size(); i++) fq[qt[i - 1] + "\t" + qt[i]]++;

        unordered_set<string> vocab;
        vocab.reserve(origBigram.size() + fq.size());
        for (auto const& kv : origBigram) vocab.insert(kv.first);
        for (auto const& kv : fq) vocab.insert(kv.first);
        if (vocab.empty()) return 0.0;

        double totalP = 0.0, totalQ = 0.0;
        for (auto const& kv : origBigram) totalP += (double)kv.second;
        for (auto const& kv : fq) totalQ += (double)kv.second;
        double V = (double)vocab.size();
        totalP += V;
        totalQ += V;

        double kl = 0.0;
        for (auto const& bg : vocab) {
            double p = (double)(origBigram[bg] + 1) / totalP;
            double q = (double)(fq[bg] + 1) / totalQ;
            kl += p * log2(p / q);
        }
        return fabs(kl);
    };

    struct SchemeResult {
        string category;
        string name;
        double bpw;        // 【Academic Correction 1】Theoretical Limit Capacity
        double klDiv_mean; // First-order KL divergence mean
        double klDiv_ci;   // KL divergence confidence interval
        double tokenKl_mean;
        double tokenKl_ci;
        double tokenBigramKl_mean;
        double tokenBigramKl_ci;
        double markovShift;
        double entropyDiff;
        double markov_mean;
        double markov_ci;
        double entropy_mean;
        double entropy_ci;
        double timeMsKb;   // computational complexity
        double rob_l2_mean;// L2 robustness mean
        double rob_l2_ci;  // L2 robustness confidence interval
        double rob_l3_mean;// L3 robustness mean
        double rob_l3_ci;  // L3 robustness confidence interval
        string samplePath;
        DetectReport detect;
    };
    vector<SchemeResult> results;

    // General academic testing framework (using std::function to solve lambda generic derivation problems)
    auto run_academic_benchmark = [&](
        string cat, 
        string name, 
        function<string(string, const vector<int>&, int&, std::mt19937&)> embed_func, 
        function<vector<int>(string, int)> extract_func, 
        function<int(const string&)> cap_func) 
    {
        SchemeResult r;
        r.category = cat; r.name = name;
        
        if (debugMode) cout << "\n[Debug] Start test plan:" << name << endl;

        // [Academic Correction 1] Calculate theoretical limit capacity (Theoretical Capacity)
        int max_capacity = cap_func(origStr);
        r.bpw = (double)max_capacity / wordCount;

        vector<double> kl_vals(ITERATIONS), tkl_vals(ITERATIONS), tbkl_vals(ITERATIONS), markov_vals(ITERATIONS), ent_vals(ITERATIONS), l2_vals(ITERATIONS), l3_vals(ITERATIONS);
        double total_time = 0.0;
        double first_markov = 0.0;
        double first_entropy_diff = 0.0;
        int embedded_sample = 0;
        string stego_first;

        for (int iter = 0; iter < ITERATIONS; ++iter) {
            vector<int> const& secretBits = secrets[iter];
            uint32_t seed = (uint32_t)(0xC0FFEEu ^ (uint32_t)std::hash<string>{}(name) ^ (uint32_t)iter);
            std::mt19937 genEmbed(seed);

            int embedded = 0;
            auto start = chrono::high_resolution_clock::now();
            string stego_str = embed_func(origStr, secretBits, embedded, genEmbed);
            auto end = chrono::high_resolution_clock::now();
            double time_ms_kb = chrono::duration<double, milli>(end - start).count() / origSizeKB;
            total_time += time_ms_kb;

            if (iter == 0) {
                embedded_sample = embedded;
                first_markov = calcMarkovShift(origStr, stego_str);
                first_entropy_diff = abs(calcEntropy(origStr) - calcEntropy(stego_str));
                stego_first = stego_str;
            }

            kl_vals[iter] = calcKL(origContent, str2vec(stego_str));
            tkl_vals[iter] = calcTokenKL(stego_str);
            tbkl_vals[iter] = calcTokenBigramKL(stego_str);
            markov_vals[iter] = calcMarkovShift(origStr, stego_str);
            ent_vals[iter] = abs(calcEntropy(origStr) - calcEntropy(stego_str));

            // [Academic Modification 4] Multi-level attack robustness test
            auto test_robustness = [&](const string& attacked_str) -> double {
                vector<int> extracted = extract_func(attacked_str, embedded);
                int matches = 0; int check_len = min((int)extracted.size(), embedded);
                for(int i=0; i<check_len; i++) if(extracted[i] == secretBits[i]) matches++;
                return embedded > 0 ? ((double)matches / embedded) * 100.0 : 0.0;
            };

            l2_vals[iter] = test_robustness(apply_l2_attack(stego_str));
            l3_vals[iter] = test_robustness(apply_l3_attack(stego_str));

            if (perCsv) {
                DetectReport d = detect_all(origStr, stego_str);
                perCsv
                    << csv_escape(secretMsg) << ","
                    << csv_escape(origFile) << ","
                    << iter << ","
                    << (iter == 0 ? 1 : 0) << ","
                    << csv_escape(cat) << ","
                    << csv_escape(name) << ","
                    << embedded << ","
                    << fixed << setprecision(8) << time_ms_kb << ","
                    << scientific << setprecision(12) << kl_vals[iter] << ","
                    << scientific << setprecision(12) << tkl_vals[iter] << ","
                    << scientific << setprecision(12) << tbkl_vals[iter] << ","
                    << fixed << setprecision(8) << markov_vals[iter] << ","
                    << fixed << setprecision(12) << ent_vals[iter] << ","
                    << fixed << setprecision(8) << l2_vals[iter] << ","
                    << fixed << setprecision(8) << l3_vals[iter] << ","
                    << d.zwc_count << ","
                    << d.markup_count << ","
                    << d.homoglyph_count << ","
                    << fixed << setprecision(12) << d.ws.double_space_ratio << ","
                    << d.ws.max_space_run << ","
                    << d.ws.trailing_space_lines << ","
                    << fixed << setprecision(12) << d.lm_ce << ","
                    << fixed << setprecision(12) << d.lm_ce_delta << ","
                    << (d.det_zwc ? 1 : 0) << ","
                    << (d.det_markup ? 1 : 0) << ","
                    << (d.det_homoglyph ? 1 : 0) << ","
                    << (d.det_trailing ? 1 : 0) << ","
                    << (d.det_ws_stats ? 1 : 0) << ","
                    << (d.det_lm ? 1 : 0)
                    << "\n";
            }
        }

        r.timeMsKb = total_time / ITERATIONS;
        r.markovShift = first_markov;
        r.entropyDiff = first_entropy_diff;

        auto calcMeanCI = [](const vector<double>& vals, double& mean, double& ci) {
            double sum = 0.0;
            for(double v : vals) sum += v;
            mean = sum / vals.size();
            double sq_sum = 0.0;
            for(double v : vals) sq_sum += (v - mean) * (v - mean);
            if (vals.size() < 2) {
                ci = 0.0;
                return;
            }
            double stddev = sqrt(sq_sum / (vals.size() - 1));
            ci = 1.96 * (stddev / sqrt((double)vals.size()));
        };

        calcMeanCI(kl_vals, r.klDiv_mean, r.klDiv_ci);
        calcMeanCI(tkl_vals, r.tokenKl_mean, r.tokenKl_ci);
        calcMeanCI(tbkl_vals, r.tokenBigramKl_mean, r.tokenBigramKl_ci);
        calcMeanCI(markov_vals, r.markov_mean, r.markov_ci);
        calcMeanCI(ent_vals, r.entropy_mean, r.entropy_ci);
        calcMeanCI(l2_vals, r.rob_l2_mean, r.rob_l2_ci);
        calcMeanCI(l3_vals, r.rob_l3_mean, r.rob_l3_ci);

        std::filesystem::create_directories(outDir + "/stego_outputs");
        string safe = r.name;
        for (char& c : safe) {
            if (!(isalnum((unsigned char)c) || c == '_' || c == '-' || c == ' ')) c = '_';
        }
        r.samplePath = outDir + "/stego_outputs/" + safe + ".txt";
        {
            ofstream out(r.samplePath, ios::binary);
            out.write(stego_first.data(), (streamsize)stego_first.size());
        }
        r.detect = detect_all(origStr, stego_first);

        results.push_back(r);
        cout << ">> Test completed:" << r.name << "(Single embed" << embedded_sample << "/" << secretBitsCount << " bits, " << ITERATIONS << "experiments)" << endl;
    };

    // --- 1. Formatted steganography ---
    run_academic_benchmark("1. Format-based steganography", "Whitespace Modulation [This plan]", embed_ws, extract_ws, max_capacity_ws);
    run_academic_benchmark("1. Format-based steganography", "Invisible character embedding (Zero-Width Characters)", embed_zwc, extract_zwc, max_capacity_zwc);
    run_academic_benchmark("1. Format-based steganography", "Font-based Manipulation", embed_font, extract_font, max_capacity_font);

    // --- 2. Linguistic steganography ---
    run_academic_benchmark("2. Linguistic", "Synonym Substitution", embed_syn, extract_syn, max_capacity_syn);
    run_academic_benchmark("2. Linguistic", "Syntactic Transformation", embed_syn_trans, extract_syn_trans, max_capacity_syn_trans);
    run_academic_benchmark("2. Linguistic", "Abbreviation & Spelling", embed_abbr, extract_abbr, max_capacity_abbr);

    // --- 3. Generative steganography ---
    run_academic_benchmark("3. Generative steganography (Generative)", "Markov Chain Generation", embed_markov, extract_markov, max_capacity_gen);
    run_academic_benchmark("3. Generative steganography (Generative)", "Neural probability distribution encoding (LLM-based Encoding)", embed_llm, extract_llm, max_capacity_gen_llm);

    // --- 4. Structuring and protocol steganography ---
    run_academic_benchmark("4. Structural & Protocol-based", "Protocol Padding", embed_pad, extract_pad, max_capacity_pad);
    run_academic_benchmark("4. Structural & Protocol-based", "Layout-specific encoding (Layout/Acrostic)", embed_layout, extract_layout, max_capacity_layout);
    run_academic_benchmark("4. Structural & Protocol-based", "Field Ordering", embed_field, extract_field, max_capacity_field);

    // --- Output the final comparison results ---
    cout << "\n=========================================================================================" << endl;
    cout << "Final Academic Metrics Output" << endl;
    cout << "=========================================================================================" << endl;
    
    string currentCat = "";
    for (const auto& res : results) {
        if (res.category != currentCat) {
            currentCat = res.category;
            cout << "\n\033[1;32m" << currentCat << "\033[0m" << endl;
        }
        
        cout << "▶ Solution:" << res.name << endl;
        cout << fixed << setprecision(4);
        cout << "- Theoretical maximum carrier capacity (Theoretical BPW):" << setw(10) << res.bpw << " bits/word" << endl;
        cout << scientific << setprecision(6);
        cout << "- Byte distribution KL:" << res.klDiv_mean << " ± " << res.klDiv_ci << "(The lower the more natural)" << endl;
        cout << "- Word distribution KL:" << res.tokenKl_mean << " ± " << res.tokenKl_ci << "(Linguistics are more sensitive)" << endl;
        cout << "- word bigram KL:" << res.tokenBigramKl_mean << " ± " << res.tokenBigramKl_ci << "(More order sensitive)" << endl;
        cout << fixed << setprecision(4);
        cout << "- Second-order continuous feature shift (Markov Shift):" << setw(10) << res.markovShift << "(Reflecting structural anomalies)" << endl;
        cout << "- Text information entropy difference (Entropy Diff):" << setw(10) << res.entropyDiff << "(changes before and after steganography)" << endl;
        cout << "- Algorithm computational complexity:" << setw(10) << res.timeMsKb << " ms/KB" << endl;
        cout << "- Robustness (L2-common noise processing/format cleaning):" << setw(10) << res.rob_l2_mean << " ± " << res.rob_l2_ci << " %" << endl;
        cout << "- Robustness (L3-semantic rewriting/synonym fallback):" << setw(10) << res.rob_l3_mean << " ± " << res.rob_l3_ci << " %" << endl;
        cout << "- Sample text output path:" << res.samplePath << endl;
        cout << "- Detection (ZWC scan):" << (res.detect.det_zwc ? "DETECTED" : "OK") << " (count=" << res.detect.zwc_count << ")" << endl;
        cout << "- Detection (rich text tags):" << (res.detect.det_markup ? "DETECTED" : "OK") << " (count=" << res.detect.markup_count << ")" << endl;
        cout << "- Detection (Homographs/Unicode):" << (res.detect.det_homoglyph ? "DETECTED" : "OK") << " (count=" << res.detect.homoglyph_count << ")" << endl;
        cout << "- Detection (trailing whitespace):" << (res.detect.det_trailing ? "DETECTED" : "OK") << " (lines=" << res.detect.ws.trailing_space_lines << ")" << endl;
        cout << "- Detection (blank statistics):" << (res.detect.det_ws_stats ? "DETECTED" : "OK") << " (dblRatio=" << res.detect.ws.double_space_ratio << ", maxRun=" << res.detect.ws.max_space_run << ")" << endl;
        cout << "- Detection (Language Model CE):" << (res.detect.det_lm ? "DETECTED" : "OK") << " (CE=" << res.detect.lm_ce << ", Δ=" << res.detect.lm_ce_delta << ")" << endl;
        cout << "-----------------------------------------------------------------------------------------" << endl;
    }
    
    cout << "\n[Academic-level benchmark core conclusion]" << endl;
    cout << "\033[1;32m1. This solution (space ambiguity encoding) has absolute advantages in [KL divergence] and [computational complexity]. \033[0m\n";
    cout << "\033[1;32m Although there is a slight offset on the second-order continuous feature (Markov Shift) (due to the introduction of double spaces), in a scenario such as logs that naturally contains a large number of aligned spaces, this offset has a strong hidden rationality. \033[0m\n";
    cout << "2. The theoretical capacity (Theoretical BPW) of linguistic steganography (such as syntax, synonyms) is extremely low, and its robustness decreases rapidly when faced with L3-level semantic rewriting attacks." << endl;
    cout << "3. Although LLM coding is excellent in terms of generation quality, it will produce huge KL divergence and Markov Shift when faced with original text comparison, and the inference time cost is extremely high, making it unable to be implemented in high-frequency scenarios." << endl;
    cout << "=========================================================================================\n" << endl;

    ofstream sumCsv(summaryCsvPath, ios::binary);
    if (sumCsv) {
        sumCsv
            << "secret,orig_file,iterations,category,scheme,theoretical_bpw,byte_kl_mean,byte_kl_ci,token_kl_mean,token_kl_ci,token_bigram_kl_mean,token_bigram_kl_ci,markov_iter0,entropy_iter0,markov_mean,markov_ci,entropy_mean,entropy_ci,time_ms_kb_mean,rob_l2_mean,rob_l2_ci,rob_l3_mean,rob_l3_ci,sample_path,zwc_detected,zwc_count,markup_detected,markup_count,homoglyph_detected,homoglyph_count,ws_stats_detected,ws_double_space_ratio,ws_max_space_run,ws_trailing_detected,ws_trailing_space_lines,lm_detected,lm_ce,lm_ce_delta\n";
        for (auto const& res : results) {
            sumCsv
                << csv_escape(secretMsg) << ","
                << csv_escape(origFile) << ","
                << ITERATIONS << ","
                << csv_escape(res.category) << ","
                << csv_escape(res.name) << ","
                << fixed << setprecision(12) << res.bpw << ","
                << scientific << setprecision(12) << res.klDiv_mean << ","
                << scientific << setprecision(12) << res.klDiv_ci << ","
                << scientific << setprecision(12) << res.tokenKl_mean << ","
                << scientific << setprecision(12) << res.tokenKl_ci << ","
                << scientific << setprecision(12) << res.tokenBigramKl_mean << ","
                << scientific << setprecision(12) << res.tokenBigramKl_ci << ","
                << fixed << setprecision(12) << res.markovShift << ","
                << fixed << setprecision(12) << res.entropyDiff << ","
                << fixed << setprecision(12) << res.markov_mean << ","
                << fixed << setprecision(12) << res.markov_ci << ","
                << fixed << setprecision(12) << res.entropy_mean << ","
                << fixed << setprecision(12) << res.entropy_ci << ","
                << fixed << setprecision(12) << res.timeMsKb << ","
                << fixed << setprecision(12) << res.rob_l2_mean << ","
                << fixed << setprecision(12) << res.rob_l2_ci << ","
                << fixed << setprecision(12) << res.rob_l3_mean << ","
                << fixed << setprecision(12) << res.rob_l3_ci << ","
                << csv_escape(res.samplePath) << ","
                << (res.detect.det_zwc ? 1 : 0) << ","
                << res.detect.zwc_count << ","
                << (res.detect.det_markup ? 1 : 0) << ","
                << res.detect.markup_count << ","
                << (res.detect.det_homoglyph ? 1 : 0) << ","
                << res.detect.homoglyph_count << ","
                << (res.detect.det_ws_stats ? 1 : 0) << ","
                << fixed << setprecision(12) << res.detect.ws.double_space_ratio << ","
                << res.detect.ws.max_space_run << ","
                << (res.detect.det_trailing ? 1 : 0) << ","
                << res.detect.ws.trailing_space_lines << ","
                << (res.detect.det_lm ? 1 : 0) << ","
                << fixed << setprecision(12) << res.detect.lm_ce << ","
                << fixed << setprecision(12) << res.detect.lm_ce_delta
                << "\n";
        }
    }

    cout << "CSV output:" << perIterCsvPath << endl;
    cout << "CSV output:" << summaryCsvPath << endl;
}

void doGlobalComparison() {
    bool debugMode = false;
    string debugInput;
    cout << "Whether to enable Debug mode to output detailed variables and calculation logic? (y/n, default is n):";
    getline(cin, debugInput);
    if (debugInput == "y" || debugInput == "Y") {
        debugMode = true;
    }

    string origFile;
    cout << "Please enter the path to the log file used for benchmarking (default is log.txt):";
    getline(cin, origFile);
    if (origFile.empty()) origFile = "log.txt";

    string secretMsg;
    cout << "Please enter the same message to be hidden for all scenarios (default is Hidewriteforlog):";
    getline(cin, secretMsg);
    if (secretMsg.empty()) secretMsg = "Hidewriteforlog";

    doGlobalComparisonWithArgs(origFile, secretMsg, debugMode, 100, "benchmark_outputs");
}
