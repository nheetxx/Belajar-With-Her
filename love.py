import turtle
import math
import random
import time

screen = turtle.Screen()
screen.colormode(255)
screen.bgcolor("#0a0208")
screen.title("Zaya Kasur")
screen.tracer(0)

cv = screen.getcanvas()

pen = turtle.Turtle()
pen.hideturtle()
pen.penup()

skala = 14
gendut = 0.35
points = []

def bentuk(a):
    x = 16 * math.sin(a) ** 3
    y = 13 * math.cos(a) - 5 * math.cos(2*a) - 2*math.cos(3*a) - math.cos(4*a) 
    t = max(0.0, min(1.0, (4 - y) / 16))
    x *= 1 + gendut * t
    return x, y

n = 240
for i in range(n):
    a = 2 * math.pi * i / n
    x, y = bentuk(a)
    points.append((x * skala, y * skala))
for _ in range(60):
    a = random.uniform(0, 2 * math.pi)
    k = random.uniform(0.3, 0.85)
    x, y = bentuk(a)
    points.append((x * skala * k, y * skala * k))
    
warna = [(255, 105, 180), (255, 130, 195), (255, 150, 205), (240, 110, 170), (255, 120, 200), (220, 130, 220)]
teks = ["love you", "LOVE YOU", "Love you"]

def hex_(c, f=1.0):
    r, g, b = (max(0, min(255, int(v *f))) for v in c)
    return "#%02x%02x%02x" % (r, g, b)

def tulis(x, y, t, warna_hex, ukuran):
    pen.goto(x, y)
    pen.color(warna_hex)
    pen.write(t, align="center", font=("Arial", ukuran, "bold"))
    
for x, y in points:
    c = random.choice(warna)
    t = random.choice(teks)
    u = random.randint(8, 14)
    
    n0 = len(cv.find_all())
    
    for k in range(12):
        ang = 2 * math.pi * k / 12 
        tulis(x + 5 * math.cos(ang), y + 5 * math.sin(ang), t, hex_(c, 0.15), u)   
    for k in range(8):
        ang = 2 * math.pi * k / 8 
        tulis(x + 3 * math.cos(ang), y + 3 * math.sin(ang), t, hex_(c, 0.3), u)   
    for k in range(4):
        ang = 2 * math.pi * k / 4 
        tulis(x + 1.5 * math.cos(ang), y + 1.5 * math.sin(ang), t, hex_(c, 0.55), u)   
    
    for item in cv.find_all()[n0: ]:
        cv.tag_lower(item)
    inti = tuple(min(255, v + 90) for v in c)
    tulis(x, y, t, hex_(inti), u)
    
    screen.update()
    time.sleep(0.03)

nama = "Zeeyaa"
time.sleep(0.6)

def tulis_besar(x, y, t, ukuran, c):
    n0 = len(cv.find_all())
    for r, f, jml in [(9, 0.18, 16), (6, 0.32, 12), (3, 0.55, 8)]:
        ang = 2 * math.pi * k / jml
        tulis(x + r * math.cos(ang), y + r * math.sin(ang), t, hex_(c, f), ukuran) 
    for item in cv.find_all()[n0: ]:
        cv.tag_lower(item)
    tulis(x, y, t, hex_(c), ukuran)
    screen.update()
        
tulis_besar(0, -5, "I Love You", 46, (255, 105, 180))
time.sleep(0.8)
tulis_besar(0, -62, nama, 40, (255, 150, 205))
        
turtle.done()