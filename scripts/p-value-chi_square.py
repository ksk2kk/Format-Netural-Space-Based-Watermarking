import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.ticker import ScalarFormatter
import matplotlib

matplotlib.rcParams['pdf.fonttype'] = 42
sns.set_theme(style="darkgrid", rc={"axes.facecolor": "#EAEAF2", "font.family": "serif"})

df = pd.read_csv('chi_square_plot_data.csv')
# In order to highlight the invisibility, we only display the area where the P value begins to fluctuate significantly, or display it in full but remove the interference
plot_df = df.iloc[::20, :] 

fig, ax = plt.subplots(figsize=(8, 5), dpi=120)

# Plot only P-Value
ax.plot(plot_df['Size_KB'], plot_df['p_value'], color='#2ca02c', 
        linewidth=2.5, marker='s', markersize=4, markevery=50,
        markerfacecolor='white', label='P-Value (Detection Significance)')

ax.axhline(y=0.05, color='r', linestyle='--', alpha=0.6, label='Significance Level (0.05)')

# landscaping settings
ax.set_xscale('log')
ax.set_xlabel('Log Window Size (KB)', fontsize=12, fontweight='bold')
ax.set_ylabel('P-Value', fontsize=12, fontweight='bold')
ax.set_ylim(-0.05, 1.1)

formatter = ScalarFormatter()
formatter.set_scientific(False)
ax.xaxis.set_major_formatter(formatter)
ax.set_xticks([0.5, 1, 10, 100, 1000, 3000])

ax.legend(loc='lower left', frameon=True, facecolor='white')
plt.tight_layout()
plt.savefig('Result_PValue_Invisibility.pdf', format='pdf', bbox_inches='tight')
plt.show()
