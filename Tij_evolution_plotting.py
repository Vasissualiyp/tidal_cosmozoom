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
    boxsizes = []
    seed_vals = []
    file_paths = []
    for file in files:
        splits = file.split("_")[1:] # Discratd "Tij_" split
        n = int(get_param_from_fname_string(splits, "n"))
        boxsize = float(get_param_from_fname_string(splits, "boxsize"))
        padding = get_param_from_fname_string(splits, "padding")
        if padding == "Full":
            padding = -1
        else:
            padding = int(padding)
        seed = int(get_param_from_fname_string(splits, "seed").split(".")[0])
        n_vals.append(n)
        boxsizes.append(boxsize)
        seed_vals.append(seed)
        padding_vals.append(padding)
        file_paths.append(os.path.join(tables_dir, file))
    list_of_columns=      ["n",    "padding",    "boxsize", "seed",    "filepath"] 
    full_data_list = zip(*[ n_vals, padding_vals, boxsizes,  seed_vals, file_paths ])
    return pd.DataFrame(full_data_list, columns=list_of_columns)

def plot_mean_std_plot_for_runs(df_of_interest, axs, color, n, boxsize, label, plot_init=True):

    cellsize = boxsize / n
    first_file_df = pd.read_csv(df_of_interest.filepath.iloc[0])
    Tij_list = [c for c in first_file_df.columns if c[0] == "T"]

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
        std_ratio  = tij_ratio_df.std(axis=1)
        
        # Get n_eff from the first file (same for all files)
        n_eff = first_file_df.n_eff
        
        # Assign subplot position
        col = i // ncols
        row = i % ncols
        subplot = axs[col, row]
        
        # Plot mean line
        subplot.plot(n_eff*cellsize, mean_ratio, label=label, color=color)
        # Plot shaded region for mean ± standard deviation
        subplot.fill_between(n_eff*cellsize, mean_ratio - std_ratio, mean_ratio + std_ratio, 
                             alpha=0.3, color=color) #, label='$\pm 1\sigma$')
        # Add reference line at y=1
        if plot_init:
            subplot.plot(n_eff*cellsize, np.ones(len(n_eff)), 'k--', alpha=0.5)
        
        # Customize subplot
        subplot.set_title(f"${Tij}/{Tij}_0$")
        subplot.set_xlabel('Box physical size, Mpc/h')
        #subplot.set_ylabel('Ratio')
        subplot.legend()

# Run parameters that we care about
padding = 0 # -1 for Full padding
boxsize = 1000
eps = 1e-10

df = get_df_of_run_files()
df_of_interest_256 = df[(df.n == 256) & (df.padding == padding) & (abs(df.boxsize - boxsize)/boxsize < eps)]
df_of_interest_128 = df[(df.n == 128) & (df.padding == padding) & (abs(df.boxsize - boxsize)/boxsize < eps)]
df_of_interest_64  = df[(df.n == 64)  & (df.padding == padding) & (abs(df.boxsize - boxsize)/boxsize < eps)]
df_of_interest_128_full = df[(df.n == 128)  & (df.padding == -1) & (abs(df.boxsize - boxsize)/boxsize < eps)]

boxsize = 1000
minx = 400
# Define subplot grid parameters (adjust ncols as needed)
ncols, nrows = 3, 2
subplotsize=4
figsize = (ncols+1)*subplotsize, (nrows+1)*subplotsize
fig, axs = plt.subplots(nrows, ncols, figsize=figsize, sharex=True)
axs = np.array(axs).reshape(nrows, ncols)  # Ensure axs is 2D even if nrows=1

plot_mean_std_plot_for_runs(df_of_interest_256, axs, "blue",  256, boxsize, "256 cells", True )
plot_mean_std_plot_for_runs(df_of_interest_128, axs, "green", 128, boxsize, "128 cells")
#plot_mean_std_plot_for_runs(df_of_interest_128_full, axs, "yellow", 128, boxsize, "128 cells, padded")
plot_mean_std_plot_for_runs(df_of_interest_64,  axs, "red",   64,  boxsize, "64 cells")

for i in range(ncols*nrows):
    axis = axs[i//ncols, i%ncols]
    axis.set_xlim([minx, boxsize])
    axis.relim()
    axis.autoscale()
    
# Adjust layout and display
plt.tight_layout()
plt.savefig("Tij_comparison.png")
plt.show()
