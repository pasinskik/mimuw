"""
Task 14 - tests for a few tasks, they check correctness if processing the original file,
as well as mock files.
"""

import os
import sys
import xml.etree.ElementTree as ET
import pandas as pd
import pytest
import matplotlib.pyplot as plt
from unittest.mock import patch
from fastapi.testclient import TestClient

# Import task functions from src modules
from src.task1 import process_task as process_task_1
from src.task2_plot_synonyms import plot_synonyms_graph
from src.task4 import process_task as process_task_4
from src.task5 import process_task as process_task_5
from src.task7 import process_task as process_task_7
from src.task9 import process_task as process_task_9
from src.task13 import process_task as process_task_13
from src.task15 import app

# =======================================================
# Fixtures for incomplete data for XML & CSV testing
# =======================================================

# One drug with ID and name, no other fields
@pytest.fixture
def incomplete_xml(tmp_path):
    """
    Create an XML tree with one drug element that is missing several fields.
    """
    xml_str = '''<?xml version="1.0" encoding="UTF-8"?>
    <drugbank xmlns="http://www.drugbank.ca" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
      <drug type="small molecule">
        <drugbank-id primary="true">DB00099</drugbank-id>
        <name>TestDrug</name>
        <!-- Missing description, state, indication, mechanism-of-action, food-interactions -->
      </drug>
    </drugbank>
    '''
    xml_file = tmp_path / "incomplete.xml"
    xml_file.write_text(xml_str)
    tree = ET.ElementTree(ET.fromstring(xml_str))
    return tree.getroot()

# Three drugs with some fields
@pytest.fixture
def incomplete_xml_2(tmp_path):
    xml_str = '''<?xml version="1.0" encoding="UTF-8"?>
    <drugbank xmlns="http://www.drugbank.ca" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    <drug type="small molecule">
        <drugbank-id primary="true">DB00010</drugbank-id>
        <name>Adderall</name>
        <!-- Missing description and indication -->
        <state>Tablet</state>
        <mechanism-of-action>Mixed amphetamine salts act as central nervous system stimulants.</mechanism-of-action>
    </drug>
    <drug type="small molecule">
        <drugbank-id primary="true">DB00011</drugbank-id>
        <name>Zoloft</name>
        <!-- Missing description, state, indication, food interactions -->
        <mechanism-of-action>Selective serotonin reuptake inhibitor.</mechanism-of-action>
    </drug>
    <drug type="small molecule">
        <drugbank-id primary="true">DB00012</drugbank-id>
        <name>Wellbutrin</name>
        <!-- Missing description, state, indication -->
        <mechanism-of-action>Norepinephrine-dopamine reuptake inhibitor.</mechanism-of-action>
    </drug>
    </drugbank>
    '''
    xml_file = tmp_path / "incomplete_drugs.xml"
    xml_file.write_text(xml_str)
    tree = ET.ElementTree(ET.fromstring(xml_str))
    return tree.getroot()

# Original file without the synonyms column
@pytest.fixture
def incomplete_synonyms_csv(tmp_path):
    """
    Create a temporary CSV file where the synonyms column is missing or empty.
    """
    data = {"DrugBank ID": ["DB00001"], "Synonyms": [None]}
    df = pd.DataFrame(data)
    csv_path = tmp_path / "incomplete_synonyms.csv"
    df.to_csv(csv_path, index=False)
    return csv_path

# =======================================================
# Test for behavior with original file
# =======================================================

# ---- Task 1 Tests ----
# Checks if a csv file is created
def test_task1_csv_file_exists():
    process_task_1()
    csv_file = "drug_data_summary.csv"
    assert os.path.exists(csv_file), "Task 1: CSV file not created."

# Checks if the file created contains all the necessary collumns
def test_task1_csv_columns():
    csv_file = "drug_data_summary.csv"
    df = pd.read_csv(csv_file)
    expected_columns = {"DrugBank ID", "Name", "Type", "Description", "State", 
                        "Indication", "Mechanism of Action", "Food Interactions"}
    assert expected_columns.issubset(set(df.columns)), "Task 1: Missing expected columns."

# ---- Task 4 Tests ----
# Checks if a csv file is created
def test_task4_csv_file_exists():
    process_task_4()
    csv_file = "drug_pathways.csv"
    assert os.path.exists(csv_file), "Task 4: CSV file not created."

