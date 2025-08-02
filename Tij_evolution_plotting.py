import pandas as pd
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
    pass
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
            padding = 0
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

df = get_df_of_run_files("tables")
print(df)
