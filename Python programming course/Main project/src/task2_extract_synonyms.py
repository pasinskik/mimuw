"""
Task 2.1 - create a data frame that keeps all names (synonyms) for each drug.
"""

import pandas as pd
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_id = drug.find("drugbank:drugbank-id[@primary='true']", namespace)
        synonyms = drug.find("drugbank:synonyms", namespace)

        if synonyms is not None:
            synonym_list = [syn.text for syn in synonyms.findall("drugbank:synonym", namespace)]
        else:
            synonym_list = []

        data.append({
            "DrugBank ID": drug_id.text if drug_id is not None else None,
            "Synonyms": "; ".join(synonym_list) if synonym_list else None
        })

    df = pd.DataFrame(data)
    df.to_csv("drug_synonyms.csv", index=False)
    print("Task 2.1 completed: drug_synonyms.csv saved.")

# Run if executed directly
if __name__ == "__main__":
    process_task()