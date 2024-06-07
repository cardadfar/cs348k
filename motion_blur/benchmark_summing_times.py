def read_log_file(file_path):
    """Read a log file and return a list of its lines as float values."""
    with open(file_path, 'r') as file:
        lines = file.readlines()
    return [float(line.strip()) for line in lines]

def sum_logs(file1, file2):
    """Sum all values from two pass log files."""
    # Read the log files
    log1 = read_log_file(file1)
    log2 = read_log_file(file2)
    
    # Sum all values from both log files
    total_sum = sum(log1) + sum(log2)
    
    return total_sum

def sum_log(file1):
    """Sum all values from one singe pass log files."""
    # Read the log files
    log1 = read_log_file(file1)
    # Sum all values from both log files
    total_sum = sum(log1)
    
    return total_sum

# Example usage
file1 = 'logs/plugin_benchmark_2_opt_10_good.log'
#file2 = 'logs/plugin_benchmark_1_saved_10.log'
#total_sum = sum_logs(file1, file2)
total_sum = sum_log(file1)
print(f"Total sum: {total_sum}")

