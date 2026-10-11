#!/usr/bin/env python3
"""Apply compact, print-safe OOXML styles to the generated laboratory report."""

import pathlib
import sys
import xml.etree.ElementTree as ET
import zipfile


W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
ET.register_namespace("w", W)


def tag(name):
    return f"{{{W}}}{name}"


def child(parent, name):
    found = parent.find(tag(name))
    if found is None:
        found = ET.SubElement(parent, tag(name))
    return found


def style_styles(data):
    root = ET.fromstring(data)
    font_sizes = {"Normal": 20, "BodyText": 20, "Title": 38, "Subtitle": 23,
                  "Heading1": 29, "Heading2": 25,
                  "SourceCode": 18, "VerbatimChar": 18}
    for style in root.findall(tag("style")):
        style_id = style.get(tag("styleId"))
        if style_id not in font_sizes:
            continue
        properties = child(style, "rPr")
        fonts = child(properties, "rFonts")
        font_name = ("Courier New" if style_id in ("SourceCode", "VerbatimChar")
                     else "Times New Roman")
        for attribute in ("ascii", "hAnsi", "eastAsia", "cs"):
            fonts.set(tag(attribute), font_name)
        child(properties, "color").set(tag("val"), "000000")
        child(properties, "sz").set(tag("val"), str(font_sizes[style_id]))
        if style_id == "BodyText":
            spacing = child(child(style, "pPr"), "spacing")
            spacing.set(tag("before"), "80")
            spacing.set(tag("after"), "100")
        if style_id == "Title":
            paragraph = child(style, "pPr")
            child(paragraph, "jc").set(tag("val"), "center")
            spacing = child(paragraph, "spacing")
            spacing.set(tag("before"), "200")
            spacing.set(tag("after"), "150")
        if style_id in ("Heading1", "Heading2"):
            paragraph = child(style, "pPr")
            child(paragraph, "keepNext").set(tag("val"), "true")
            spacing = child(paragraph, "spacing")
            spacing.set(tag("before"), "220")
            spacing.set(tag("after"), "90")
        if style_id == "SourceCode":
            child(child(style, "pPr"), "keepLines").set(tag("val"), "true")
    return ET.tostring(root, encoding="utf-8", xml_declaration=True)


def style_document(data):
    root = ET.fromstring(data)
    section = root.find(f".//{tag('sectPr')}")
    if section is not None:
        page = child(section, "pgSz")
        page.set(tag("w"), "12240")
        page.set(tag("h"), "15840")
        margins = child(section, "pgMar")
        for key, value in {"top": "720", "bottom": "720",
                           "left": "1152", "right": "1152",
                           "header": "500", "footer": "500"}.items():
            margins.set(tag(key), value)

    widths = ([2500, 1800, 2200, 2860],
              [2500, 2200, 2400, 2260],
              [2200, 2500, 2500, 2160])
    for index, table in enumerate(root.findall(f".//{tag('tbl')}")):
        column_widths = widths[index]
        properties = child(table, "tblPr")
        table_width = child(properties, "tblW")
        table_width.set(tag("w"), "9360")
        table_width.set(tag("type"), "dxa")
        child(properties, "tblLayout").set(tag("type"), "fixed")
        borders = child(properties, "tblBorders")
        for side in ("top", "left", "bottom", "right", "insideH", "insideV"):
            border = child(borders, side)
            border.set(tag("val"), "single")
            border.set(tag("sz"), "4")
            border.set(tag("color"), "D9D9D9")
        padding = child(properties, "tblCellMar")
        for side in ("top", "left", "bottom", "right"):
            margin = child(padding, side)
            margin.set(tag("w"), "90")
            margin.set(tag("type"), "dxa")
        grid = child(table, "tblGrid")
        for column, width in zip(grid.findall(tag("gridCol")), column_widths):
            column.set(tag("w"), str(width))
        for row_number, row in enumerate(table.findall(tag("tr"))):
            if row_number == 0:
                child(child(row, "trPr"), "tblHeader").set(tag("val"), "true")
            for column_number, cell in enumerate(row.findall(tag("tc"))):
                cell_properties = child(cell, "tcPr")
                cell_width = child(cell_properties, "tcW")
                cell_width.set(tag("w"), str(column_widths[column_number]))
                cell_width.set(tag("type"), "dxa")
                child(cell_properties, "vAlign").set(tag("val"), "center")
                if row_number == 0:
                    child(cell_properties, "shd").set(tag("fill"), "E7EEF4")
    return ET.tostring(root, encoding="utf-8", xml_declaration=True)


source = pathlib.Path(sys.argv[1])
destination = pathlib.Path(sys.argv[2])
with zipfile.ZipFile(source) as original, zipfile.ZipFile(
        destination, "w", compression=zipfile.ZIP_DEFLATED) as styled:
    for entry in original.infolist():
        data = original.read(entry.filename)
        if entry.filename == "word/styles.xml":
            data = style_styles(data)
        elif entry.filename == "word/document.xml":
            data = style_document(data)
        styled.writestr(entry, data)
