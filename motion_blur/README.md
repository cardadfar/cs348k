# Motion Blur  

## Usage 
Load plugin into AE. Open desired video as a layer in a composition. Search for "Directional Motion Blur" under Sample Plugins and apply to layer. Effects should pop up along with sliders for tuning parameters.  

## Benchmarking 

Checkout benchmarking scripts at specific commits using `git checkout 5ca6fd`, `git checkout 4277e6` and `git checkout da3004`. 

- Commit 4277e6: Computes timings for first/second pass of original shader 
- Commit 5ca6fd: Computes timings for combined pass of optimized shader. For num_Lines = 5, 10 
- Commit 5ca6fd: Computes timings for combined pass of optimized shader. For num_Lines = 0

Run the benchmarking script by launching After Effects from a terminal. Apply the Motion Blur plugin and log(s) should pop up. 
For original shader, two logs should show up: 'plugin_benchmark_2.log' and 'plugin_benchmark_1.log' for each pass. 
For optimized shader, one log should show up: 'plugin_benchmark_2.log' for the combined pass. 

Use the python script `benchmark_summing_times.py` for summing up the runtimes from the logs. Uncomment Example 1 and Example 2 accordingly depending on whether you are calculating runtime for original shader or optimized shader. 



