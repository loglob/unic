"""Generates unic embeddings of the unicode character database.

    Reads templates from `template/`, scans them for placeholders marked with `$`, and writes the resulting files to `out/`
"""
import os
import re
import ucd
import math
from datetime import datetime

IN_DIR = "template"
OUT_DIR = "out"

# Matches a `$identifier` placeholder.
_VAR_RE = re.compile(r"\$[a-zA-Z_]+")

def bits(value: int) -> int:
    """The minimum number of bits required to represent an integer"""
    return int(math.ceil(math.log2(abs(value) + 1)))

def to_hex_c(value: int) -> str:
    """Format an integer as a C hex literal"""
    return "0x" + format(value, "04X")

def _gc_enum() -> str:
    """Build the `enum unic_gc` body"""

    out: list[str] = []
    counter = 0

    for gc in [g for g in ucd.GENERAL_CATEGORIES.values() if g.is_super]:
        out.append(f"{gc.full_id} = {counter} << UNIC_GC_SUB_BITS,")
        out.append(f"/** Alias for {gc.full_id} */")
        out.append(f"{gc.short_id} = {gc.full_id},")

        for sub in gc.sub_categories:
            out.append(f"{sub.full_id},")
            out.append(f"/** Alias for {sub.full_id} */")
            out.append(f"{sub.short_id} = {sub.full_id},")

        counter += 1

    return '\t' + "\n\t".join(out)

def expand(key : str) -> str:
    """ Computes the replacement of the given placeholder key """
    super_categories = [g for g in ucd.GENERAL_CATEGORIES.values() if g.is_super]
    super_bits = bits(len(super_categories) - 1)
    sub_bits = bits(max(len(g.sub_categories) for g in super_categories))

    match key:
        case "version":
            return f"{ucd.VERSION[0]}{ucd.VERSION[1]}{ucd.VERSION[2]}"
        case "version_string": 
            return '"' + ucd.version_string() + '"'
        case "max":
            return to_hex_c(max(ucd.CODEPOINTS))
        case "bit":
            return str(bits(max(ucd.CODEPOINTS)))
        case "sub_bits":
            return str(sub_bits)
        case "gc_bits":
            return str(super_bits + sub_bits)
        case "GC":
            return _gc_enum()
        case "UCDB":
            return '\t' + ',\n\t'.join([
                f"{{ {to_hex_c(chr.value)}, {ucd.GeneralCategory.PREFIX}_{chr.general_category.upper()}, {chr.simple_uppercase_delta}, {chr.simple_lowercase_delta} }}"
                for chr in ucd.CODEPOINTS.values()
            ])
        case "count":
            return str(len(ucd.CODEPOINTS))
        case "ucd_bits":
            return str(bits(max(c.simple_uppercase_delta for c in ucd.CODEPOINTS.values())) + 1)
        case "lcd_bits":
            return str(bits(max(c.simple_lowercase_delta for c in ucd.CODEPOINTS.values())) + 1)
        case "max_direct":
            return str(next(
                    i
                    for i, c in enumerate(ucd.CODEPOINTS.values())
                    if c.value != i
                ) - 1)
        case _:
            raise KeyError(f"Unknown template placeholder: {key}")

def process(path: str) -> None:
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

    ucd.load()
    print(f"Loaded Unicode Character Database Version {ucd.version_string()}")

    templates = [os.path.join(IN_DIR, f) for f in sorted(os.listdir(IN_DIR))]
    ucd.progress("Processing Templates...", templates, lambda f: process(f))

if __name__ == "__main__":
    main()
