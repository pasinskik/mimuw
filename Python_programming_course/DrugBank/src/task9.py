"""
Task 9 - create a data frame that will keep the number of drugs that are either
approved, withdrawn, experimental or approved for veterinary use and present
the data as a pie chart.
"""

import pandas as pd
import matplotlib.pyplot as plt
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    drug_status = {"Approved": 0, "Withdrawn": 0, "Experimental": 0, "Veterinary": 0}
    approved_without_withdrawn = 0

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace).text \
            if drug.find("drugbank:name", namespace) is not None else None
        
        groups = drug.find("drugbank:groups", namespace)
        if groups is not None:
            statuses = {group.text for group in groups.findall("drugbank:group", namespace)}
            
            if "approved" in statuses:
                drug_status["Approved"] += 1
                if "withdrawn" not in statuses:
                    approved_without_withdrawn += 1
            elif "withdrawn" in statuses:
                drug_status["Withdrawn"] += 1
            elif "experimental" in statuses or "investigational" in statuses:
                drug_status["Experimental"] += 1
            elif "veterinary" in statuses:
                drug_status["Veterinary"] += 1

    df = pd.DataFrame(list(drug_status.items()), columns=["Status", "Count"])

    df.to_csv("drug_status_summary.csv", index=False)

    # Filter out low-common labels for readability
    def autopct_filter(pct):
        return ('%1.1f%%' % pct) if pct >= 0.5 else ''

    filtered_labels = [label if (count / df["Count"].sum()) * 100 >= 0.5 else '' \
                        for label, count in zip(df["Status"], df["Count"])]


    # Plot pie chart
    plt.figure(figsize=(8, 8))
    plt.pie(df["Count"], labels=filtered_labels, autopct=autopct_filter, startangle=140)
    plt.title("Distribution of Drug Approval Statuses")
    plt.tight_layout()
    plt.savefig("drug_status_piechart.png", format="png", dpi=300)
    plt.show()

    print(f"Number of approved drugs that have not been withdrawn: {approved_without_withdrawn}")
    print("Task 9 completed: pie chart saved as drug_status_piechart.png.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()