# Checks if the file created contains all the necessary collumns
def test_task4_csv_contents():
    csv_file = "drug_pathways.csv"
    df = pd.read_csv(csv_file)
    expected_columns = {"Pathway ID", "Pathway Name", "Pathway Type"}
    assert expected_columns.issubset(set(df.columns)), "Task 4: Missing expected columns."
    assert len(df) > 0, "Task 4: CSV file is empty."

# ---- Task 5 Tests ----
# Checks if a graph is created
@patch("src.task5.plt.show")
@patch("src.task5.plt.savefig")
def test_task5_graph(mock_savefig, mock_show):
    process_task_5()
    png_file = "drug_pathway_graph.png"
    assert os.path.exists(png_file), "Task 5: PNG graph not created."
    mock_savefig.assert_called_once()
    mock_show.assert_called_once()

# ---- Task 7 Tests ----
# Checks if a csv file is created
def test_task7_csv_file_exists():
    process_task_7()
    csv_file = "drug_target_interactions.csv"
    assert os.path.exists(csv_file), "Task 7: CSV file not created."

# Checks if the file created contains all the necessary collumns
def test_task7_csv_contents():
    csv_file = "drug_target_interactions.csv"
    df = pd.read_csv(csv_file)
    expected_columns = {"Drug Name", "Target DrugBank ID", "External Source", 
                        "External ID", "Polypeptide Name", "Gene Name", 
                        "GenAtlas ID", "Chromosome", "Cellular Location"}
    assert expected_columns.issubset(set(df.columns)), "Task 7: Missing expected columns."

# ---- Task 9 Tests ----
# Checks if a graph is created
@patch("src.task9.plt.show")
@patch("src.task9.plt.savefig")
def test_task9_pie_chart(mock_savefig, mock_show):
    process_task_9()
    png_file = "drug_status_piechart.png"
    assert os.path.exists(png_file), "Task 9: Pie chart PNG not created."
    mock_savefig.assert_called_once()
    mock_show.assert_called_once()

# ---- Task 13 Tests ----
# Checks if the code generates 200 drugs
def test_task13_generated_xml():
    process_task_13()
    xml_file = "drugbank_partial_and_generated.xml"
    assert os.path.exists(xml_file), "Task 13: XML file not created."
    tree = ET.parse(xml_file)
    root_elem = tree.getroot()
    drugs = root_elem.findall("{http://www.drugbank.ca}drug")
    assert len(drugs) == 200, f"Task 13: Expected 200 drugs, got {len(drugs)}."

# Checks the correctness of the generated file
@pytest.mark.parametrize("tag", [
    "{http://www.drugbank.ca}drugbank-id",
    "{http://www.drugbank.ca}name",
    "{http://www.drugbank.ca}description",
    "{http://www.drugbank.ca}mechanism-of-action",
    "{http://www.drugbank.ca}indication"
])
def test_task13_drug_fields(tag):
    xml_file = "drugbank_partial_and_generated.xml"
    tree = ET.parse(xml_file)
    root_elem = tree.getroot()
    first_drug = root_elem.find("{http://www.drugbank.ca}drug")
    assert first_drug is not None, "Task 13: No drug element found."
    assert first_drug.find(tag) is not None, f"Task 13: Missing field {tag}."

# Checks the correctness of the structure of the generated file
def test_task13_valid_xml_structure():
    xml_file = "drugbank_partial_and_generated.xml"
    try:
        ET.parse(xml_file)
    except ET.ParseError:
        pytest.fail("Task 13: Generated XML is not well-formed.")

# ---- Task 15 Tests (FastAPI Server) ----
# Checks if the code works as intended
def test_task15_get_pathway_count():
    client = TestClient(app)
    response = client.post("/get-pathway-count/", json={"drug_id": "DB00001"})
    assert response.status_code == 200
    json_data = response.json()
    assert "drug_id" in json_data and "pathway_count" in json_data, "Task 15: Invalid response structure."

# =======================================================
# Tests for behavior with mock (incomplete) data
# =======================================================

# ---- Task 1 Tests ----
# Checks if missing fields are handled when given incomplete data (first fixture)
def test_task1_incomplete_data(monkeypatch, tmp_path, incomplete_xml):
    import src.task1 as task1
    # Monkeypatch the root in task1 with fixture 1
    monkeypatch.setattr(task1, "root", incomplete_xml)
    task1.process_task()
    df = pd.read_csv("drug_data_summary.csv")
    # Check that fields like Description, Indication, Mechanism of Action are None or empty
    assert pd.isna(df.loc[0, "Description"]) or df.loc[0, "Description"] == "", "Description should be missing"
    assert pd.isna(df.loc[0, "Indication"]) or df.loc[0, "Indication"] == "", "Indication should be missing"
    # Clean up the generated CSV file
    os.remove("drug_data_summary.csv")

