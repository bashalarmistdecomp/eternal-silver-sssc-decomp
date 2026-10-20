#!/usr/bin/env python3
"""Append phony targets for each prerequisite in a GAS --MD depfile.

GAS has no -MP equivalent, so when a matched function's .s file is deleted
from asm/nonmatchings, a stale depfile would abort make with "No rule to
make target". The phony targets make missing prerequisites harmless.
"""
import sys

path = sys.argv[1]
with open(path) as f:
    text = f.read()

body = text.replace("\\\n", " ")
prereqs = []
for line in body.splitlines():
    if ":" in line:
        prereqs.extend(line.split(":", 1)[1].split())

with open(path, "a") as f:
    f.write("\n")
    for p in prereqs:
        f.write(f"{p}:\n")
