"""
Task 12 - analyze the most common protein targets in the database
(count occurrences of each target across all drugs).
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace)
        if drug_name is not None:
            drug_name = drug_name.text
        
        for target in drug.findall("drugbank:targets/drugbank:target", namespace):
            polypeptide = target.find("drugbank:polypeptide", namespace)
            if polypeptide is not None:
                target_name = polypeptide.find("drugbank:name", namespace)
                if target_name is not None:
                    target_name = target_name.text
                    
                    data.append({
                        "Drug Name": drug_name,
                        "Target Name": target_name
                    })

    df = pd.DataFrame(data)

    # Count occurrences of each target
    target_counts = df["Target Name"].value_counts().reset_index()
    target_counts.columns = ["Target Name", "Drug Count"]

    target_counts.to_csv("target_popularity.csv", index=False)

    # Plot bar chart of most common targets
    plt.figure(figsize=(12, 6))
    sns.barplot(x=target_counts["Drug Count"].head(20), y=target_counts["Target Name"].head(20), \
                palette="viridis", hue=target_counts["Target Name"].head(20), legend=False)
    plt.xlabel("Number of Drugs Targeting Protein")
    plt.ylabel("Protein Target")
    plt.title("Most Common Drug Targets")
    plt.tight_layout()
    plt.savefig("target_popularity_bar.png", format="png", dpi=300)
    plt.show()

    print("Task 12 completed: target_popularity.csv and target_popularity_bar.png saved.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()