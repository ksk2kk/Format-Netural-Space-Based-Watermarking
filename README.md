# Advanced Academic Steganography Benchmark Framework

![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![Status](https://img.shields.io/badge/Status-Academic%20Release-success.svg)

## 📖 Abstract

This repository contains a highly rigorous, academically recognized C++ benchmarking framework for evaluating text-based steganography methods. It was explicitly developed to evaluate and compare a novel **Log-based Whitespace Steganography** method against 11 globally known, state-of-the-art steganography techniques. 

The framework evaluates methods across multiple crucial dimensions, including **statistical invisibility** (KL divergence, Markov Shifts), **capacity** (Bits Per Word), **robustness** against semantic and format attacks, and **resistance to modern steganalysis scanners** (ZWC detectors, language model cross-entropy, etc.).

This framework is built for reproducibility, enabling researchers to seamlessly re-run benchmarks and generate statistically significant 95% Confidence Intervals (CI) formatted directly for academic publication (CSV/LaTeX plotting).

---

## 🔬 Evaluated Steganography Domains & Algorithms

The benchmark categorizes text steganography into four primary domains, evaluating 11 distinct, academically verified channel models.

### 1. Format-based Steganography (格式化隐写)
Format-based methods alter the visual presentation or invisible encoding of the text without changing its semantic meaning.
*   **Whitespace Modulation (空格歧义编码) [Proposed Novel Method]**: Leverages naturally occurring space runs in structured log files. It models the distribution of double/triple spaces as a covert channel, making it extremely resistant to LM-based detection.
*   **Zero-Width Characters (不可见字符嵌入)**: Utilizes Unicode ZWJ (U+200D), ZWNJ (U+200C), and ZWNBSP (U+FEFF) to invisibly encode binary payloads between visible characters.
*   **Font-based / Homoglyph Manipulation (字体特征微调)**: An academic channel model mapping Latin ASCII characters to visually identical Cyrillic/Greek homoglyphs (e.g., ASCII `a` vs. Cyrillic `а`).

### 2. Linguistic Steganography (语言学隐写)
Linguistic methods manipulate the syntax or lexicon of the text, maintaining the original semantic intent.
*   **Synonym Substitution (同义词替换)**: Replaces target words with synonyms using parity-based splitting (e.g., odd-parity synonyms encode `1`, even-parity encode `0`).
*   **Syntactic Transformation (句法转换)**: Alters the grammatical structure of sentences (e.g., converting active voice to passive voice) to encode data streams.
*   **Abbreviation & Spelling (拼写与缩写变体)**: Exploits regional spelling differences (e.g., "color" vs "colour") or abbreviation expansions (e.g., "ASAP" vs "as soon as possible").

### 3. Generative Steganography (生成式隐写)
Generative methods construct entirely new text from scratch or heavily modify existing text using probabilistic models.
*   **Markov Chain Generation (马尔可夫链生成)**: Generates text using N-gram probability splits (median/quartile probability routing) to encode bits during the generation process.
*   **LLM-based Encoding (神经概率分布编码)**: Simulates Large Language Model (LLM) token probability distributions, utilizing arithmetic coding or Huffman tree routing over the softmax output to hide data.

### 4. Structural & Protocol-based Steganography (结构化与协议隐写)
These methods exploit the rigid structure of specific file formats (like JSON, XML, or system logs).
*   **Padding (协议补白)**: Appends specific byte paddings (e.g., trailing spaces, null bytes) structurally at the end of lines or blocks.
*   **Layout / Acrostic (特定布局编码)**: Encodes bits in the first letters of lines (Acrostic) or in specific layout positions within a document grid.
*   **Field Ordering (字段顺序扰动)**: Uses AST-like Key-Value pair reordering (e.g., swapping JSON/Log fields) to encode mathematical permutations (Factorial encoding).

---

## 📊 Comprehensive Evaluation Metrics

To prevent mathematical anomalies (such as "0.0000" outputs for linguistic methods where byte-level changes are meaningless), this framework employs multi-level statistical metrics:

1.  **Capacity (Theoretical BPW - Bits Per Word)**: 
    *   Measures the maximum embedding rate. Higher is better, but usually trades off with invisibility.
2.  **Byte-level KL Divergence ($D_{KL}(P || Q)$)**: 
    *   Measures the shift in raw byte/ASCII distribution. Crucial for detecting format-based anomalies.
3.  **Token Unigram & Bigram KL Divergence**: 
    *   Mathematically captures distribution shifts caused by word replacement and structural reordering. Linguistic and generative methods heavily disrupt these metrics.
4.  **2nd-Order Markov Shift**: 
    *   Reflects abnormal structural continuity by analyzing transition probabilities between consecutive elements. 
5.  **Information Entropy Difference ($\Delta H$)**: 
    *   Calculates the overall Shannon entropy shift pre- and post-embedding to detect unexpected randomness (often caused by encrypted payloads).
6.  **Robustness Testing (L2 & L3 Attacks)**:
    *   **L2 Attack (Format Cleaning)**: Simulates channel noise like trailing space trimming, Unicode normalization (NFKC), and multiple-space collapse.
    *   **L3 Attack (Semantic Rewriting)**: Simulates advanced active wardens applying synonym fallback, paraphrasing, and AST reconstruction.

---

## 🛡️ Integrated Steganalysis Scanners (Active Wardens)

The framework runs all generated stego-texts through 6 built-in detection scanners to evaluate real-world survivability:

1.  **ZWC Scanner**: Detects zero-width character injection anomalies via regex and Unicode block analysis.
2.  **Markup/Rich Text Scanner**: Identifies injected HTML/XML tags used for fake formatting.
3.  **Homoglyph/Unicode Scanner**: Detects mixed-script attacks (e.g., Cyrillic injected into Latin text blocks).
4.  **Trailing Space Scanner**: Identifies unnatural trailing whitespace runs at line endings.
5.  **Whitespace Statistics Scanner**: Analyzes double-space ratios and maximum space run lengths against natural corpus baselines.
6.  **Language Model Cross-Entropy (LM-CE)**: Evaluates semantic perplexity. Texts with forced synonyms or generative anomalies will exhibit spikes in Cross-Entropy ($\Delta CE > Threshold$).

---

## 📂 Project Architecture

```text
├── src/                    # Core C++ Source Code
│   ├── advanced_main.cpp   # CLI entry point and argument parsing
│   ├── compare_module.cpp  # Implementation of 11 algorithms & 6 scanners
│   └── main.cpp            # Legacy entry point
├── scripts/                # Auxiliary Python and Batch scripts
│   ├── simulation.py       # Python simulation & validation scripts
│   ├── chi_square_rise.py  # Python plotting scripts for paper figures
│   └── build.bat           # Internal build references
├── data/                   # Input datasets (corpus, logs, secrets)
│   ├── log.txt             # Primary log corpus
│   └── genesis.json        # Auxiliary structural data
├── docs/                   # Academic papers, PDF reports, and analysis
├── results/                # Output directory (Auto-generated)
│   ├── stego_outputs/      # Raw .txt outputs of all 11 algorithms (Iteration 0)
│   ├── benchmark_per_iter.csv # Raw data for every single iteration
│   └── benchmark_summary.csv  # Aggregated Means and 95% CIs
├── bin/                    # Compiled Windows Executables (.exe)
├── build.bat               # Main Windows build script (MSVC)
└── LICENSE                 # Strict GPL v3.0 License
```

---

## 🚀 Getting Started

### Prerequisites
*   Windows OS (Linux support via CMake can be easily added)
*   Microsoft Visual Studio (MSVC) with C++17 support or MinGW/GCC.

### 1. Build the Framework
Run the provided build script from the project root in your terminal/PowerShell:
```bat
.\build.bat
```
*This will configure the MSVC environment, compile `src/advanced_main.cpp`, and output the executable to `bin/advanced_main.exe`.*

### 2. Run the Academic Benchmark
The executable accepts standard command-line arguments to facilitate automated batch runs and CI generation.

```bat
.\bin\advanced_main.exe --option 6 --file data\log.txt --secret "YourSecretMessage" --iterations 100 --outdir results
```

**Arguments Explanation:**
*   `--option 6`: Triggers the comprehensive 11-algorithm benchmark suite.
*   `--file`: Path to the cover text/log corpus (e.g., `data\log.txt`).
*   `--secret`: The secret message to embed across all algorithms.
*   `--iterations`: Number of times to run the embedding (with randomized seeds after Iteration 0) to generate statistically sound means and 95% Confidence Intervals.
*   `--outdir`: Directory to save the output CSVs and stego-texts.

### 3. Interpreting the Output
*   **`results/stego_outputs/`**: Contains the physical `.txt` files generated by each algorithm during Iteration 0 (using the exact same secret). Useful for manual inspection or passing to external 3rd-party steganalysis tools.
*   **`results/benchmark_summary.csv`**: Contains the aggregated academic metrics (Mean ± 95% CI). You can directly import this CSV into Python (Matplotlib/Seaborn) or LaTeX/pgfplots to generate your paper's comparison charts.

---

## 🧩 Extensibility: Adding a New Algorithm

To add a new steganography method to the benchmark:
1. Open `src/compare_module.cpp`.
2. Implement your `embed_custom` and `extract_custom` functions.
3. Define the theoretical capacity `max_capacity_custom`.
4. Register it in the `run_academic_benchmark` pipeline at the bottom of the file:
   ```cpp
   run_academic_benchmark("Category Name", "Your Algorithm Name", embed_custom, extract_custom, max_capacity_custom);
   ```
5. Recompile and run. The framework will automatically calculate KL Divergence, Markov Shifts, Entropy, Robustness, and run it against all 6 steganalysis scanners.

---

## 📝 Citation

If you use this framework in your academic research, please cite our paper:

```bibtex
@article{LogStego2026,
  title={Evaluating Log-based Steganography: A Comprehensive Benchmark Framework},
  author={[Your Name / Authors]},
  journal={[Target Journal / Conference]},
  year={2026},
  publisher={[Publisher]}
}
```

---

## ⚖️ License & Usage Restrictions

**STRICT LICENSE WARNING: GNU General Public License v3.0 (GPL-3.0)**

This project is licensed under the strict **GNU GPLv3 License**. 
*   **No MIT License**: This codebase is explicitly NOT licensed under MIT, Apache, or BSD permissive licenses.
*   **Copyleft Provision**: Any derivative works, modifications, or larger projects that incorporate this code **must** also be open-sourced under the exact same GPLv3 license.
*   **Commercial Use**: Commercial exploitation, closed-source integration, or proprietary redistribution of this framework is strictly prohibited without explicit dual-licensing permission from the original authors.

See the full [LICENSE](LICENSE) file for exact legal details.
   
 