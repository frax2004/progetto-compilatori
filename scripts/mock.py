import random as rd


CITIES = tuple({
  'Palermo': 'PMO',
  'Cefalù': 'CFU',
  'Messina': 'MSN',
  "Trapani": "TRP",
  "Roma": "RMA",
  "Firenze": "FRZ",
  "Milano": "MLN",
  "Torino": "TRN",
  "Bari": "BAR",
  "Catania": "CTN",
  "Varese": "VRS",
  "Venezia": "VNZ",
  "Catanzaro": "CTZ",
  "Genova": "GNV"
}.items())

CYCLIST_SEP = '&&&'
SECTION_SEP = '%%%'
COORDINATES = tuple((rd.random() * 1_000, rd.random() * 1_000) for _ in range(2048))
NUMBERS = tuple(range(1, 1001))
SECONDS = tuple(rd.randint(10_000, 100_000) for _ in range(2048))

FIRST_NAMES = "Gianmarco Marta Francesco Maria Riccardo Sara Emilio Virginia Dennis Vanessa Bjarne Tony".split(' ')
LAST_NAMES = "Tocco Gilli Marzano Perrotti Pescini Colombo Evan Ritchie Kirby Stroustrap Stark".split(' ')

  
def gen_correct():

  PLACES = tuple(
    "{1} \"{0}\" ({2}, {3})".format(*rd.choice(CITIES), *rd.choice(COORDINATES))
    for _ in range(rd.randint(10, 50))
  )
  
  CYCLISTS = tuple((
    str(rd.choice(NUMBERS)), 
    f"{rd.choice(FIRST_NAMES)} {rd.choice(LAST_NAMES)}",
    rd.choice(CITIES)[1]
  ) for _ in range(rd.randint(10, 50)))

  TIMINGS = tuple(
    f"{rd.choice(CYCLISTS)[0]} {rd.choice(CITIES)[1]} {rd.choice(SECONDS)}" 
    for _ in range(rd.randint(10, 50))
  )

  SECTION1 = "\n".join(PLACES)
  SECTION2 = f'\n{CYCLIST_SEP}\n'.join(
    map(lambda cyclist: "\n".join(cyclist), CYCLISTS)
  )
  SECTION3 = '\n'.join(TIMINGS)

  return f"\n{SECTION_SEP}\n".join([
    SECTION1, 
    SECTION2, 
    SECTION3
  ])

with open("examples/correct_input.txt", 'w') as f:
  f.write(gen_correct())