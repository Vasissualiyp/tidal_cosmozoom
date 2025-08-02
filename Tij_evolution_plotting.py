import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
    
def get_param_from_fname_string(list_of_splits, param_name) -> str:
    value = 0
    for split in list_of_splits:
        if split[0:len(param_name)] == param_name:
            value = split[len(param_name):]
            break
    return value

def get_df_of_run_files(tables_dir="tables"):
    files = sorted([ f for f in os.listdir(tables_dir) if os.path.isfile(os.path.join(tables_dir, f)) ])
    
    n_vals = []
    padding_vals = []
    seed_vals = []
    file_paths = []
    for file in files:
        splits = file.split("_")[1:] # Discratd "Tij_" split
        n = int(get_param_from_fname_string(splits, "n"))
        padding = get_param_from_fname_string(splits, "padding")
        if padding == "Full":
            padding = -1
        else:
            padding = int(padding)
        seed = int(get_param_from_fname_string(splits, "seed").split(".")[0])
        n_vals.append(n)
        seed_vals.append(seed)
        padding_vals.append(padding)
        file_paths.append(os.path.join(tables_dir, file))
    list_of_columns=      ["n",    "padding",    "seed",    "filepath"] 
    full_data_list = zip(*[ n_vals, padding_vals, seed_vals, file_paths ])
    return pd.DataFrame(full_data_list, columns=list_of_columns)

# Run parameters that we care about
n = 128
padding = 0 # -1 for Full padding

df = get_df_of_run_files()
df_of_interest = df[(df.n == n) & (df.padding == padding)]

fig, axs = plt.subplots(ncols=ncols, nrows=nrows, figsize=figsize,
                        layout="constrained")

# Assume df_of_interest.filepath is your list of file paths
# Get Tij columns from the first file (assuming all files have the same structure)
first_file_df = pd.read_csv(df_of_interest.filepath[0])
Tij_list = [c for c in first_file_df.columns if c[0] == "T"]

# Define subplot grid parameters (adjust ncols as needed)
ncols = 3  # Example: adjust based on number of Tij columns
nrows = (len(Tij_list) + ncols - 1) // ncols  # Calculate rows needed
subplotsize=4
figsize = (ncols+1)*subplotsize, (nrows+1)*subplotsize
fig, axs = plt.subplots(nrows, ncols, figsize=figsize, sharex=True)
axs = np.array(axs).reshape(nrows, ncols)  # Ensure axs is 2D even if nrows=1
# Dictionary to store Tij_ratio data for all files
tij_ratio_data = {Tij: [] for Tij in Tij_list}

# Step 1: Collect Tij_ratio from all files
for file in df_of_interest.filepath:
    file_df = pd.read_csv(file)
    for Tij in Tij_list:
        Tij_ratio = file_df[Tij] / file_df[Tij][0]
        tij_ratio_data[Tij].append(Tij_ratio)

# Step 2 & 3: Compute statistics and plot for each Tij
for i, Tij in enumerate(Tij_list):
    # Concatenate all Tij_ratio series into a DataFrame (rows: n_eff, cols: files)
    tij_ratio_df = pd.concat(tij_ratio_data[Tij], axis=1)
    
    # Calculate mean and standard deviation across files for each n_eff
    mean_ratio = tij_ratio_df.mean(axis=1)
    std_ratio = tij_ratio_df.std(axis=1)
    
    # Get n_eff from the first file (same for all files)
    n_eff = first_file_df.n_eff
    
    # Assign subplot position
    col = i // ncols
    row = i % ncols
    subplot = axs[col, row]
    
    # Plot mean line
    subplot.plot(n_eff, mean_ratio, label='Mean', color='blue')
    # Plot shaded region for mean ± standard deviation
    subplot.fill_between(n_eff, mean_ratio - std_ratio, mean_ratio + std_ratio, 
                         alpha=0.3, color='blue', label='$\pm 1\sigma$')
    # Add reference line at y=1
    subplot.plot(n_eff, np.ones(len(n_eff)), 'k--', alpha=0.5)
    
    # Customize subplot
    subplot.set_title(f"${Tij}/{Tij}_0$")
    subplot.set_xlabel('n_eff')
    subplot.set_ylabel('Ratio')
    subplot.legend()

# Remove empty subplots if any
for i in range(len(Tij_list), nrows * ncols):
    col = i // ncols
    row = i % ncols
    fig.delaxes(axs[col, row])

# Adjust layout and display
plt.tight_layout()
plt.show()