# Checks if missing fields are handled when given incomplete data (second fixture)
def test_task1_incomplete_data_multiple(monkeypatch, tmp_path, incomplete_xml_2):
    import src.task1 as task1
    # Monkeypatch the root in task1 with fixture 2
    monkeypatch.setattr(task1, "root", incomplete_xml_2)
    task1.process_task()
    df = pd.read_csv("drug_data_summary.csv")
    # Check that all three drugs are present
    drugs = set(df["DrugBank ID"].tolist())
    expected = {"DB00010", "DB00011", "DB00012"}
    assert expected.issubset(drugs), "Not all expected drugs are present in CSV."
    # Check that missing fields (e.g., Description, Indication) are empty or NaN
    for col in ["Description", "Indication", "Food Interactions"]:
        # Either the value is an empty string or it is NaN
        for val in df[col]:
            assert val == "" or pd.isna(val), f"Field {col} should be missing but got: {val}"
    os.remove("drug_data_summary.csv")

# ---- Task 2.2 Tests ----
def create_test_synonyms_csv(tmp_path):
    data = {"DrugBank ID": ["DB00001"], "Synonyms": ["Syn1; Syn2; Syn3"]}
    df = pd.DataFrame(data)
    csv_path = tmp_path / "drug_synonyms_test.csv"
    df.to_csv(csv_path, index=False)
    return csv_path

# Checks if a graph is created with mock data
@patch("src.task2_plot_synonyms.plt.show")
@patch("src.task2_plot_synonyms.plt.savefig")
def test_task2_2_graph(mock_savefig, mock_show, tmp_path):
    csv_path = create_test_synonyms_csv(tmp_path)
    output_png = tmp_path / "synonyms_DB00001.png"
    plot_synonyms_graph("DB00001", df_path=str(csv_path), output_path=str(output_png))
    mock_savefig.assert_called_once()
    mock_show.assert_called_once()

# Checks if a graph is created with an empty synonyms CSV file
def test_task2_2_empty_synonyms(incomplete_synonyms_csv, capsys):
    # Use the fixture's return value (the file path)
    csv_path = incomplete_synonyms_csv 
    # Call plot_synonyms_graph; it should print a message and return without creating a file
    plot_synonyms_graph("DB00001", df_path=str(csv_path), output_path="dummy.png")
    # Capture printed output
    captured = capsys.readouterr().out
    assert "No synonyms found for DB00001" in captured, "Should warn about missing synonyms"


# ---- Task 13 Tests ----
# Checks if 100 new drugs are created when given a file with 3 drugs (fixture 2)
def test_task13_incomplete_data_multiple(monkeypatch, tmp_path, incomplete_xml_2):
    import src.task13 as task13
    monkeypatch.setattr(task13, "root", incomplete_xml_2)
    task13.process_task()
    xml_file = "drugbank_partial_and_generated.xml"
    assert os.path.exists(xml_file), "Task 13: XML file not created."
    tree = ET.parse(xml_file)
    root_elem = tree.getroot()
    # There should be 3 original drugs plus 100 synthetic ones
    drugs = root_elem.findall("{http://www.drugbank.ca}drug")
    assert len(drugs) == 103, f"Task 13: Expected 103 drugs, got {len(drugs)}."
    os.remove("drugbank_partial_and_generated.xml")

# Checks if the file prints an error message when given no drug elements
def test_task13_no_drugs(monkeypatch, tmp_path):
    empty_root = ET.Element("drugbank", {"version": "0"})
    monkeypatch.setattr("src.task13.root", empty_root)
    # Capture output message by redirecting stdout
    from io import StringIO
    captured_output = StringIO()
    sys.stdout = captured_output
    try:
        process_task_13()
    finally:
        sys.stdout = sys.__stdout__
    output = captured_output.getvalue()
    assert "No drugs found in the original XML data." in output

# ---- Task 15 Tests ----
# Checks the behavior for a non-existing drug ID
def test_task15_drug_not_found():
    client = TestClient(app)
    response = client.post("/get-pathway-count/", json={"drug_id": "NONEXISTENT"})
    assert response.status_code == 200
    json_data = response.json()
    assert json_data["pathway_count"] == 0, "Should return 0 for non-existent drug"

# =======================================================
# Integration: run all tests if invoked from main.py
# =======================================================
def process_task():
    sys.exit(pytest.main(["-q", os.path.abspath(__file__)]))

if __name__ == "__main__":
    process_task()
