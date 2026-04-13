import os
import sys
import subprocess
import threading
import time
import json
import shutil
import re
import signal

# Configuration
GENESIS_FILE = "genesis.json"
DATA_DIR = "chain_data"
PASSWORD_FILE = "password.txt"
GETH_IPC = r"\\.\pipe\geth_simulation.ipc"
CHAIN_ID = 12345
# C++ Program Name
CPP_EXE = "advanced_main.exe"

def cleanup_previous_run():
    """Removes previous data directories to ensure a clean start."""
    if os.path.exists(DATA_DIR):
        print(f"[Cleanup] Removing existing data directory: {DATA_DIR}")
        try:
            shutil.rmtree(DATA_DIR)
        except Exception as e:
            print(f"[Cleanup] Warning: Failed to remove {DATA_DIR}: {e}")

    if os.path.exists(GENESIS_FILE):
        try:
            os.remove(GENESIS_FILE)
        except:
            pass
        
    if os.path.exists(PASSWORD_FILE):
        try:
            os.remove(PASSWORD_FILE)
        except:
            pass

def create_password_file():
    """Creates a password file for the account."""
    with open(PASSWORD_FILE, 'w') as f:
        f.write("") # Empty password for simulation convenience
    print(f"[Setup] Created password file: {PASSWORD_FILE}")

def create_account():
    """Creates a new Geth account and returns the address."""
    print("[Setup] Creating new account for mining/signing...")
    
    if not os.path.exists(DATA_DIR):
        os.makedirs(DATA_DIR)

    # Use explicit geth path if needed, assume 'geth' in PATH
    cmd = [
        'geth',
        'account', 'new',
        '--datadir', DATA_DIR,
        '--password', PASSWORD_FILE
    ]
    
    try:
        # Run command and capture output
        # Using encoding='utf-8' and errors='ignore' to be safe
        result = subprocess.run(
            cmd, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.STDOUT, 
            encoding='utf-8',
            errors='ignore',
            check=True
        )
        
        # Parse output for address
        # Output format usually contains: "Public address of the key:   0x..."
        match = re.search(r"Public address of the key:\s+(0x[a-fA-F0-9]{40})", result.stdout)
        if match:
            address = match.group(1)
            print(f"[Setup] Account created: {address}")
            return address
        else:
            print(f"[Error] Could not parse address from geth output:\n{result.stdout}")
            return None
            
    except subprocess.CalledProcessError as e:
        print(f"[Error] Failed to create account:\n{e.output}")
        return None

def create_genesis(signer_address):
    """Creates a genesis block configuration for Clique PoA."""
    print(f"[Setup] Configuring genesis block with signer: {signer_address}")
    
    # Strip 0x prefix for extraData
    clean_address = signer_address.replace("0x", "").lower()
    
    # Construct extraData for Clique:
    # 32 bytes vanity (64 hex chars) + 
    # Signer address (40 hex chars) + 
    # 65 bytes signature (130 hex chars)
    extra_data = "0x" + ("0" * 64) + clean_address + ("0" * 130)
    
    genesis = {
        "config": {
            "chainId": CHAIN_ID,
            "homesteadBlock": 0,
            "eip150Block": 0,
            "eip155Block": 0,
            "eip158Block": 0,
            "byzantiumBlock": 0,
            "constantinopleBlock": 0,
            "petersburgBlock": 0,
            "istanbulBlock": 0,
            "clique": {
                "period": 5,
                "epoch": 30000
            }
        },
        "difficulty": "1",
        "gasLimit": "8000000",
        "extradata": extra_data,
        "alloc": {
            signer_address: { "balance": "1000000000000000000000" } # Pre-fund the signer
        }
    }

    with open(GENESIS_FILE, 'w') as f:
        json.dump(genesis, f, indent=4)
    print(f"[Setup] Genesis file created at {GENESIS_FILE}")

