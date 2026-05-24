#!/usr/bin/env python3
"""Построение графиков из sorting_results.csv"""

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
CSV = ROOT / "sorting_results.csv"
OUT = ROOT / "figures"
OUT.mkdir(exist_ok=True)

ALGOS = [
    "QuickSort",
    "MergeSort",
    "StringQuickSort",
    "StringMergeSort",
    "MSDRadixSort",
    "MSDRadixSort+QS",
]
TYPES = ["Random", "Reverse", "NearlySorted", "Prefix"]


def plot_metric(df, metric_col, ylabel, filename):
    for array_type in TYPES:
        fig, ax = plt.subplots(figsize=(10, 6))
        sub = df[df["ArrayType"] == array_type]
        for algo in ALGOS:
            s = sub[sub["Algorithm"] == algo].sort_values("Size")
            if s.empty:
                continue
            ax.plot(s["Size"], s[metric_col], marker="o", markersize=3, label=algo)
        ax.set_xlabel("Размер массива (n)")
        ax.set_ylabel(ylabel)
        ax.set_title(f"{ylabel} — {array_type}")
        ax.grid(True, alpha=0.3)
        ax.legend()
        fig.tight_layout()
        fig.savefig(OUT / f"{filename}_{array_type}.png", dpi=150)
        plt.close(fig)


def main():
    if not CSV.exists():
        raise SystemExit(f"Не найден {CSV}. Сначала запустите ./sort_test")

    df = pd.read_csv(CSV)
    plot_metric(df, "AvgTime(us)", "Среднее время (мкс)", "time")
    plot_metric(df, "AvgComparisons", "Среднее число посимвольных сравнений", "comparisons")
    print(f"Графики сохранены в {OUT}/")


if __name__ == "__main__":
    main()
