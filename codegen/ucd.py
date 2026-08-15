# ucd.py: Interacts with the unicode character database
import os
import re
import urllib.request
from dataclasses import dataclass
from typing import Callable, Iterable, Optional, TypeVar, Iterator

URL = "https://www.unicode.org/Public/UCD/latest/ucd/"
CACHE_DIR = "cache"

# Populated by the parsers below.
GENERAL_CATEGORIES: dict[str, "GeneralCategory"] = {}
CODEPOINTS: dict[int, "Codepoint"] = {}
VERSION: tuple[int, int, int] = (0, 0, 0)
ParserFn = Callable[[list[str]], None]

# Filename -> parser function.
PARSERS: dict[str, ParserFn] = {}

def remove_comments(lines: Iterable[str], cc: str = "#") -> Iterator[str]:
    """Removes comments from the given lines, and deletes entirely commented lines"""

    for line in lines:
        idx = line.find(cc)
        if idx < 0:
            yield line
        elif idx > 0:
            yield line[:idx]

def parser(filename: str) -> Callable[[ParserFn], ParserFn]:
    """Register a function as the parser for the given UCD file"""

    def decorate(fn: ParserFn) -> ParserFn:
        PARSERS[filename] = fn
        return fn

    return decorate


def version_string() -> str:
    """The unicode version as a human-readable string, e.g. `15.0.0`"""
    return f"{VERSION[0]}.{VERSION[1]}.{VERSION[2]}"

@dataclass(frozen=True)
class GeneralCategory:
    """A Unicode general category (e.g. `Lu`/`Uppercase_Letter`)."""

    PREFIX = "UCLASS"

    # The one or two character shorthand.
    shorthand: str
    # The alternative full name.
    full_name: str

    @property
    def full_id(self) -> str:
        return f"{self.PREFIX}_{self.full_name.upper()}"

    @property
    def short_id(self) -> str:
        return f"{self.PREFIX}_{self.shorthand.upper()}"

    @property
    def is_super(self) -> bool:
        """A super category is a single-character shorthand e.g. `L`"""

        return len(self.shorthand) == 1

    @property
    def sub_categories(self) -> list["GeneralCategory"]:
        """All sub categories that fall under this super category."""

        return [
            gc
            for gc in GENERAL_CATEGORIES.values()
            if gc.shorthand.startswith(self.shorthand) and not gc.is_super
        ]


@dataclass(frozen=True)
class Codepoint:
    """A single assigned Unicode codepoint and its simple case mappings."""

    # The codepoint's value.
    value: int
    # The single character this maps to in uppercase.
    simple_uppercase_mapping: int
    # The single character this maps to in lowercase.
    simple_lowercase_mapping: int
    # The general category shorthand it belongs to.
    general_category: str

    @property
    def simple_lowercase_delta(self) -> int:
        return self.simple_lowercase_mapping - self.value

    @property
    def simple_uppercase_delta(self) -> int:
        return self.simple_uppercase_mapping - self.value

    def __str__(self) -> str:
        return to_hex_c(self.value)


def _try_hex(text: str) -> Optional[int]:
    """Tries parsing a hexadecimal number"""

    text = text.strip()
    if not text:
        return None
    try:
        return int(text, 16)
    except ValueError:
        return None


@parser("UnicodeData.txt")
def parse_codepoints(lines: list[str]) -> None:
    codepoints: dict[int, Codepoint] = {}

    for raw in remove_comments(lines):
        fields = raw.split(";")
        if not fields or not fields[0].strip():
            continue

        value = int(fields[0], 16)
        upper = _try_hex(fields[12]) if len(fields) > 12 else None
        lower = _try_hex(fields[13]) if len(fields) > 13 else None

        codepoints[value] = Codepoint(
            value=value,
            simple_uppercase_mapping=upper if upper is not None else value,
            simple_lowercase_mapping=lower if lower is not None else value,
            general_category=fields[2],
        )

    global CODEPOINTS
    CODEPOINTS = codepoints


@parser("PropertyValueAliases.txt")
def parse_general_categories(lines: list[str]) -> None:
    match = re.search(r"[0-9]+\.[0-9]+\.[0-9]+", lines[0])
    parts = [int(p) for p in match.group(0).split(".")]
    global VERSION
    VERSION = (parts[0], parts[1], parts[2])

    categories: dict[str, GeneralCategory] = {}

    for raw in remove_comments(lines):
        fields = [field.strip() for field in raw.split(";")]
        if fields and fields[0] == "gc":
            gc = GeneralCategory(shorthand=fields[1], full_name=fields[2])
            categories[gc.shorthand] = gc

    global GENERAL_CATEGORIES
    GENERAL_CATEGORIES = categories


def _download(filename: str) -> str:
    """Download a single UCD file into the cache, skipping if present."""

    out_path = os.path.join(CACHE_DIR, filename)

    if os.path.exists(out_path):
        return "CACHED"

    temp_path = out_path + ".part"
    if os.path.exists(temp_path):
        os.remove(temp_path)

    with urllib.request.urlopen(URL + filename) as response, open(
        temp_path, "wb"
    ) as out_file:
        out_file.write(response.read())

    os.rename(temp_path, out_path)

T = TypeVar('T')
def progress(headline : str, jobs : list[T], run : Callable[[T], str|None], fmt : Callable[[T],str] = lambda x: str(x)):
    i = 0
    print(headline)
    for j in jobs:
        i += 1
        print(f"  ({i}/{len(jobs)}) {fmt(j)}... ", end="")
        try:
            state = run(j)
            print(state if state else "OK")
        except:
            print("ERROR")
            raise


def load() -> None:
    """Download (if needed) and parse every registered UCD file."""

    os.makedirs(CACHE_DIR, exist_ok=True)
    cache : dict[str,str] = {}
    
    progress("Downloading UCD...", PARSERS.keys(), lambda f: _download(f))

    def loadFile(fn : str) -> list[str]:
        with open(os.path.join(CACHE_DIR, fn), encoding="utf-8") as file:
            return file.read().splitlines()

    progress("Running parsers...", PARSERS.items(), lambda i: i[1](loadFile(i[0])), fmt=lambda i: i[1].__name__)