def init_geth():
    """Initializes Geth with the genesis block."""
    print("[Setup] Initializing Geth node...")
    cmd = f"geth --datadir {DATA_DIR} init {GENESIS_FILE}"
    try:
        subprocess.run(cmd, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("[Setup] Geth initialized successfully.")
        return True
    except subprocess.CalledProcessError as e:
        print(f"[Error] Geth init failed:\n{e.stderr.decode('utf-8', errors='ignore')}")
        return False

def stream_reader(pipe_in, pipe_out, stop_event):
    """Reads from pipe_in (Geth) and writes to pipe_out (Advanced_Main)."""
    try:
        while not stop_event.is_set():
            # Read line from Geth
            line = pipe_in.readline()
            if not line:
                break
            
            # Write to Advanced_Main
            try:
                if isinstance(line, str):
                    pipe_out.write(line.encode('utf-8'))
                else:
                    pipe_out.write(line)
                pipe_out.flush()
                
                # Optional: print raw log to console for debugging
                # print(f"[Raw] {line.strip()}")
            except Exception as e:
                # Pipe might be broken if C++ app closes
                break
    except Exception as e:
        pass

def main():
    print("=== Blockchain Simulation & Steganography Environment (Final Fix) ===")
    
    # --- Check for C++ Executable ---
    if not os.path.exists(CPP_EXE):
        print(f"[Error] '{CPP_EXE}' not found!")
        print("Please compile 'advanced_main.cpp' first.")
        print("Example command: g++ advanced_main.cpp -o advanced_main.exe")
        return

    # --- Setup Phase ---
    # Always try to setup correctly if something is missing
    setup_needed = False
    if not os.path.exists(os.path.join(DATA_DIR, "geth")):
        setup_needed = True
    else:
        # Check if we can get an account
        try:
             cmd = ['geth', 'account', 'list', '--datadir', DATA_DIR]
             result = subprocess.run(cmd, stdout=subprocess.PIPE, encoding='utf-8', errors='ignore')
             if "Account #0" not in result.stdout:
                 setup_needed = True
        except:
             setup_needed = True

    if setup_needed:
        cleanup_previous_run()
        create_password_file()
        signer_address = create_account()
        if not signer_address:
            print("[Fatal] Could not create account. Exiting.")
            return
        
        create_genesis(signer_address)
        if not init_geth():
            print("[Fatal] Could not initialize Geth. Exiting.")
            return
    else:
        print("[Setup] Existing chain data found.")
        create_password_file()
        # Retrieve existing account
        try:
            cmd = ['geth', 'account', 'list', '--datadir', DATA_DIR]
            result = subprocess.run(cmd, stdout=subprocess.PIPE, encoding='utf-8', errors='ignore')
            match = re.search(r"Account #0: \{([a-fA-F0-9]{40})\}", result.stdout)
            if match:
                signer_address = "0x" + match.group(1)
                print(f"[Setup] Using existing account: {signer_address}")
            else:
                print("[Error] Could not find existing account. Re-running setup...")
                cleanup_previous_run()
                main()
                return
        except Exception:
            cleanup_previous_run()
            main()
            return

    # --- Configuration Phase ---
    print("\n--- Configuration ---")
    print("Select Log Level (Controls generation speed and detail):")
    print("1. Lightweight (Verbosity 3 - INFO)")
    print("2. Debug (Verbosity 4 - DEBUG)")
    
    level_choice = input("Choice (1/2): ").strip()
    verbosity = 3
    if level_choice == '2':
        verbosity = 4
    
    # Note: Secret message is now configured inside C++, not here.
    print("[Info] Secret message is configured inside advanced_main.cpp (Stream Mode).")

    # --- Execution Phase ---
    print("\n[System] Starting processes...")

    # 1. Start Advanced_Main
    # We use bufsize=0 to ensure unbuffered piping
    # stdout=None allows C++ output to go directly to console for user interaction
    cpp_process = subprocess.Popen(
        [CPP_EXE],
        stdin=subprocess.PIPE,
        stdout=None, 
        stderr=None,
        bufsize=0 
    )

    try:
        print("[System] Configuring Steganography Engine...")
        # Send Mode 3 -> Just "3\n"
        cpp_process.stdin.write(b"3\n")
        cpp_process.stdin.flush()
        
        # User will be prompted by C++ program via CONIN$
        print("[System] Please enter the secret message in the C++ prompt above.")
        
        print("[System] Steganography Engine Ready.")
    except Exception as e:
        print(f"[Error] Failed to configure C++ engine: {e}")
        return

    # 2. Start Geth
    geth_cmd = [
        'geth',
        '--datadir', DATA_DIR,
        '--networkid', str(CHAIN_ID),
        '--nodiscover',
        '--mine',
        '--miner.threads', '1',
        '--miner.etherbase', signer_address,
        '--unlock', signer_address,
        '--password', PASSWORD_FILE,
        '--allow-insecure-unlock',
        '--verbosity', str(verbosity),
        '--ipcpath', GETH_IPC
    ]
    
    print(f"[System] Launching Geth (Mining enabled, Verbosity: {verbosity})...")
    geth_process = subprocess.Popen(
        geth_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT, # Merge stderr to stdout
        bufsize=0 
    )

    # Check for Geth startup success (brief wait)
    time.sleep(2)
    if geth_process.poll() is None:
        print("\n[System] Geth Started Successfully!\n")
    else:
        print("\n[Error] Geth failed to start immediately.\n")

    # 3. Connect Pipes
    stop_event = threading.Event()
    io_thread = threading.Thread(target=stream_reader, args=(geth_process.stdout, cpp_process.stdin, stop_event))
    io_thread.daemon = True
    io_thread.start()

    print("\n" + "="*40)
    print("SIMULATION RUNNING")
    print(f"Log -> advanced_main -> hidden_log.txt")
    print("Press ENTER to stop the simulation...")
    print("="*40 + "\n")

    try:
        input() 
    except KeyboardInterrupt:
        pass

    # --- Cleanup Phase ---
    print("\n[System] Stopping simulation...")
    stop_event.set()
    
    # Kill Geth
    geth_process.terminate()
    try:
        geth_process.wait(timeout=3)
    except:
        geth_process.kill()
        
    # Close C++ stdin to signal EOF
    try:
        cpp_process.stdin.close()
    except:
        pass
    
    # Wait for C++ to finish writing
    try:
        cpp_process.wait(timeout=5)
    except:
        cpp_process.kill()

    print("[System] Simulation stopped. Output saved to hidden_log.txt.")

if __name__ == "__main__":
    main()
