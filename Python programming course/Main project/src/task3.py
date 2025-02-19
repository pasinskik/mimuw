"""
Task 3 - create a data frame of pharmaceutical products containing a given drug;
the frame keeps for each product: its active substance's ID, name, manufacturer,
National Drug Code (NDC), dosage form, route of administration, dose, country and
regulatory agency.
"""

import pandas as pd
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_id = drug.find("drugbank:drugbank-id[@primary='true']", namespace).text \
            if drug.find("drugbank:drugbank-id[@primary='true']", namespace) is not None else None
        
        for product in drug.findall("drugbank:products/drugbank:product", namespace):
            product_name = product.find("drugbank:name", namespace).text \
                if product.find("drugbank:name", namespace) is not None else None
            manufacturer = product.find("drugbank:labeller", namespace).text \
                if product.find("drugbank:labeller", namespace) is not None else None
            ndc_code = product.find("drugbank:ndc-product-code", namespace).text \
                if product.find("drugbank:ndc-product-code", namespace) is not None else None
            dosage_form = product.find("drugbank:dosage-form", namespace).text \
                if product.find("drugbank:dosage-form", namespace) is not None else None
            route = product.find("drugbank:route", namespace).text \
                if product.find("drugbank:route", namespace) is not None else None
            strength = product.find("drugbank:strength", namespace).text \
                if product.find("drugbank:strength", namespace) is not None else None
            country = product.find("drugbank:country", namespace).text \
                if product.find("drugbank:country", namespace) is not None else None
            source = product.find("drugbank:source", namespace).text \
                if product.find("drugbank:source", namespace) is not None else None
            
            data.append({
                "DrugBank ID": drug_id,
                "Product Name": product_name,
                "Manufacturer": manufacturer,
                "NDC Code": ndc_code if country == 'US' else 'N/A',
                "Dosage Form": dosage_form,
                "Route": route,
                "Strength": strength,
                "Country": country,
                "Regulatory Agency": source
            })

    df = pd.DataFrame(data)
    df.to_csv("pharmaceutical_products.csv", index=False)
    print("Task 3 completed: pharmaceutical_products.csv saved.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()