import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.ticker import ScalarFormatter
import matplotlib

matplotlib.rcParams['pdf.fonttype'] = 42
sns.set_theme(style="darkgrid", rc={"axes.facecolor": "#EAEAF2", "font.family": "serif"})

df = pd.read_csv('chi_square_plot_data.csv')
plot_df = df.iloc[::20, :] 

fig, ax = plt.subplots(figsize=(8, 5), dpi=120)

# Draw Chi-Square only
ax.plot(plot_df['Size_KB'], plot_df['Chi_Square'], color='#1f77b4', 
        marker='o', markersize=5, linewidth=1.5,
        markerfacecolor='white', markeredgewidth=1.5, label='Chi-Square Statistic')

# landscaping settings
ax.set_xscale('log')
ax.set_yscale('log') # This growth trend is most "linear" and professional when viewed using logarithmic coordinates.
ax.set_xlabel('Log Window Size (KB)', fontsize=12, fontweight='bold')
ax.set_ylabel('Chi-Square Value', fontsize=12, fontweight='bold')

ax.xaxis.set_major_formatter(ScalarFormatter())
ax.set_xticks([0.5, 1, 10, 100, 1000, 3000])

ax.legend(loc='upper left', frameon=True, facecolor='white')
plt.tight_layout()
plt.savefig('Result_ChiSquare_Trend.pdf', format='pdf', bbox_inches='tight')
plt.show()