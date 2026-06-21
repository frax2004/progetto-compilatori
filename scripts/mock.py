import random as rd


CITIES = tuple({
  'PMO': 'Palermo',
  'CFU': 'Cefalu',
  'MSN': 'Messina',
  "TRP": "Trapani",
  "RMA": "Roma",
  "FRZ": "Firenze",
  "MLN": "Milano",
  "TRN": "Torino",
  "BAR": "Bari",
  "CTN": "Catania",
  "VRS": "Varese",
  "VNZ": "Venezia",
  "CTZ": "Catanzaro",
  "GNV": "Genova"
}.items())

CYCLIST_SEP = '&&&'
SECTION_SEP = '%%%'
COORDINATES = tuple((rd.random() * 1_000, rd.random() * 1_000) for _ in range(2048))
NUMBERS = tuple(range(1, 1001))
SECONDS = tuple(rd.randint(10_000, 100_000) for _ in range(2048))

FIRST_NAMES = "Gianmarco Marta Francesco Maria Riccardo Sara Emilio Virginia Dennis Vanessa Bjarne Tony".split(' ')
LAST_NAMES = "Tocco Gilli Marzano Perrotti Pescini Colombo Evan Ritchie Kirby Stroustrap Stark".split(' ')

def clamp(min, x, max): return min if x < min else max if x > max else x
  
def random_subset(collection, count):
  count = clamp(0, count, len(collection))
  start = rd.randint(0, len(collection)-count)
  end = start + count
  return collection[start:end]


def happy_path(k_cities, k_cyclists, max_stages):
  PLACES = list(zip(random_subset(CITIES, k_cities), rd.choices(COORDINATES, k = k_cities)))

  POSSIBLE_NUMBERS = list(random_subset(NUMBERS, k_cyclists))
  rd.shuffle(POSSIBLE_NUMBERS)

  CYCLISTS = tuple((
    str(POSSIBLE_NUMBERS[i]),
    f"{rd.choice(FIRST_NAMES)} {rd.choice(LAST_NAMES)}",
    rd.choice(PLACES)[0][0]
  ) for i in range(k_cyclists))

  TIMINGS = tuple(
    f"{rd.choice(CYCLISTS)[0]} {rd.choice(PLACES)[0][0]} {rd.choice(SECONDS)}" 
    for _ in range(max_stages)
  )

  SECTION1 = "\n".join("{} \"{}\" ({}, {})".format(*city, *coords) for (city, coords) in PLACES)
  SECTION2 = f'\n{CYCLIST_SEP}\n'.join(
    map(lambda cyclist: "\n".join(cyclist), CYCLISTS)
  )
  SECTION3 = '\n'.join(TIMINGS)

  return f"\n{SECTION_SEP}\n".join([
    SECTION1, 
    SECTION2, 
    SECTION3
  ])

def semantic_error(k_cities, k_cyclists, max_stages):
  PLACES = list(zip(rd.choices(CITIES, k = k_cities), rd.choices(COORDINATES, k = k_cities)))

  CYCLISTS = tuple((
    str(rd.choice(NUMBERS)),
    f"{rd.choice(FIRST_NAMES)} {rd.choice(LAST_NAMES)}",
    rd.choice(CITIES)[0]
  ) for _ in range(k_cyclists))

  TIMINGS = tuple(
    f"{rd.choice(NUMBERS)} {rd.choice(CITIES)[0]} {rd.choice(SECONDS)}" 
    for _ in range(max_stages)
  )

  SECTION1 = "\n".join("{} \"{}\" ({}, {})".format(*city, *coords) for (city, coords) in PLACES)
  SECTION2 = f'\n{CYCLIST_SEP}\n'.join(
    map(lambda cyclist: "\n".join(cyclist), CYCLISTS)
  )
  SECTION3 = '\n'.join(TIMINGS)

  return f"\n{SECTION_SEP}\n".join([
    SECTION1, 
    SECTION2, 
    SECTION3
  ])


gen_table = [
  (happy_path, 4, 7, 15),
  (happy_path, 6, 3, 30),
  (happy_path, 3, 5, 4),
  (happy_path, 2, 2, 9),
  (semantic_error, 4, 7, 15),
  (semantic_error, 6, 3, 30),
  (semantic_error, 3, 5, 4),
  (semantic_error, 2, 2, 9)
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