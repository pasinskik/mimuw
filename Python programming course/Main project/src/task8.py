"""
Task 8 - create a pie chart showing the percentage distribution of targets
in different cellular locations.
"""

import pandas as pd
import matplotlib.pyplot as plt
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace).text \
            if drug.find("drugbank:name", namespace) is not None else None
        
        for target in drug.findall("drugbank:targets/drugbank:target", namespace):
            polypeptide = target.find("drugbank:polypeptide", namespace)
            
            if polypeptide is not None:
                target_id = polypeptide.get("id", "N/A")
                source = polypeptide.get("source", "N/A")
                poly_name = polypeptide.find("drugbank:name", namespace).text \
                    if polypeptide.find("drugbank:name", namespace) is not None else "N/A"
                gene_name = polypeptide.find("drugbank:gene-name", namespace).text \
                    if polypeptide.find("drugbank:gene-name", namespace) is not None else "N/A"
                chromosome = polypeptide.find("drugbank:chromosome-location", namespace).text \
                    if polypeptide.find("drugbank:chromosome-location", namespace) is not None else "N/A"
                cell_location = polypeptide.find("drugbank:cellular-location", namespace).text \
                    if polypeptide.find("drugbank:cellular-location", namespace) is not None else "N/A"
                
                # Extract GenAtlas ID and external identifiers
                genatlas_id = "N/A"
                external_id = "N/A"
                external_source = "N/A"
                external_identifiers = polypeptide.find("drugbank:external-identifiers", namespace)
                if external_identifiers is not None:
                    for ext_id in external_identifiers.findall("drugbank:external-identifier", namespace):
                        resource = ext_id.find("drugbank:resource", namespace)
                        identifier = ext_id.find("drugbank:identifier", namespace)
                        if resource is not None and identifier is not None:
                            if resource.text == "GenAtlas":
                                genatlas_id = identifier.text
                            if resource.text in ["Swiss-Prot", "UniProtKB", "GenBank Protein Database"]:
                                external_source = resource.text
                                external_id = identifier.text
                                
                data.append({
                    "Drug Name": drug_name,
                    "Target DrugBank ID": target_id,
                    "External Source": external_source,
                    "External ID": external_id,
                    "Polypeptide Name": poly_name,
                    "Gene Name": gene_name,
                    "GenAtlas ID": genatlas_id,
                    "Chromosome": chromosome,
                    "Cellular Location": cell_location
                })

    df = pd.DataFrame(data)

    # Count occurrences of each unique cellular location
    cellular_distribution = df["Cellular Location"].value_counts()

    # Filter out labels with less than 1% occurrence
    def autopct_filter(pct):
        return ('%1.1f%%' % pct) if pct >= 1 else ''

    filtered_labels = [label if (count / cellular_distribution.sum()) * 100 >= 1 else '' \
                        for label, count in zip(cellular_distribution.index, cellular_distribution)]

    # Plot pie chart
    plt.figure(figsize=(10, 10))
    plt.pie(cellular_distribution, labels=filtered_labels, 
            autopct=autopct_filter, startangle=140)
    plt.title("Percentage Distribution of Targets in Different Cellular Locations")
    plt.tight_layout()
    plt.savefig("target_cellular_distribution.png", format="png", dpi=300)
    plt.show()

    print("Task 8 completed: pie chart saved as target_cellular_distribution.png.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()