# Advanced Academic Steganography Benchmark Framework

![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20Docker-lightgrey.svg)
![Status](https://img.shields.io/badge/Status-Academic%20Release-success.svg)

## 📖 Abstract

This repository contains a highly rigorous, academically recognized **C++ benchmarking framework** for evaluating text-based steganography methods. It was explicitly developed to evaluate and compare a novel **Log-based Whitespace Steganography** method against 11 globally known, state-of-the-art steganography techniques. 

The framework evaluates methods across multiple crucial dimensions:

* **Statistical Invisibility:** KL divergence, Markov Shifts.
* **Capacity:** Bits Per Word (BPW).
* **Robustness:** Against semantic and format attacks.
* **Resistance:** To modern steganalysis scanners (ZWC detectors, language model cross-entropy, etc.).

This framework is built for reproducibility, enabling researchers to seamlessly re-run benchmarks and generate statistically significant 95% Confidence Intervals (CI) formatted directly for academic publication (CSV/LaTeX plotting).

---

## 🔬 Evaluated Steganography Domains & Algorithms

The benchmark categorizes text steganography into four primary domains, evaluating 11 distinct, academically verified channel models.

### 1. Format-based Steganography

* **Whitespace Modulation [Proposed Novel Method]:** Leverages naturally occurring space runs in structured log files. It models the distribution of double/triple spaces as a covert channel, making it extremely resistant to LM-based detection.
* **Zero-Width Characters:** Utilizes Unicode ZWJ (U+200D), ZWNJ (U+200C), and ZWNBSP (U+FEFF) to invisibly encode binary payloads between visible characters.
* **Font-based / Homoglyph Manipulation:** An academic channel model mapping Latin ASCII characters to visually identical Cyrillic/Greek homoglyphs (e.g., ASCII `a` vs. Cyrillic `a`).

### 2. Linguistic Steganography

* **Synonym Substitution:** Replaces target words with synonyms using parity-based splitting (e.g., odd-parity synonyms encode `1`, even-parity encode `0`).
* **Syntactic Transformation:** Alters the grammatical structure of sentences (e.g., converting active voice to passive voice) to encode data streams.
* **Abbreviation & Spelling:** Exploits regional spelling differences (e.g., "color" vs "colour") or abbreviation expansions (e.g., "ASAP" vs "as soon as possible").

### 3. Generative Steganography

* **Markov Chain Generation:** Generates text using N-gram probability splits (median/quartile probability routing) to encode bits during the generation process.
* **LLM-based Encoding:** Simulates Large Language Model (LLM) token probability distributions, utilizing arithmetic coding or Huffman tree routing over the softmax output to hide data.

### 4. Structural & Protocol-based Steganography

* **Padding:** Appends specific byte paddings (e.g., trailing spaces, null bytes) structurally at the end of lines or blocks.
* **Layout / Acrostic:** Encodes bits in the first letters of lines (Acrostic) or in specific layout positions within a document grid.
* **Field Ordering:** Uses AST-like Key-Value pair reordering (e.g., swapping JSON/Log fields) to encode mathematical permutations (Factorial encoding).

---

## 📊 Comprehensive Evaluation Metrics

To prevent mathematical anomalies (such as "0.0000" outputs for linguistic methods where byte-level changes are meaningless), this framework employs multi-level statistical metrics:

* **Capacity (Theoretical BPW - Bits Per Word):** Measures the maximum embedding rate. Higher is better, but usually trades off with invisibility.
* **Byte-level KL Divergence:** Measures the shift in raw byte/ASCII distribution. Crucial for detecting format-based anomalies.

$$
D_{KL}(P || Q) = \sum_{x \in \mathcal{X}} P(x) \log \left( \frac{P(x)}{Q(x)} \right)
$$

* **Token Unigram & Bigram KL Divergence:** Mathematically captures distribution shifts caused by word replacement and structural reordering. Linguistic and generative methods heavily disrupt these metrics.
* **2nd-Order Markov Shift:** Reflects abnormal structural continuity by analyzing transition probabilities between consecutive elements. 
* **Information Entropy Difference:** Calculates the overall Shannon entropy shift pre- and post-embedding to detect unexpected randomness (often caused by encrypted payloads).

$$
\Delta H = H_{stego} - H_{cover}
$$

* **Robustness Testing (L2 & L3 Attacks):**
  * **L2 Attack (Format Cleaning):** Simulates channel noise like trailing space trimming, Unicode normalization (NFKC), and multiple-space collapse.
  * **L3 Attack (Semantic Rewriting):** Simulates advanced active wardens applying synonym fallback, paraphrasing, and AST reconstruction.

---

## 🛡️ Integrated Steganalysis Scanners (Active Wardens)

The framework runs all generated stego-texts through 6 built-in detection scanners to evaluate real-world survivability:

1. **ZWC Scanner:** Detects zero-width character injection anomalies via regex and Unicode block analysis.
2. **Markup/Rich Text Scanner:** Identifies injected HTML/XML tags used for fake formatting.
3. **Homoglyph/Unicode Scanner:** Detects mixed-script attacks (e.g., Cyrillic injected into Latin text blocks).
4. **Trailing Space Scanner:** Identifies unnatural trailing whitespace runs at line endings.
5. **Whitespace Statistics Scanner:** Analyzes double-space ratios and maximum space run lengths against natural corpus baselines.
6. **Language Model Cross-Entropy (LM-CE):** Evaluates semantic perplexity. Texts with forced synonyms or generative anomalies will exhibit spikes in Cross-Entropy.

---

## 🏛️ Academic Integrity Declarations

To ensure strict academic rigor, this benchmark implements the following design constraints:
1. **Zero Data Faking:** The framework will statically check the capacity of your input log file. If the capacity is insufficient to embed the required bits across *all* schemes, it will hard-fail rather than artificially duplicating or faking input text.
2. **LLM Evaluation Honesty:** To maintain an agile CI/CD pipeline and reasonable repository size, the *Generative LLM-based Encoding* scheme uses equivalent FLOPs matrix multiplications to accurately simulate inference delays, rather than loading a multi-gigabyte neural network. All entropy, KL divergence, and robustness metrics remain mathematically accurate to the algorithm's output.

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
├── bin/                    # Compiled Executables
├── build.bat               # Windows build script (MSVC)
├── build.sh                # Linux/POSIX build script (g++)
├── Makefile                # Standard Makefile for Linux
├── Dockerfile              # Docker container definition
└── LICENSE                 # Strict GPL v3.0 License
```

---

## 🚀 Getting Started

### Prerequisites

* **Windows OS:** Microsoft Visual Studio (MSVC) with C++17 support.
* **Linux / macOS:** `g++` (GCC) or `clang++` with C++17 support.
* **Docker:** If you prefer not to compile it locally.

### 1. Build the Framework

**Windows:**

Run the provided build script from the project root in your terminal/PowerShell:

```bat
.\build.bat
```

*This will configure the MSVC environment, compile `src/advanced_main.cpp`, and output the executable to `bin/advanced_main.exe`.*

**Linux / macOS:**

We provide a standard `Makefile` and a bash script. You can build it using `g++` (C++17 required):

```bash
make
# OR
bash build.sh
```

*This will compile the source code and output the executable to `bin/advanced_main`.*

### 2. Run the Academic Benchmark

The executable accepts standard command-line arguments to facilitate automated batch runs and CI generation.

**On Windows:**

```bat
.\bin\advanced_main.exe --option 6 --file data\log.txt --secret "YourSecretMessage" --iterations 100 --outdir results
```

**On Linux / macOS:**

```bash
./bin/advanced_main --option 6 --file data/log.txt --secret "YourSecretMessage" --iterations 100 --outdir results
```

---

## 🐳 Docker Support (One-Click Deploy - Foolproof)

To guarantee 100% reproducibility across any OS (including Windows WSL2, macOS, Linux) without dealing with C++ compilers or Geth installations, we provide a fully automated Docker container.

### 1. Build the Docker Image

```bash
docker build -t stego-benchmark .
```

### 2. Run the Academic Benchmark

Run the container to automatically execute the 11-algorithm benchmark. It will output to the `results` folder.

```bash
docker run --rm -v $(pwd)/results:/app/results stego-benchmark
```

### 3. Run the Geth Blockchain Simulation (Active Warden Test)

To run the live blockchain simulation (where `geth` feeds logs to the steganography engine in real-time), simply append `simulate` to the command:

```bash
docker run --rm -it -v $(pwd)/results:/app/results stego-benchmark simulate
```
*(Note: `-it` is required for interactive input of the secret message).*

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


---

## ⚖️ License & Usage Restrictions

**STRICT LICENSE WARNING: GNU General Public License v3.0 (GPL-3.0)**

This project is licensed under the strict **GNU GPLv3 License**. 

* **No MIT License:** This codebase is explicitly NOT licensed under MIT, Apache, or BSD permissive licenses.
* **Copyleft Provision:** Any derivative works, modifications, or larger projects that incorporate this code **must** also be open-sourced under the exact same GPLv3 license.
* **Commercial Use:** Commercial exploitation, closed-source integration, or proprietary redistribution of this framework is strictly prohibited without explicit dual-licensing permission from the original authors.

See the full [LICENSE](LICENSE) file for exact legal details.
