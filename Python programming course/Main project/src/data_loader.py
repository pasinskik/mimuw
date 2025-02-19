import xml.etree.ElementTree as ET

# Load XML once
XML_PATH = "data/drugbank_partial.xml"
tree = ET.parse(XML_PATH)
root = tree.getroot()

# Define namespace
namespace = {'drugbank': 'http://www.drugbank.ca'}
