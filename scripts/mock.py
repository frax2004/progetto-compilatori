import random as rd
from dataclasses import dataclass

@dataclass
class Coords:
  x: float
  y: float

  @staticmethod
  def random():
    return Coords(rd.random() * 1_000, rd.random() * 1_000)

  def __str__(self):
    return f'({self.x}, {self.y})'

@dataclass
class City:
  code: str
  name: str

  def __str__(self):
    return f'{self.code} "{self.name}"'

@dataclass
class CityDecl:
  city: City
  coords: Coords

  def __str__(self):
    return f'{str(self.city)} {str(self.coords)}'

@dataclass
class CyclistDecl:
  code: int
  name: str
  city: str

  def __str__(self):
    return f'{self.code}\n{self.name}\n{self.city}'

@dataclass
class StageStmt:
  code: int
  city: str
  seconds: int

  def __str__(self):
    return f'{self.code} {self.city} {self.seconds}'

CITIES = [
  City('PMO', 'Palermo'),
  City('CFU', 'Cefalu'),
  City('MSN', 'Messina'),
  City("TRP", "Trapani"),
  City("RMA", "Roma"),
  City("FRZ", "Firenze"),
  City("MLN", "Milano"),
  City("TRN", "Torino"),
  City("BAR", "Bari"),
  City("CTN", "Catania"),
  City("VRS", "Varese"),
  City("VNZ", "Venezia"),
  City("CTZ", "Catanzaro"),
  City("GNV", "Genova")
]

NUMBERS = [*range(1, 1001)]

CYCLIST_SEP = '&&&'
SECTION_SEP = '%%%'

FIRST_NAMES = "Gianmarco Marta Francesco Maria Riccardo Sara Emilio Virginia Dennis Vanessa Bjarne Tony".split(' ')
LAST_NAMES = "Tocco Gilli Marzano Perrotti Pescini Colombo Evan Ritchie Kirby Stroustrap Stark".split(' ')
SECONDS = tuple(rd.randint(10_000, 100_000) for _ in range(2048))

def random_name():
  return f'{rd.choice(FIRST_NAMES)} {rd.choice(LAST_NAMES)}'


def clamp(min, x, max): return min if x < min else max if x > max else x

def random_subset(collection, count):
  copy = [*collection]
  rd.shuffle(copy)
  return copy[0:clamp(0, count, len(copy)-1)]


def happy_path(k_cities, k_cyclists, max_stages):
  POSSIBLE_CITIES = random_subset(CITIES, k_cities)
  POSSIBLE_NUMBERS = random_subset(NUMBERS, k_cyclists)
  
  SECTION1 = {city.code: CityDecl(city, Coords.random()) for city in POSSIBLE_CITIES}
  SECTION2 = {code: CyclistDecl(code, random_name(), rd.choice([*SECTION1.keys()])) for code in POSSIBLE_NUMBERS}


  SECTION3 = []
  possible_checkpoints = [*SECTION1.keys()]
  current_states = {cyclist.code: cyclist.city for cyclist in SECTION2.values()}
  for _ in range(max_stages):
    code = rd.choice([*SECTION2.keys()])
    prob = 1.0/(len(possible_checkpoints)-1)
    weights = [prob if c != current_states[code] else 0 for c in possible_checkpoints]
    city = rd.choices(possible_checkpoints, weights)[0]
    SECTION3.append(StageStmt(code, city, rd.choice(SECONDS)))
    current_states[code] = city

  section1 = '\n'.join(map(str, SECTION1.values()))
  section2 = f'\n{CYCLIST_SEP}\n'.join(map(str, SECTION2.values()))
  section3 = '\n'.join(map(str, SECTION3))

  return f"""{section1}
{SECTION_SEP}
{section2}
{SECTION_SEP}
{section3}"""


gen_table = [
  (happy_path, 4, 7, 15),
  (happy_path, 6, 3, 30),
  (happy_path, 3, 5, 4),
  (happy_path, 2, 2, 9),
]

import subprocess

def rgb(r, g, b): return f"\033[38;2;{r};{g};{b}m"

for i, unit in enumerate(gen_table):
  inputPath = f"examples/input/{unit[0].__name__}#{i}.txt"
  outputPath = f"examples/output/{unit[0].__name__}#{i}.txt"

  with open(inputPath, 'w') as f:
    f.write(unit[0](*unit[1:]))

  print("{:3}% | {}Running test {}{}".format(round((i+1)/len(gen_table)*100), rgb(10, 255, 0), inputPath, "\033[0m"))
  subprocess.run(f".\\bin\\compiler.exe {inputPath} --output {outputPath}")

print(f"Done.\033[0m")