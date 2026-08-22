# ucd.py: Interacts with the unicode character database
import os
import re
import urllib.request
from dataclasses import dataclass
from typing import Callable, Iterable, Optional, TypeVar, Iterator, List, Tuple, Dict

URL = "https://www.unicode.org/Public/UCD/latest/ucd/"
CACHE_DIR = "cache"

@dataclass(frozen=True)
class GeneralCategory:
    """A Unicode general category"""

    shorthand: str
    """The 1 or 2 character shorthand for the category"""
    full_name: str
    """The full name of the category, e.g. `Uppercase_Letter`"""

    @property
    def full_id(self) -> str:
        """The enum entry name for this category"""
        return f"UCLASS_{self.full_name.upper()}"

    @property
    def short_id(self) -> str:
        """The alternative enum entry name for this category"""
        return f"UCLASS_{self.shorthand.upper()}"

@dataclass(frozen=True)
class MinorCategory(GeneralCategory):
    """A major unicode category (e.g. `Lu`/`Uppercase_Letter`)"""

    @property
    def majorShorthand(self) -> str:
        """The shorthand for the MajorCategory that contains this category"""
        return self.shorthand[0]

@dataclass(frozen=True)
class MajorCategory(GeneralCategory):
    """A major unicode category (e.g. `L`/`Letter`)"""
    
    minor_categories : List[MinorCategory]
    """The minor categories contained in this major categories, in the declaration order from the Unicode standard"""


@dataclass(frozen=True)
class Codepoint:
    """A single assigned Unicode codepoint and its simple case mappings."""

    value: int
    simple_uppercase_mapping: int
    simple_lowercase_mapping: int
    general_category: MinorCategory

    # simple *case mappings are stored as deltas in the database to save space
    @property
    def simple_lowercase_delta(self) -> int:
        return self.simple_lowercase_mapping - self.value

    @property
    def simple_uppercase_delta(self) -> int:
        return self.simple_uppercase_mapping - self.value

    @property
    def delta(self) -> tuple[int,int]:
        return (self.simple_uppercase_delta, self.simple_lowercase_delta)

    def __str__(self) -> str:
        return "U+%04X" % value

@dataclass(frozen=True)
class UCDB:
    """ The unicode character database """

    version : List[int] 
    """ The unicode standard version as [major,minor,patch] """
    
    codepoints : List[Codepoint]
    """ The list of all codepoints, ordered by value """

    categories : Dict[str, GeneralCategory]
    """ Maps shorthands onto corresponding general categories, in declaration order """
    
    @property
    def major_categories(self) -> List[MajorCategory]:
        return [ c for c in self.categories.values() if isinstance(c, MajorCategory) ] 

    @property
    def version_string(self) -> str:
        """The unicode version as a human-readable string, e.g. `15.0.0`"""
        return f"{self.version[0]}.{self.version[1]}.{self.version[2]}"

    @property
    def max_codepoint(self) -> int:
        """ The maximum allocated codepoint """
        return max(c.value for c in self.codepoints)

ParserFn = Callable[[List[str], UCDB], None]
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

def _try_hex(text: str) -> Optional[int]:
    """Tries parsing a hexadecimal number"""

    text = text.strip()
    if not text:
        return None
    try:
        return int(text, 16)
    except ValueError:
        return None


@parser("PropertyValueAliases.txt")
def parse_general_categories(lines: List[str], db : UCDB) -> None:
    match = re.search(r"[0-9]+\.[0-9]+\.[0-9]+", lines[0])
    
    for p in match.group(0).split("."):
        db.version.append(int(p))

    for line in remove_comments(lines):
        fields = [field.strip() for field in line.split(";")]

        if(fields[0] != "gc"):
            continue

        shorthand = fields[1]
        full_name = fields[2]
        assert(len(shorthand) in [1,2])

        cat = MajorCategory(shorthand, full_name, []) if len(shorthand) == 1 else MinorCategory(shorthand, full_name)        
        db.categories[cat.shorthand] = cat
    
    # insert major -> minor references
    for c in db.categories.values():
        if not isinstance(c, MinorCategory):
            continue

        c : MinorCategory = c        
        maj : MajorCategory = db.categories[c.majorShorthand]
        maj.minor_categories.append(c)

@parser("UnicodeData.txt")
def parse_codepoints(lines: List[str], db : UCDB) -> None:
    codepoints: dict[int, Codepoint] = {}

    for raw in remove_comments(lines):
        fields = raw.split(";")
        if not fields or not fields[0].strip():
            continue

        value = int(fields[0], 16)
        upper = _try_hex(fields[12]) if len(fields) > 12 else None
        lower = _try_hex(fields[13]) if len(fields) > 13 else None

        cat = db.categories[fields[2]]
        assert(isinstance(cat, MinorCategory))

        db.codepoints.append(Codepoint(
            value = value,
            simple_uppercase_mapping = upper if upper is not None else value,
            simple_lowercase_mapping = lower if lower is not None else value,
            general_category = cat,
        ))


def _download(filename: str, lines : Dict[str, str]) -> str:
    """Download a single UCD file into the cache, skipping if present."""

    out_path = os.path.join(CACHE_DIR, filename)

    if os.path.exists(out_path):
        with open(out_path, encoding="utf-8") as file:
            lines[filename] = file.read().splitlines()

        return "CACHED"

    temp_path = out_path + ".part"
    if os.path.exists(temp_path):
        os.remove(temp_path)

    with urllib.request.urlopen(URL + filename) as response, open(temp_path, "wb") as out_file:
        r = response.read()
        out_file.write(r)
        r = r.decode('utf-8')
        lines[filename] = r.splitlines()

    os.rename(temp_path, out_path)

T = TypeVar('T')
def progress(headline : str, jobs : List[T], run : Callable[[T], str|None], fmt : Callable[[T],str] = lambda x: str(x)):
    """Runs an operation on a list of arguments, printing live status to the console."""
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


def load() -> UCDB:
    """Download (if needed) and parse every registered UCD file."""

    os.makedirs(CACHE_DIR, exist_ok=True)
    cache : dict[str,str] = {}
    db : UCDB = UCDB([], [], {})
    
    progress("Downloading UCD...", PARSERS.keys(), lambda f: _download(f, cache))
    progress("Running parsers...", PARSERS.items(), lambda i: i[1](cache[i[0]], db), fmt=lambda i: i[1].__name__)

    assert(len(db.version) > 0)
    assert(len(db.codepoints) > 0)
    assert(len(db.categories) > 0)

    return db
