"""Generates unic embeddings of the unicode character database.

    Reads templates from `template/`, scans them for placeholders marked with `$`, and writes the resulting files to `out/`
"""
import os
import re
import math
from ucd import *
from datetime import datetime
from typing import List

IN_DIR = "template"
OUT_DIR = "out"

# Matches a `$identifier` placeholder.
_VAR_RE = re.compile(r"\$[a-zA-Z_]+")

def bits(value: int) -> int:
    """The minimum number of bits required to represent an integer"""
    return int(math.ceil(math.log2(abs(value) + 1)))

def indexBits(into : List[any]) -> int:
    """The minimum number of bits required to represent an index into an array"""
    return bits(len(into) - 1)

def to_hex_c(value: int) -> str:
    """Format an integer as a C hex literal"""
    return "0x" + format(value, "04X")

def _gc_enum(db : UCDB) -> str:
    """Build the `enum unic_gc` body"""

    out: list[str] = []
    counter = 0

    for maj in db.major_categories:
        out.append(f"{maj.full_id} = {counter} << UNIC_GC_SUB_BITS,")
        out.append(f"/** Alias for {maj.full_id} */")
        out.append(f"{maj.short_id} = {maj.full_id},")

        for gc in maj.minor_categories:
            out.append(f"{gc.full_id},")
            out.append(f"/** Alias for {gc.full_id} */")
            out.append(f"{gc.short_id} = {gc.full_id},")

        counter += 1

    return '\t' + "\n\t".join(out)

def process(path: str, expand : Callable[[str], str]) -> None:
    """Substitutes placeholders in a single template"""

    filename = os.path.basename(path)
    out_path = os.path.join(OUT_DIR, filename)

    with open(path, encoding="utf-8") as in_file:
        content = in_file.read()

    content = _VAR_RE.sub(lambda m: expand(m.group(0)[1:]), content)

    with open(out_path, "w", encoding="utf-8") as out_file:
        out_file.write(f"// THIS IS AN AUTO-GENERATED FILE - DO NOT EDIT - generated at {datetime.now()}\n")
        out_file.write(content)


def main() -> None:
    if os.path.isdir(OUT_DIR):
        for f in os.listdir(OUT_DIR):
            os.remove(os.path.join(OUT_DIR, f))

    os.makedirs(OUT_DIR, exist_ok=True)

    db : UCDB = load()
    print(f"Loaded Unicode Character Database Version {db.version_string}")

    deltas = sorted({ c.delta for c in db.codepoints }, key= lambda xs: sum(abs(x) for x in xs))
    sub_bits = max(indexBits(maj.minor_categories) for maj in db.major_categories)

    def expand(key : str) -> str:
        """ Computes the replacement of the given placeholder key """

        match key:
            case "version":
                return "%u%u%u" % tuple(db.version)
            case "version_string": 
                return '"%s"' % db.version_string
            case "max":
                return '0x%04X' % db.max_codepoint
            case "bit":
                return str(bits(db.max_codepoint))
            case "sub_bits":
                return str(sub_bits)
            case "gc_bits":
                return str(indexBits(db.major_categories) + sub_bits)
            case "GC":
                return _gc_enum(db)
            case "UCDB":
                return ',\n\t'.join(
                    "{ 0x%04X, %s, %d }" % (c.value, c.general_category.short_id, deltas.index(c.delta))
                    for c in db.codepoints
                )
            case "UCDB_DELTAS":
                return ',\n\t'.join(
                    "{ %d, %d }" % tuple(d)
                    for d in deltas
                )
            case "count":
                return str(len(db.codepoints))
            case "ucd_bits":
                return str(bits(max(c.simple_uppercase_delta for c in db.codepoints)) + 1)
            case "lcd_bits":
                return str(bits(max(c.simple_lowercase_delta for c in db.codepoints)) + 1)
            case "delta_ix_bits":
                return str(indexBits(deltas))
            case "max_direct":
                return str(next(
                        i
                        for i, c in enumerate(db.codepoints)
                        if c.value != i
                    ) - 1)
            case _:
                raise KeyError(f"Unknown template placeholder: {key}")

    templates = [os.path.join(IN_DIR, f) for f in sorted(os.listdir(IN_DIR))]
    progress("Processing Templates...", templates, lambda f: process(f, expand))

if __name__ == "__main__":
    main()
