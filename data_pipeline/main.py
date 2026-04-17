#!/usr/bin/python3


import os
from xml2db import xml2db
from xml_preprocessor import xml_preprocessor

os.makedirs("output", exist_ok=True)

xmlSource = "Malagasy.xml"
xmlCleaned = "output/Malagasy_cleaned.xml"
removed_text = "output/removed_text"
dbFile = "output/baiboly.db"

xml_prepocessor(xmlSource,xmlCleaned, removed_text)
xml2db(xmlCleaned,dbFile)