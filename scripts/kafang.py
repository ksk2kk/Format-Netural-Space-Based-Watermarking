import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.ticker import ScalarFormatter
import matplotlib

# --- Vector fonts and scientific research style settings ---
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
sns.set_theme(style="darkgrid", rc={
    "axes.facecolor": "#EAEAF2",
    "grid.color": "white",
    "font.family": "serif", 
})

# 1. Load data
df = pd.read_csv('chi_square_plot_data.csv')

# 2. Create a biaxial canvas
fig, ax1 = plt.subplots(figsize=(10, 6), dpi=120)
ax2 = ax1.twinx()

# --- Plot P-Value (left axis) ---
plot_df = df.iloc[::20, :] 
ax1.plot(plot_df['Size_KB'], plot_df['p_value'], color='#2ca02c', 
         label='P-Value', linewidth=2.5, alpha=0.9, zorder=3)
ax1.set_ylabel('P-Value', fontsize=14, color='#2ca02c', labelpad=10, fontweight='bold')
ax1.tick_params(axis='y', labelcolor='#2ca02c')
ax1.set_ylim(-0.05, 1.15) # Leave a little white space to prevent curves from welting

# --- Draw Chi-Square (right axis) ---
ax2.plot(plot_df['Size_KB'], plot_df['Chi_Square'], color='#1f77b4', 
         marker='o', markersize=6, linewidth=1.5, label='Chi-Square Statistic',
         markerfacecolor='white', markeredgewidth=1.5, clip_on=False, zorder=2)
ax2.set_ylabel('Chi-Square Statistic', fontsize=14, color='#1f77b4', labelpad=10, fontweight='bold')
ax2.tick_params(axis='y', labelcolor='#1f77b4')

# 3. Coordinate axes and scale
ax1.set_xlabel('Log Window Size (KB)', fontsize=14, labelpad=10)
ax1.set_xscale('log')

formatter = ScalarFormatter()
formatter.set_scientific(False)
ax1.xaxis.set_major_formatter(formatter)
ax1.set_xticks([0.5, 1, 5, 20, 100, 500, 3000])

# 4. [Core modification: Legend position] Move to the top to avoid the rising curve on the right
lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper center', 
           bbox_to_anchor=(0.5, 1.1), ncol=2, # Place it above the outside of the picture or directly above the inside of the picture
           frameon=True, facecolor='white', framealpha=0.9, fontsize=11)

# 5. [Core modification: mark text position and mask]
# Move the label above P=1 on the left to completely avoid the blue dot matrix
ax1.annotate('Statistical Invisibility Zone\n(P > 0.05)', 
             xy=(2, 1.0), xytext=(5, 0.6), # Adjust text coordinates
             arrowprops=dict(arrowstyle="->", color='black', 
                             connectionstyle="arc3,rad=.2", # Curved arrows look more professional
                             linewidth=1.5),
             fontsize=12, fontweight='bold', color='#1a5e1a',
             bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="none", alpha=0.7)) # Add text background mask

# 6. Save the output
plt.tight_layout()
plt.savefig('Chi_Square_Invisibility_Clean.pdf', format='pdf', bbox_inches='tight')
print("The adjusted PDF is saved. Text overlapping issue has been fixed.")
plt.show()