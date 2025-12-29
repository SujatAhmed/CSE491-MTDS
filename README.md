# MTDS Project

## Build the binaries (CMake + Ninja)

This project uses CMake with the Ninja generator to build the `MTDS` binary.

1. Create a build directory and configure the project:
   ```bash
   mkdir build 
   cd build
   cmake -G Ninja ..
   ```
2. Build the binary (Go to the build directory if you haven't already):
   ```bash
   ninja
   ```
3. Create the syng binary
   ```bash
   cd synGgen
   g++ synGgen.cpp -o syng

The resulting executable will be at `build/MTDS` and `synGgen/syng`

## Run experiments (Python)

The experiment scripts invoke the built `MTDS` binary and the SynGGen generator.

1. Install Python dependencies:
   ```bash
   python -m pip install -r requirements.txt
   ```
2. Update the `BASE` path in the experiment scripts to point to your local repo
   path (for example, `/absolute/path/to/CSE491-MTDS`).
   * `experiment2.py`
3. Ensure the MTDS binary exists at `build/MTDS` and SynGGen is available at
   `synGgen/syng` (the scripts derive both paths from `BASE`).

4. Generate experiments:
   - Change the parameters in `generate_experiments.py` to create certain combinations of experiments
   - ```bash
     pyhton generate_experiments.py
     ```
   - The script produces an `experiments.txt` file which the other scripts can read
5. Run the experiments:

   
   ```bash
   python experiment2.py
   ```

The scripts read experiment configurations from `experiments.txt`.
