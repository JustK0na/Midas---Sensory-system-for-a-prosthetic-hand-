import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import StrMethodFormatter
dane = pd.read_csv("daneSprytne.csv")
# dane.columns = ["czas", "czujnik1", "czujnik2"]
dane2 = dane[(dane["czas"]>0) & (dane["czas"]<80000) ]
t = dane2["czas"]
c1 = dane2["czujnik1"]
c2 = dane2["czujnik2"]

plt.style.use('seaborn-v0_8-white')


plt.figure()
plt.scatter(t, c1)
plt.gca().yaxis.set_major_formatter(StrMethodFormatter('{x:.0f}'))
plt.ticklabel_format(style='plain', axis='y')
plt.xlabel("Czas [ms]")
plt.ylabel("Ciśnienie [Pa]")
plt.title("Czujnik 1")
plt.grid()



plt.figure()
plt.scatter(t, c2)
plt.gca().yaxis.set_major_formatter(StrMethodFormatter('{x:.0f}'))
plt.xlabel("Czas [ms]")
plt.ylabel("Ciśnienie [Pa]")
plt.title("Czujnik 2")
plt.grid()
plt.show()

#figure2 = prawa
#figure 1 = lewa
