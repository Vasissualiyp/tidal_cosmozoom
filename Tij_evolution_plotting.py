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
file_df = pd.read_csv(df_of_interest.filepath[0])
n_eff = file_df.n_eff

ncols, nrows = 3, 2
subplotsize=4
figsize = (ncols+1)*subplotsize, (nrows+1)*subplotsize

fig, axs = plt.subplots(ncols=ncols, nrows=nrows, figsize=figsize,
                        layout="constrained")
Tij_list = [c for c in file_df.columns if c[0]=="T"]
for i, Tij in enumerate(Tij_list):
    Tij_ratio = file_df[Tij]/file_df[Tij][0]
    col = i // ncols
    row = i % ncols
    subplot = axs[col, row]
    subplot.set_title("$"+Tij + "/" + Tij + "_0$")
    subplot.plot(n_eff, Tij_ratio, 'o')
    subplot.plot(n_eff, np.ones(len(n_eff)))
plt.show()
